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
#ifndef QMMPTRAYICON_H
#define QMMPTRAYICON_H

#include <QSystemTrayIcon>
#include <QPointer>

class QEvent;
class QWheelEvent;
#ifdef QMMP_WS_X11
class StatusIconPopupWidget;
#endif

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class QmmpTrayIcon : public QSystemTrayIcon
{
Q_OBJECT
public:
    QmmpTrayIcon(QObject *parent = nullptr);
    ~QmmpTrayIcon();

     void setToolTip(const QString &tip);

#ifdef QMMP_WS_X11
private:
    bool event(QEvent *e) override;
    void wheelEvent(QWheelEvent *e);
    void showToolTip();
    
    bool hasToolTipEvent();
    QPointer<StatusIconPopupWidget> m_popupWidget;
    QString m_message;
#endif
};

#endif
