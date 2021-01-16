/***************************************************************************
 *   Copyright (C) 2020-2021 by Ilya Kotov                                 *
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
#include <QtConcurrent>
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
    QObject(parent),
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
                qWarning("Library: plugin is disabled");
        }
    }
    QSqlDatabase::removeDatabase(CONNECTION_NAME);

    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_dirs = settings.value("Library/dirs").toStringList();

    QAction *action = new QAction(QIcon::fromTheme("view-refresh"), tr("Update library"), this);
    UiHelper::instance()->addAction(action, UiHelper::TOOLS_MENU);
    connect(action, SIGNAL(triggered()), SLOT(startDirectoryScanning()));

    connect(&m_watcher, &QFutureWatcher<bool>::finished, [=] {
        if(!m_libraryWidget->isNull())
        {
            m_libraryWidget->data()->setEnabled(true);
            m_libraryWidget->data()->refresh();
        }
    });
}

Library::~Library()
{
    if(m_future.isRunning())
    {
        m_stopped = true;
        m_future.waitForFinished();
    }

    if(QSqlDatabase::contains(CONNECTION_NAME))
    {
        QSqlDatabase::database(CONNECTION_NAME).close();
        QSqlDatabase::removeDatabase(CONNECTION_NAME);
    }
}

bool Library::isRunning() const
{
    return m_future.isRunning();
}

void Library::showLibraryWindow()
{
    /*if(!m_historyWindow)
        m_historyWindow = new HistoryWindow(QSqlDatabase::database(CONNECTION_NAME), qApp->activeWindow());
    m_historyWindow->show();
    m_historyWindow->activateWindow();*/
}

void Library::startDirectoryScanning()
{
    if(m_future.isRunning())
        return;

    m_filters = MetaDataManager::instance()->nameFilters();
    m_future = QtConcurrent::run(this, &Library::scanDirectories, m_dirs);
    m_watcher.setFuture(m_future);
    if(!m_libraryWidget->isNull())
        m_libraryWidget->data()->setEnabled(false);
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
                         "Year INTEGER, Track INTEGER, DiscNumber INTEGER, Duration INTEGER, "
                         "AudioInfo BLOB, URL TEXT, FilePath TEXT, SearchString TEXT)");

    if(!ok)
        qWarning("Library: unable to create table, error: %s", qPrintable(query.lastError().text()));

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

    query.bindValue(":timestamp", QFileInfo(filePath).lastModified());
    query.bindValue(":title", track->value(Qmmp::TITLE));
    query.bindValue(":artist", track->value(Qmmp::ARTIST));
    query.bindValue(":albumartist", track->value(Qmmp::ALBUMARTIST));
    query.bindValue(":album", track->value(Qmmp::ALBUM));
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
    query.bindValue(":searchstring", QString("%1|||%2|||%3").arg(track->value(Qmmp::ARTIST))
                    .arg(track->value(Qmmp::ALBUM)).arg(track->value(Qmmp::TITLE)).toLower());
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

        QSqlQuery query(db);
        query.exec("PRAGMA journal_mode = WAL");
        query.exec("PRAGMA synchronous = NORMAL");

        for(const QString &path : qAsConst(paths))
        {
            addDirectory(path);
            if(m_stopped)
            {
                db.close();
                QSqlDatabase::removeDatabase(CONNECTION_NAME);
                return false;
            }
        }

        removeMissingFiles(paths);
        db.close();
    }

    QSqlDatabase::removeDatabase(CONNECTION_NAME);
    qDebug("Library: directory scan finished");
    return true;
}

void Library::addDirectory(const QString &s)
{
    QList<TrackInfo *> tracks;
    QHash<const TrackInfo *, QString> filePathHash;
    QStringList ignoredPaths;

    QDir dir(s);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Name);
    QFileInfoList l = dir.entryInfoList(m_filters);

    for(const QFileInfo &info : qAsConst(l))
    {
        if(!checkFile(info))
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

    //filter directories
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Name);
    l.clear();
    l = dir.entryInfoList();

    for (int i = 0; i < l.size(); ++i)
    {
        QFileInfo fileInfo = l.at(i);
        addDirectory(fileInfo.absoluteFilePath());
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
    if(!query.exec("SELECT FilePath FROM track_library"))
    {
        qWarning("Library: exec error: %s", qPrintable(query.lastError().text()));
        return;
    }

    QString previousPath;

    while (query.next())
    {
        QString path = query.value(0).toString();
        if(previousPath == path)
            continue;

        previousPath = path;

        if(!QFile::exists(path) || //remove missing or disabled file paths
                !std::any_of(paths.cbegin(), paths.cend(), [path](const QString &p){ return path.startsWith(p); } ))
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
