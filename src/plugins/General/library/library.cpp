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
#include <qmmpui/uihelper.h>
#include <qmmp/soundcore.h>
//#include "historywindow.h"
#include "library.h"

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
                         "AudioInfo BLOB, URL BLOB, FilePath BLOB)");

    if(!ok)
        qWarning("Library: unable to create table, error: %s", qPrintable(query.lastError().text()));

    return ok;
}

/*void Library::saveTrack()
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    if(!db.isOpen() || m_trackInfo.isEmpty())
        return;

    QSqlQuery query(db);
    query.prepare("INSERT INTO track_library VALUES(NULL, CURRENT_TIMESTAMP, :title, :artist, :albumartist, :album, :comment,"
                  ":genre, :composer, :year, :track, :discnumber, :duration, :url);");
    query.bindValue(":title", m_trackInfo.value(Qmmp::TITLE));
    query.bindValue(":artist", m_trackInfo.value(Qmmp::ARTIST));
    query.bindValue(":albumartist", m_trackInfo.value(Qmmp::ALBUMARTIST));
    query.bindValue(":album", m_trackInfo.value(Qmmp::ALBUM));
    query.bindValue(":comment", m_trackInfo.value(Qmmp::COMMENT));
    query.bindValue(":genre", m_trackInfo.value(Qmmp::GENRE));
    query.bindValue(":composer", m_trackInfo.value(Qmmp::COMPOSER));
    query.bindValue(":year", m_trackInfo.value(Qmmp::YEAR));
    query.bindValue(":track", m_trackInfo.value(Qmmp::TRACK));
    query.bindValue(":discnumber", m_trackInfo.value(Qmmp::DISCNUMBER));
    query.bindValue(":duration", m_trackInfo.duration());
    query.bindValue(":url", m_trackInfo.path());
    bool ok = query.exec();

    if(!ok)
        qWarning("Library: unable to save track, error: %s", qPrintable(query.lastError().text()));
    else
        qDebug("Library: track '%s' has been added to history",
               qPrintable(m_trackInfo.value(Qmmp::ARTIST) + " - " + m_trackInfo.value(Qmmp::TITLE)));
}*/
