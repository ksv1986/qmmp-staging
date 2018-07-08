/***************************************************************************
 *   Copyright (C) 2009-2018 by Ilya Kotov                                 *
 *   forkotov02@ya.ru                                                      *
 *                                                                         *
 *   Copyright (C) 2003-2007 by Justin Karneges and Michail Pishchagin     *
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

#include <QtGlobal>
#ifdef QMMP_WS_X11
#include <QSettings>
#include <QX11Info>
#include <QtDebug>
#include <QEvent>
#include <QKeyEvent>
#include <QCoreApplication>
#include <QApplication>
#include <QAbstractEventDispatcher>
#define Visual XVisual
extern "C" {
#include <X11/X.h>
#include <X11/keysym.h>
#include <X11/XF86keysym.h>
#include <X11/XKBlib.h>
#include <xcb/xcb.h>
}
#undef CursorShape
#undef Status
#undef Bool
#undef None
#undef KeyPress
#undef Visual

#include <qmmp/qmmp.h>
#include <qmmp/soundcore.h>
#include <qmmpui/mediaplayer.h>
#include <qmmpui/uihelper.h>
#include "hotkeymanager.h"

quint32 Hotkey::defaultKey()
{
    return defaultKey(action);
}

quint32 Hotkey::defaultKey(int act)
{
    //default key bindings
    QMap<int, quint32> keyMap;
    keyMap[PLAY] = 0;
    keyMap[STOP] = XF86XK_AudioStop;
    keyMap[PAUSE] = XF86XK_AudioPause;
    keyMap[PLAY_PAUSE] = XF86XK_AudioPlay;
    keyMap[NEXT] = XF86XK_AudioNext;
    keyMap[PREVIOUS] = XF86XK_AudioPrev;
    keyMap[SHOW_HIDE] = 0;
    keyMap[VOLUME_UP] = XF86XK_AudioRaiseVolume;
    keyMap[VOLUME_DOWN] = XF86XK_AudioLowerVolume;
    keyMap[FORWARD] = 0;
    keyMap[REWIND] = 0;
    keyMap[JUMP_TO_TRACK] = 0;
    keyMap[VOLUME_MUTE] = XF86XK_AudioMute;
    return keyMap[act];
}

HotkeyManager::HotkeyManager(QObject *parent) : QObject(parent)
{
    QCoreApplication::instance()->installEventFilter(this);
    WId rootWindow = DefaultRootWindow(QX11Info::display());
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat); //load settings
    settings.beginGroup("Hotkey");
    for (int i = Hotkey::PLAY, j = 0; i <= Hotkey::VOLUME_MUTE; ++i, ++j)
    {
        quint32 key = settings.value(QString("key_%1").arg(i), Hotkey::defaultKey(i)).toUInt();
        quint32 mod = settings.value(QString("modifiers_%1").arg(i), 0).toUInt();

        if (key)
        {
            foreach(long mask_mod, ignModifiersList())
            {
                Hotkey *hotkey = new Hotkey;
                hotkey->action = i;
                hotkey->key = key;
                hotkey->code = XKeysymToKeycode(QX11Info::display(), hotkey->key);
                if(!hotkey->code)
                    continue;

                XGrabKey(QX11Info::display(),  hotkey->code, mod | mask_mod, rootWindow, True,
                         GrabModeAsync, GrabModeAsync);
                hotkey->mod = mod | mask_mod;
                m_grabbedKeys << hotkey;
            }
        }
    }
    settings.endGroup();
    XSync(QX11Info::display(), False);
    qApp->installNativeEventFilter(this);
}

HotkeyManager::~HotkeyManager()
{
    qApp->removeNativeEventFilter(this);
    while(!m_grabbedKeys.isEmpty())
    {
        Hotkey *key = m_grabbedKeys.takeFirst ();
        if(key->code)
            XUngrabKey(QX11Info::display(), key->code, key->mod, QX11Info::appRootWindow());
        delete key;
    }
}

const QString HotkeyManager::getKeyString(quint32 key, quint32 modifiers)
{
    QString strModList[] = { "Control", "Shift", "Alt", "Mod2", "Mod3", "Super", "Mod5" };
    quint32 modList[] = { ControlMask, ShiftMask, Mod1Mask, Mod2Mask, Mod3Mask, Mod4Mask, Mod5Mask };
    QString keyStr;
    for (int j = 0; j < 7; j++)
    {
        if (modifiers & modList[j])
            keyStr.append(strModList[j] + "+");
    }
    keyStr.append(XKeysymToString(key));
    return keyStr;
}

bool HotkeyManager::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(eventType);
    Q_UNUSED(result);
    xcb_generic_event_t *e = static_cast<xcb_generic_event_t*>(message);

    if(e->response_type == XCB_KEY_PRESS)
    {
        xcb_key_press_event_t *ke = (xcb_key_press_event_t*)e;
        quint32 key = keycodeToKeysym(ke->detail);
        quint32 mod = ke->state;
        SoundCore *core = SoundCore::instance();
        MediaPlayer *player = MediaPlayer::instance();
        foreach(Hotkey *hotkey, m_grabbedKeys)
        {
            if (hotkey->key != key || hotkey->mod != mod)
                continue;
            qDebug("HotkeyManager: [%s] pressed", qPrintable(getKeyString(key, mod)));

            switch (hotkey->action)
            {
            case Hotkey::PLAY:
                player->play();
                break;
            case Hotkey::STOP:
                player->stop();
                break;
            case Hotkey::PAUSE:
                core->pause();
                break;
            case Hotkey::PLAY_PAUSE:
                if (core->state() == Qmmp::Stopped)
                    player->play();
                else if (core->state() != Qmmp::FatalError)
                    core->pause();
                break;
            case Hotkey::NEXT:
                player->next();
                break;
            case Hotkey::PREVIOUS:
                player->previous();
                break;
            case Hotkey::SHOW_HIDE:
                UiHelper::instance()->toggleVisibility();
                break;
            case Hotkey::VOLUME_UP:
                core->volumeUp();
                break;
            case Hotkey::VOLUME_DOWN:
                core->volumeDown();
                break;
            case Hotkey::FORWARD:
                core->seek(core->elapsed() + 5000);
                break;
            case Hotkey::REWIND:
                core->seek(qMax(qint64(0), core->elapsed() - 5000));
                break;
            case Hotkey::JUMP_TO_TRACK:
                UiHelper::instance()->jumpToTrack();
                break;
            case Hotkey::VOLUME_MUTE:
                SoundCore::instance()->setMuted(!SoundCore::instance()->isMuted());
                break;
            }
        }
    }
    return false;
}

QList<long> HotkeyManager::ignModifiersList()
{
    QList<long> ret = { 0, Mod2Mask, LockMask, (Mod2Mask | LockMask) };
    return ret;
}

quint32 HotkeyManager::keycodeToKeysym(quint32 keycode)
{
    return XkbKeycodeToKeysym(QX11Info::display(), keycode, 0, 0);
}

#include "moc_hotkeymanager.cpp"

#endif
