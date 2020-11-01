/***************************************************************************
 *   Copyright (C) 2017-2020 by Ilya Kotov                                 *
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
#include <qmmpui/uihelper.h>
#include <qmmp/soundcore.h>
//#include "historywindow.h"
#include "library.h"

#define CONNECTION_NAME "qmmp_library"

Library::Library(QObject *parent) : QObject(parent)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", CONNECTION_NAME);
    if(db.isValid() && !db.isOpen())
    {
        db.setDatabaseName(Qmmp::configDir() + "/" + "library.sqlite");
        db.open();
        if(createTables())
        {
            QSqlQuery query(db);
            query.exec("PRAGMA journal_mode = WAL");
            query.exec("PRAGMA synchronous = NORMAL");
            qDebug("Library: database initialization finished");
        }
        else
        {
            db.close();
            qWarning("Library: plugin is disabled");
        }
    }

//    QAction *action = new QAction(tr("History"), this);
//    action->setIcon(QIcon::fromTheme("text-x-generic"));
//    UiHelper::instance()->addAction(action, UiHelper::TOOLS_MENU);
//    connect(action, SIGNAL(triggered()), SLOT(showHistoryWindow()));
}

Library::~Library()
{
    if(QSqlDatabase::contains(CONNECTION_NAME))
    {
        QSqlDatabase::database(CONNECTION_NAME).close();
        QSqlDatabase::removeDatabase(CONNECTION_NAME);
    }
}

void Library::showLibraryWindow()
{
    /*if(!m_historyWindow)
        m_historyWindow = new HistoryWindow(QSqlDatabase::database(CONNECTION_NAME), qApp->activeWindow());
    m_historyWindow->show();
    m_historyWindow->activateWindow();*/
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
                         "Year INTEGER, Track INTEGER, DiscNumer INTEGER, Duration INTEGER, "
                         "AudioInfo BLOB, URL TEXT, FilePath TEXT)");

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
    query.prepare("INSERT INTO track_library VALUES("
                  "NULL, :timestamp, "
                  ":title, :artist, :albumartist, :album TEXT, :comment, :genre, :composer, "
                  ":year, :track, :discnumer INTEGER, :duration, "
                  ":audioinfo, :url, :filepath)");

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
}

QByteArray Library::serializeAudioInfo(const QMap<Qmmp::TrackProperty, QString> &properties)
{
    QJsonObject obj;
    QMap<Qmmp::TrackProperty, QString>::const_iterator it = properties.cbegin();
    while(it != properties.cend())
    {
        QString value =  properties[it.key()];

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
