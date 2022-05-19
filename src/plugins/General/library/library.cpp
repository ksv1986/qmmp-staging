/***************************************************************************
 *   Copyright (C) 2020-2022 by Ilya Kotov                                 *
 *   forkotov02@ya.ru                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QAction>
#include <QApplication>
#include <QFileInfo>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QHash>
#include <QtDebug>
#include <algorithm>
#include <qmmp/qmmp.h>
#include <qmmp/metadatamanager.h>
#include <qmmpui/uihelper.h>
#include "librarymodel.h"
#include "librarywidget.h"
#include "library.h"

#define CONNECTION_NAME "qmmp_library"

Library::Library(QPointer<LibraryWidget> *libraryWidget, QObject *parent) :
    QThread(parent),
    m_libraryWidget(libraryWidget)
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", CONNECTION_NAME);
        if(db.isValid() && !db.isOpen())
        {
            db.setDatabaseName(Qmmp::configDir() + "/" + "library.sqlite");
            db.open();
            if(createTables())
                qDebug("Library: database initialization finished");
            else
                qWarning("Library: unable to create table");
        }
    }
    QSqlDatabase::removeDatabase(CONNECTION_NAME);

    QSettings settings;
    m_dirs = settings.value("Library/dirs").toStringList();

    m_showAction = new QAction(QIcon::fromTheme("text-x-generic"), tr("Library"), this);
    m_showAction->setShortcut(tr("Alt+L"));
    UiHelper::instance()->addAction(m_showAction, UiHelper::TOOLS_MENU);
    connect(m_showAction, SIGNAL(triggered()), SLOT(showLibraryWindow()));
    if(!m_libraryWidget->isNull() && !m_libraryWidget->data()->isWindow())
        m_showAction->setVisible(false);

    QAction *refreshAction = new QAction(QIcon::fromTheme("view-refresh"), tr("Update library"), this);
    UiHelper::instance()->addAction(refreshAction, UiHelper::TOOLS_MENU);
    connect(refreshAction, SIGNAL(triggered()), SLOT(startDirectoryScanning()));

    connect(this, &QThread::finished, this, [=] {
        if(!m_libraryWidget->isNull())
        {
            m_libraryWidget->data()->setBusyMode(false);
            m_libraryWidget->data()->refresh();
        }
    }, Qt::QueuedConnection);

    if(settings.value("Library/recreate_db", false).toBool())
    {
        settings.setValue("Library/recreate_db", false);
        {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", CONNECTION_NAME);
            db.setDatabaseName(Qmmp::configDir() + "/" + "library.sqlite");
            db.open();
            db.exec("DELETE FROM track_library");
            db.exec("REINDEX track_library");
            db.exec("VACUUM");
            db.close();
        }
        QSqlDatabase::removeDatabase(CONNECTION_NAME);
        startDirectoryScanning();
    }
}

Library::~Library()
{
    if(isRunning())
    {
        m_stopped = true;
        wait();
    }

    if(QSqlDatabase::contains(CONNECTION_NAME))
    {
        QSqlDatabase::database(CONNECTION_NAME).close();
        QSqlDatabase::removeDatabase(CONNECTION_NAME);
    }
}

QAction *Library::showAction() const
{
    return m_showAction;
}

void Library::showLibraryWindow()
{
    if(m_libraryWidget->isNull())
        *m_libraryWidget = new LibraryWidget(true, qApp->activeWindow());

    if(m_libraryWidget->data()->isWindow())
        m_libraryWidget->data()->show();

    if(isRunning())
        m_libraryWidget->data()->setBusyMode(true);
}

void Library::startDirectoryScanning()
{
    if(isRunning())
        return;

    MetaDataManager::instance()->prepareForAnotherThread();
    m_filters = MetaDataManager::instance()->nameFilters();
    start(QThread::IdlePriority);
    if(!m_libraryWidget->isNull())
        m_libraryWidget->data()->setBusyMode(true);
}

void Library::run()
{
    scanDirectories(m_dirs);
}

bool Library::createTables()
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    if(!db.isOpen())
        return false;

    QSqlQuery query(db);
    bool ok = query.exec("CREATE TABLE IF NOT EXISTS track_library("
                         "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                         "Timestamp TIMESTAMP NOT NULL,"
                         "Title TEXT, Artist TEXT, AlbumArtist TEXT, Album TEXT, Comment TEXT, Genre TEXT, Composer TEXT,"
                         "Year INTEGER, Track INTEGER, DiscNumber TEXT, Duration INTEGER, "
                         "AudioInfo BLOB, URL TEXT, FilePath TEXT, SearchString TEXT)");

    if(!ok)
    {
        qWarning("Library: unable to create table, error: %s", qPrintable(query.lastError().text()));
        return false;
    }

    ok = query.exec("CREATE TABLE IF NOT EXISTS ignored_files(ID INTEGER PRIMARY KEY AUTOINCREMENT, FilePath TEXT UNIQUE)");

    if(!ok)
        qWarning("Library: unable to create ignored file list, error: %s", qPrintable(query.lastError().text()));

    return ok;
}

void Library::addTrack(TrackInfo *track, const QString &filePath)
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    if(!db.isOpen())
        return;

    QSqlQuery query(db);
    query.prepare("INSERT OR REPLACE INTO track_library VALUES("
                  "(SELECT ID FROM track_library WHERE URL = :url), "
                  ":timestamp, "
                  ":title, :artist, :albumartist, :album, :comment, :genre, :composer, "
                  ":year, :track, :discnumber, :duration, "
                  ":audioinfo, :url, :filepath, :searchstring)");

    QString title = track->value(Qmmp::TITLE).isEmpty() ? track->path().section("/", -1) : track->value(Qmmp::TITLE);
    QString album = track->value(Qmmp::ALBUM).isEmpty() ? tr("Unknown") : track->value(Qmmp::ALBUM);
    QString artist = track->value(Qmmp::ARTIST).isEmpty() ? tr("Unknown") : track->value(Qmmp::ARTIST);

    query.bindValue(":timestamp", QFileInfo(filePath).lastModified());
    query.bindValue(":title", title);
    query.bindValue(":artist", artist);
    query.bindValue(":albumartist", track->value(Qmmp::ALBUMARTIST));
    query.bindValue(":album", album);
    query.bindValue(":comment", track->value(Qmmp::COMMENT));
    query.bindValue(":genre", track->value(Qmmp::GENRE));
    query.bindValue(":composer", track->value(Qmmp::COMPOSER));
    query.bindValue(":year", track->value(Qmmp::YEAR));
    query.bindValue(":track", track->value(Qmmp::TRACK));
    query.bindValue(":discnumber", track->value(Qmmp::DISCNUMBER));
    query.bindValue(":duration", track->duration());
    query.bindValue(":audioinfo", serializeAudioInfo(track->properties()));
    query.bindValue(":url", track->path());
    query.bindValue(":filepath", filePath);
    query.bindValue(":searchstring", QString("%1|||%2|||%3").arg(artist).arg(album).arg(title).toLower());
    if(!query.exec())
        qWarning("Library: exec error: %s", qPrintable(query.lastError().text()));
}

QByteArray Library::serializeAudioInfo(const QMap<Qmmp::TrackProperty, QString> &properties)
{
    QJsonObject obj;
    QMap<Qmmp::TrackProperty, QString>::const_iterator it = properties.cbegin();
    while(it != properties.cend())
    {
        QString value = properties[it.key()];

        switch(it.key())
        {
        case Qmmp::BITRATE:
            obj.insert("bitrate", value.toInt());
            break;
        case Qmmp::SAMPLERATE:
            obj.insert("samplerate", value.toInt());
            break;
        case Qmmp::CHANNELS:
            obj.insert("channels", value.toInt());
            break;
        case Qmmp::BITS_PER_SAMPLE:
            obj.insert("bitsPerSample", value.toInt());
            break;
        case Qmmp::FORMAT_NAME:
            obj.insert("formatName", value);
            break;
        case Qmmp::DECODER:
            obj.insert("decoder", value);
            break;
        case Qmmp::FILE_SIZE:
            obj.insert("fileSize", value.toLongLong());
            break;
        default:
            ;
        }

        ++it;
    }

    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

bool Library::scanDirectories(const QStringList &paths)
{
    m_stopped = false;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", CONNECTION_NAME);
        db.setDatabaseName(Qmmp::configDir() + "/" + "library.sqlite");
        db.open();

        readIgnoredFiles();

        QSqlQuery query(db);
        query.exec("PRAGMA journal_mode = WAL");
        query.exec("PRAGMA synchronous = NORMAL");

        for(const QString &path : qAsConst(paths))
        {
            addDirectory(path);
            if(m_stopped)
                break;
        }

        if(!m_stopped)
            removeMissingFiles(paths);

        db.close();
    }

    QSqlDatabase::removeDatabase(CONNECTION_NAME);
    qDebug("Library: directory scan finished");
    return !m_stopped;
}

void Library::addDirectory(const QString &s)
{
    QList<TrackInfo *> tracks;
    QHash<const TrackInfo *, QString> filePathHash;
    QStringList ignoredPaths;

    QDir dir(s);
    dir.setFilter(QDir::Files | QDir::Hidden);
    dir.setSorting(QDir::Name);
    QFileInfoList l = dir.entryInfoList(m_filters);

    for(const QFileInfo &info : qAsConst(l))
    {
        if(!checkFile(info) && !m_ignoredFiles.contains(info.filePath()))
        {
            QStringList paths;
            const QList<TrackInfo *> pl = MetaDataManager::instance()->createPlayList(info.absoluteFilePath(), TrackInfo::AllParts, &paths);

            //save local file path
            for(const TrackInfo *t : qAsConst(pl))
                filePathHash.insert(t, info.absoluteFilePath());

            tracks << pl;
            ignoredPaths << paths;

        }

        if (m_stopped)
        {
            qDeleteAll(tracks);
            tracks.clear();
            return;
        }
    }

    removeIgnoredTracks(&tracks, ignoredPaths);

    for(TrackInfo *info : qAsConst(tracks))    
        addTrack(info, filePathHash.value(info));

    qDeleteAll(tracks);
    tracks.clear();

    updateIgnoredFiles(ignoredPaths);

    //filter directories
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Name);
    l.clear();
    l = dir.entryInfoList();

    for(const QFileInfo &i : qAsConst(l))
    {
        addDirectory(i.absoluteFilePath());
        if (m_stopped)
            return;
    }
}

void Library::removeMissingFiles(const QStringList &paths)
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    if(!db.isOpen())
        return;

    QSqlQuery query(db);
    if(!query.exec("SELECT FilePath,URL FROM track_library"))
    {
        qWarning("Library: exec error: %s", qPrintable(query.lastError().text()));
        return;
    }

    QString previousPath;

    while(query.next())
    {
        QString path = query.value(0).toString();
        QString url = query.value(1).toString();
        if(previousPath == path)
            continue;

        previousPath = path;

        if(!QFile::exists(path) || //remove missing or disabled file paths
                !std::any_of(paths.cbegin(), paths.cend(), [path](const QString &p){ return path.startsWith(p); } ) ||
                (!url.contains("://") && m_ignoredFiles.contains(url))) //remove ignored files
        {
            qDebug("Library: removing '%s' from library", qPrintable(path));
            QSqlQuery rmQuery(db);
            rmQuery.prepare("DELETE FROM track_library WHERE FilePath = :filepath");
            rmQuery.bindValue(":filepath", path);
            if(!rmQuery.exec())
            {
                qWarning("Library: exec error: %s", qPrintable(query.lastError().text()));
                return;
            }
        }
    }

    if(!query.exec("SELECT FilePath FROM ignored_files"))
    {
        qWarning("Library: exec error: %s", qPrintable(query.lastError().text()));
        return;
    }

    while(query.next())
    {
        QString path = query.value(0).toString();

        if(!QFile::exists(path) || //remove missing or disabled file paths
                !std::any_of(paths.cbegin(), paths.cend(), [path](const QString &p){ return path.startsWith(p); } ))
        {
            qDebug("Library: removing '%s' from ignored files", qPrintable(path));
            QSqlQuery rmQuery(db);
            rmQuery.prepare("DELETE FROM ignored_files WHERE FilePath = :filepath");
            rmQuery.bindValue(":filepath", path);
            if(!rmQuery.exec())
            {
                qWarning("Library: exec error: %s", qPrintable(query.lastError().text()));
                return;
            }
        }
    }
}

bool Library::checkFile(const QFileInfo &info)
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    if(!db.isOpen())
        return false;

    QSqlQuery query(db);
    query.prepare("SELECT Timestamp FROM track_library WHERE FilePath = :filepath");
    query.bindValue(":filepath", info.absoluteFilePath());
    if(!query.exec())
    {
        qWarning("Library: exec error: %s", qPrintable(query.lastError().text()));
        return false;
    }
    if(!query.next())
        return false;

    return info.lastModified() == query.value("Timestamp").toDateTime();
}

void Library::removeIgnoredTracks(QList<TrackInfo *> *tracks, const QStringList &ignoredPaths)
{
    if(ignoredPaths.isEmpty())
        return;

    QList<TrackInfo *>::iterator it = tracks->begin();
    while(it != tracks->end())
    {
        if(ignoredPaths.contains((*it)->path()))
        {
            delete (*it);
            it = tracks->erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void Library::updateIgnoredFiles(const QStringList &paths)
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    if(!db.isOpen())
        return;

    for(const QString &path : qAsConst(paths))
    {
        QSqlQuery query(db);
        query.prepare("INSERT OR REPLACE INTO ignored_files VALUES((SELECT ID FROM track_library WHERE FilePath = :filepath), :filepath)");
        query.bindValue(":filepath", path);
        if(!query.exec())
        {
            qWarning("Library: exec error: %s", qPrintable(query.lastError().text()));
            break;
        }
    }
}

void Library::readIgnoredFiles()
{
    m_ignoredFiles.clear();

    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    if(!db.isOpen())
        return;

    QSqlQuery query(db);
    if(!query.exec("SELECT FilePath FROM ignored_files"))
    {
        qWarning("Library: exec error: %s", qPrintable(query.lastError().text()));
        return;
    }

    while(query.next())
        m_ignoredFiles.insert(query.value(0).toString());
}
