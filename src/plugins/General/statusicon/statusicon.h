/***************************************************************************
 *   Copyright (C) 2008-2021 by Ilya Kotov                                 *
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
#ifndef STATUSICON_H
#define STATUSICON_H

#include <QMap>
#include <qmmpui/general.h>
#include <qmmp/qmmp.h>
#include <qmmpui/metadataformatter.h>
#include "qmmptrayicon.h"

#define DEFAULT_TEMPLATE "<b>%if(%t,%t,%f)</b>\n%if(%p,<br>%p,)\n%if(%a,<br>%a,)\n%if(%l,<br><b>%l</b>,)"

class SoundCore;
class MediaPlayer;
class QEvent;
class QMenu;

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class StatusIcon : public QObject
{
Q_OBJECT
public:
    StatusIcon(QObject *parent = nullptr);

    ~StatusIcon();

private slots:
    void showMetaData();
    void setState(Qmmp::State state);
    void trayActivated(QSystemTrayIcon::ActivationReason);

private:
    QmmpTrayIcon *m_tray;
    bool m_showMessage;
    bool m_hideToTray;
    bool m_useStandardIcons;
    bool m_showToolTip;
    QString m_toolTipTemplate;
    int m_messageDelay;
    SoundCore *m_core;
    MediaPlayer *m_player;
    QMenu *m_menu;
    MetaDataFormatter m_messageFormatter;
    MetaDataFormatter m_toolTipFormatter;
    bool m_splitFileName;
};

#endif
