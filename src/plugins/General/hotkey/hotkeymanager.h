/***************************************************************************
 *   Copyright (C) 2009-2022 by Ilya Kotov                                 *
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
#ifndef HOTKEYMANAGER_H
#define HOTKEYMANAGER_H

#include <QObject>
#include <QString>
#include <QTableWidgetItem>
#include <QList>
#include <QAbstractNativeEventFilter>
#ifdef Q_OS_WIN
#include <windows.h>
#endif
#include <qmmpui/general.h>

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/

class Hotkey
{
public:
    enum Action
    {
        PLAY = QTableWidgetItem::UserType,
        STOP,
        PAUSE,
        PLAY_PAUSE,
        NEXT,
        PREVIOUS,
        SHOW_HIDE,
        VOLUME_UP,
        VOLUME_DOWN,
        FORWARD,
        REWIND,
        JUMP_TO_TRACK,
        VOLUME_MUTE
    };

    quint32 mod = 0;
    quint32 key = 0;
    int action = PLAY;
    int code = 0;
#ifdef Q_OS_WIN
    UINT mods = 0;
    WPARAM id = 0;
#endif
    quint32 defaultKey();
    static quint32 defaultKey(int act);
};

#ifdef QMMP_WS_X11
typedef struct _XDisplay Display;
struct xcb_screen_t;
struct xcb_connection_t;
#endif

class HotkeyManager : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    HotkeyManager(QObject *parent = nullptr);

    ~HotkeyManager();

    static const QString getKeyString(quint32 key, quint32 modifiers);
    static QList<long> ignModifiersList();
    static quint32 keycodeToKeysym(quint32 keycode);
#ifdef QMMP_WS_X11
    static Display* display();
    static bool isPlatformX11();
    static quint32 appRootWindow();
#endif

private:
#ifdef QMMP_WS_X11
    static xcb_screen_t *screenOfDisplay(xcb_connection_t *conn, int screen);
#endif
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;
    QList <Hotkey *> m_grabbedKeys;
};

#endif
