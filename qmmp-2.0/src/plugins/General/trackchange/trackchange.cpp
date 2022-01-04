/***************************************************************************
 *   Copyright (C) 2013-2021 by Ilya Kotov                                 *
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
#include <QApplication>
#include <QProcess>
#include <qmmp/soundcore.h>
#include <qmmpui/uihelper.h>
#include <qmmpui/playlistmodel.h>
#include <qmmpui/playlistmanager.h>
#include <qmmpui/playlistitem.h>
#include <qmmpui/mediaplayer.h>
#include <qmmpui/metadataformatter.h>
#include "trackchange.h"

TrackChange::TrackChange(QObject *parent) : QObject(parent)
{
    m_core = SoundCore::instance();
    m_plManager = PlayListManager::instance();
    connect(m_core, SIGNAL(stateChanged(Qmmp::State)), SLOT(onStateChanged(Qmmp::State)));
    connect(m_core, SIGNAL(trackInfoChanged()), SLOT(onTrackInfoChanged()));
    connect(m_core, SIGNAL(finished()), SLOT(onFinised()));
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("TrackChange");
    m_newTrackCommand = settings.value("new_track_command").toString();
    m_endOfTrackCommand = settings.value("end_of_track_command").toString();
    m_endOfPlCommand = settings.value("end_of_pl_command").toString();
    m_titleChangeCommand = settings.value("title_change_command").toString();
    m_appStartupCommand = settings.value("application_startup_command").toString();
    m_appExitCommand = settings.value("application_exit_command").toString();
    settings.endGroup();

    connect(qApp, SIGNAL(aboutToQuit()), SLOT(onAppExit()));
    onAppStartup();
}

TrackChange::~TrackChange()
{}

void TrackChange::onStateChanged(Qmmp::State state)
{
    switch (state)
    {
    case Qmmp::Playing:
        break;
    default:
        m_prevInfo.clear();
    }
}

void TrackChange::onTrackInfoChanged()
{
    TrackInfo info = m_core->trackInfo();
    if(m_prevInfo.metaData() != info.metaData())
    {
        if(m_prevInfo.path() == info.path())
        {
            if(!m_titleChangeCommand.isEmpty())
            {
                qDebug("TrackChange: starting title change command..");
                executeCommand(info, m_titleChangeCommand);
            }
        }
        else
        {
            if(!m_newTrackCommand.isEmpty())
            {
                qDebug("TrackChange: starting new track command..");
                executeCommand(info, m_newTrackCommand);
            }
        }
    }
    m_prevInfo = info;
}

void TrackChange::onFinised()
{
    if(!m_endOfTrackCommand.isEmpty())
    {
        qDebug("TrackChange: starting end of track command..");
        executeCommand(m_prevInfo, m_endOfTrackCommand);
    }
    if(!m_endOfPlCommand.isEmpty() && !m_plManager->currentPlayList()->nextTrack())
    {
        qDebug("TrackChange: starting end of playlist command..");
        executeCommand(m_prevInfo, m_endOfPlCommand);
    }
}

void TrackChange::onAppStartup()
{
    if(QApplication::allWindows().count() == 1 && !m_appStartupCommand.isEmpty()) //detect startup
    {
#ifdef Q_OS_WIN
        QProcess::startDetached(QString("cmd.exe /C %1").arg(m_appStartupCommand));
#else
        QStringList args = { "-c", m_appStartupCommand };
        QProcess::startDetached("sh", args);
#endif
    }
}

void TrackChange::onAppExit()
{
    if(!m_appExitCommand.isEmpty())
    {
#ifdef Q_OS_WIN
        QProcess::startDetached(QString("cmd.exe /C %1").arg(m_appExitCommand));
#else
        QStringList args = { "-c", m_appExitCommand };
        QProcess::startDetached("sh", args);
#endif
    }
}

bool TrackChange::executeCommand(const TrackInfo &info, const QString &format)
{
    TrackInfo tmp = info;
    QMap<Qmmp::MetaData, QString> metaData = tmp.metaData();
    QMap<Qmmp::MetaData, QString>::iterator it = metaData.begin();
    while(it != metaData.end())
    {
#ifdef Q_OS_WIN
        it.value() = it.value().replace("'", "^'");
#else
        it.value() = it.value().replace("'", "'\\''");
#endif
        ++it;
    }
    tmp.setValues(metaData);
    MetaDataFormatter formatter(format);
    QString command = formatter.format(tmp);
#ifdef Q_OS_WIN
    bool ok = QProcess::startDetached(QString("cmd.exe /C %1").arg(command));
#else
    QStringList args = { "-c", command };
    bool ok = QProcess::startDetached("sh", args);
#endif
    if(!ok)
        qWarning("TrackChange: unable to start command '%s'", qPrintable(command));
    return ok;
}
