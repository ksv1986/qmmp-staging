/***************************************************************************
 *   Copyright (C) 2008-2018 by Ilya Kotov                                 *
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

#include <QMenu>
#include <QSettings>
#include <QDir>
#include <QTimer>
#include <QCoreApplication>
#include <QWheelEvent>
#include <QEvent>
#include <QStyle>
#include <QApplication>
#include <qmmp/soundcore.h>
#include <qmmpui/mediaplayer.h>
#include <qmmpui/uihelper.h>
#include "qmmptrayicon.h"
#include "statusicon.h"

StatusIcon::StatusIcon(QObject *parent) : QObject(parent)
{
    m_tray = new QmmpTrayIcon(this);
    connect(m_tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
    //m_tray->show();
    m_core = SoundCore::instance();
    m_player = MediaPlayer::instance();
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Tray");
    m_showMessage = settings.value("show_message",false).toBool();
    m_messageDelay = settings.value("message_delay", 2000).toInt();
    m_hideToTray = settings.value("hide_on_close", false).toBool();
    m_useStandardIcons = settings.value("use_standard_icons",false).toBool();
    m_showToolTip = settings.value("show_tooltip",true).toBool();
    m_splitFileName = settings.value("split_file_name",true).toBool();
#ifdef QMMP_WS_X11
    m_toolTipTemplate = settings.value("tooltip_template", DEFAULT_TEMPLATE).toString();
#else
    m_toolTipTemplate = "%p%if(%p&%t, - ,)%t";
#endif
    m_toolTipFormatter.setPattern(m_toolTipTemplate);
    m_messageFormatter.setPattern("%p%if(%p&%t, - ,)%t");
    if(m_useStandardIcons)
        m_tray->setIcon(QApplication::style ()->standardIcon(QStyle::SP_MediaStop));
    else
        m_tray->setIcon(QIcon(":/tray_stop.png"));
    settings.endGroup();
    //actions
    m_menu = new QMenu();
    QIcon playIcon = QApplication::style()->standardIcon(QStyle::SP_MediaPlay);
    QIcon pauseIcon = QApplication::style()->standardIcon(QStyle::SP_MediaPause);
    QIcon stopIcon = QApplication::style()->standardIcon(QStyle::SP_MediaStop);
    QIcon nextIcon = QApplication::style()->standardIcon(QStyle::SP_MediaSkipForward);
    QIcon previousIcon = QApplication::style()->standardIcon(QStyle::SP_MediaSkipBackward);
    QIcon exitIcon = QIcon::fromTheme("application-exit");
    m_menu->addAction(playIcon,tr("Play"), m_player, SLOT(play()));
    m_menu->addAction(pauseIcon,tr("Pause"), m_core, SLOT(pause()));
    m_menu->addAction(stopIcon,tr("Stop"), m_core, SLOT(stop()));
    m_menu->addSeparator();
    m_menu->addAction(nextIcon, tr("Next"), m_player, SLOT(next()));
    m_menu->addAction(previousIcon, tr("Previous"), m_player, SLOT(previous()));
    m_menu->addSeparator();
    m_menu->addAction(exitIcon, tr("Exit"), UiHelper::instance(), SLOT(exit()));
    m_tray->setContextMenu(m_menu);
    m_tray->show();
    connect (m_core, SIGNAL(trackInfoChanged ()), SLOT(showMetaData()));
    connect (m_core, SIGNAL(stateChanged (Qmmp::State)), SLOT(setState(Qmmp::State)));
    setState(m_core->state()); //update state
    if (m_core->state() == Qmmp::Playing) //show test message
        QTimer::singleShot(1500, this, SLOT(showMetaData()));
}

StatusIcon::~StatusIcon()
{
    delete m_menu;
}

void StatusIcon::setState(Qmmp::State state)
{
    switch ((uint) state)
    {
    case Qmmp::Playing:
    {
         if(m_useStandardIcons)
            m_tray->setIcon(QApplication::style ()->standardIcon(QStyle::SP_MediaPlay));
        else
            m_tray->setIcon (QIcon(":/tray_play.png"));
        break;
    }
    case Qmmp::Paused:
    {
         if(m_useStandardIcons)
            m_tray->setIcon(QApplication::style ()->standardIcon(QStyle::SP_MediaPause));
        else
            m_tray->setIcon (QIcon(":/tray_pause.png"));
        break;
    }
    case Qmmp::Stopped:
    {
        if(m_useStandardIcons)
            m_tray->setIcon(QApplication::style ()->standardIcon(QStyle::SP_MediaStop));
        else
            m_tray->setIcon (QIcon(":/tray_stop.png"));
        if(m_showToolTip)
            m_tray->setToolTip(tr("Stopped"));
        break;
    }
    }
}

void StatusIcon::showMetaData()
{
    TrackInfo info = m_core->trackInfo();
    if(m_splitFileName && info.value(Qmmp::TITLE).isEmpty() && !info.path().contains("://"))
    {
        QString name = QFileInfo(info.path()).completeBaseName();
        if(name.contains("-"))
        {
            info.setValue(Qmmp::TITLE, name.section('-',1,1).trimmed());
            if(info.value(Qmmp::ARTIST).isEmpty())
                info.setValue(Qmmp::ARTIST, name.section('-',0,0).trimmed());
        }
    }

    QString message = m_messageFormatter.format(info);
    if (message.isEmpty())
        message = info.path().section('/',-1);

    if (m_showMessage)
        m_tray->showMessage (tr("Now Playing"), message,
                             QSystemTrayIcon::Information, m_messageDelay);

    if(m_showToolTip)
    {
        message = m_toolTipFormatter.format(info);
        if(message.isEmpty())
            message = info.path().section('/',-1);
        m_tray->setToolTip(message);
    }
}

void StatusIcon::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger)
        UiHelper::instance()->toggleVisibility();
    else if (reason == QSystemTrayIcon::MiddleClick)
    {
        if (SoundCore::instance()->state() == Qmmp::Stopped)
            m_player->play();
        else
            m_core->pause();
    }
}
