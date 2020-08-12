/***************************************************************************
 *   Copyright (C) 2013-2020 by Ilya Kotov                                 *
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

#include <QtGlobal>
#ifdef Q_OS_WIN
#include <QSettings>
#include <QApplication>
#include <windows.h>
#include <winuser.h>
#include <commctrl.h>
#include <qmmp/qmmp.h>
#include <qmmp/soundcore.h>
#include <qmmpui/mediaplayer.h>
#include <qmmpui/uihelper.h>
#include "hotkeymanager.h"

static struct
{
    unsigned int key; //virtual key
    unsigned long code; //scan code
    QString name;
} keyMap [] = {
    { VK_LEFT,    0x14b, ""},
    { VK_UP,      0x148, "" },
    { VK_RIGHT,   0x14d, "" },
    { VK_DOWN,    0x150, "" },
    { VK_PRIOR,   0x149, "" },
    { VK_NEXT,    0x151, "" },
    { VK_END,     0x14F, "" },
    { VK_HOME,    0x147, "" },
    { VK_INSERT,  0x152, "" },
    { VK_DELETE,  0x153, "" },
    { VK_DIVIDE,  0x135, "" },
    { VK_NUMLOCK, 0x145, "" },
    { VK_NUMPAD0, 0x52,  "" },
    { VK_NUMPAD1, 0x4F,  "" },
    { VK_NUMPAD2, 0x50,  "" },
    { VK_NUMPAD3, 0x51,  "" },
    { VK_NUMPAD4, 0x4b,  "" },
    { VK_NUMPAD6, 0x4d,  "" },
    { VK_NUMPAD7, 0x47,  "" },
    { VK_NUMPAD8, 0x48,  "" },
    { VK_NUMPAD9, 0x49,  "" },
    { VK_VOLUME_MUTE, 0x120,  "Volume Mute"},
    { VK_VOLUME_DOWN, 0x12e,  "Volume Down"},
    { VK_VOLUME_UP, 0x130,  "Volume Up"},
    { VK_MEDIA_NEXT_TRACK, 0x119,  "Media Next Track"},
    { VK_MEDIA_PREV_TRACK, 0x110,  "Media Previous Track"},
    { VK_MEDIA_STOP, 0x124,  "Media Stop"},
    { VK_MEDIA_PLAY_PAUSE, 0x122,  "Media Play/Pause"},
{ 0, 0, nullptr }
};

quint32 Hotkey::defaultKey()
{
    return defaultKey(action);
}

quint32 Hotkey::defaultKey(int act)
{
    //default key bindings
    static const QMap<int, quint32> defaultKeys = {
        { PLAY, 0 },
        { STOP, VK_MEDIA_STOP },
        { PAUSE, 0 },
        { PLAY_PAUSE, VK_MEDIA_PLAY_PAUSE },
        { NEXT, VK_MEDIA_NEXT_TRACK },
        { PREVIOUS, VK_MEDIA_PREV_TRACK },
        { SHOW_HIDE, 0 },
        { VOLUME_UP, VK_VOLUME_UP },
        { VOLUME_DOWN, VK_VOLUME_DOWN },
        { FORWARD, 0 },
        { REWIND, 0 },
        { JUMP_TO_TRACK, 0 },
        { VOLUME_MUTE, VK_VOLUME_MUTE },
    };
    return defaultKeys[act];
}

HotkeyManager::HotkeyManager(QObject *parent) : QObject(parent)
{
    qApp->installNativeEventFilter(this);
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat); //load settings
    settings.beginGroup("Hotkey");
    for (int i = Hotkey::PLAY; i <= Hotkey::JUMP_TO_TRACK; ++i)
    {
        quint32 key = settings.value(QString("key_%1").arg(i), Hotkey::defaultKey(i)).toUInt();
        quint32 mod = settings.value(QString("modifiers_%1").arg(i), 0).toUInt();

        if (key)
        {
            Hotkey *hotkey = new Hotkey;
            hotkey->action = i;
            hotkey->key = key;
            hotkey->code = MapVirtualKey(key, 0);
            hotkey->mod = mod;

            if(hotkey->mod & HOTKEYF_CONTROL)
                hotkey->mods |= MOD_CONTROL;
            if(hotkey->mod & HOTKEYF_SHIFT)
                hotkey->mods |= MOD_SHIFT;
            if(hotkey->mod & HOTKEYF_ALT)
                hotkey->mods |= MOD_ALT;
            if(hotkey->mod & HOTKEYF_EXT)
                hotkey->mods |= MOD_WIN;

            hotkey->id = hotkey->mods^hotkey->key;

            if(RegisterHotKey(NULL, hotkey->id,  hotkey->mods, hotkey->key))
            {
                qDebug("HotkeyManager: registered key=0x%x, mod=0x%x", hotkey->key, hotkey->mods);
            }
            else
            {
                hotkey->id = 0;
                qWarning("HotkeyManager: unable to register key=0x%x, mod=0x%x", hotkey->key, hotkey->mods);
            }

            m_grabbedKeys << hotkey;
        }
    }
    settings.endGroup();
}

HotkeyManager::~HotkeyManager()
{
    qApp->removeNativeEventFilter(this);
    while(!m_grabbedKeys.isEmpty())
    {
        Hotkey *key = m_grabbedKeys.takeFirst ();
        if(key->id)
             UnregisterHotKey(NULL, key->id);
        delete key;
    }
}

const QString HotkeyManager::getKeyString(quint32 key, quint32 modifiers)
{
    QString strModList[] = { "Ctrl", "Shift", "Alt", "Win"};
    quint32 modList[] = { HOTKEYF_CONTROL, HOTKEYF_SHIFT, HOTKEYF_ALT, HOTKEYF_EXT};

    QString keyStr;
    for (int j = 0; j < 4; j++)
    {
        if (modifiers & modList[j])
            keyStr.append(strModList[j] + "+");
    }

    if(key == VK_SHIFT || key == VK_CONTROL || key == VK_LWIN || key == VK_RWIN || key == VK_MENU)
        return keyStr;

    LONG lScan = MapVirtualKey(key, 0) << 16;

    for(unsigned int i = 0; i < sizeof(keyMap)/sizeof(keyMap[0]); ++i)
    {
        if(keyMap[i].key == key)
        {
            if(!keyMap[i].name.isEmpty())
            {
                keyStr += keyMap[i].name;
                return keyStr;
            }
            else if(keyMap[i].code & 0x100)
            {
                lScan |= 0x1000000; // set extended bit
                break;
            }
        }
    }

    int nBufferLen = 64;
    std::wstring str;
    int nLen;
    do
    {
        nBufferLen *= 2;
        str.resize(nBufferLen);
        nLen = ::GetKeyNameTextW(lScan, &str[0], nBufferLen);
    }
    while (nLen == nBufferLen);

    keyStr += QString::fromWCharArray(str.c_str());

    return keyStr;
}

QList<long> HotkeyManager::ignModifiersList()
{
    return QList<long>();
}

quint32 HotkeyManager::keycodeToKeysym(quint32 keycode)
{
    //MapVirtualKey does not work with some scan codes
    //using hardcoded key map instead
    for(unsigned int i = 0; i < sizeof(keyMap)/sizeof(keyMap[0]); ++i)
    {
        if(keyMap[i].code == keycode)
            return keyMap[i].key;
    }
    return MapVirtualKey(keycode, 1);
}

bool HotkeyManager::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(eventType);
    Q_UNUSED(result);
    MSG *m = static_cast<MSG*>(message);
    if (m->message == WM_HOTKEY)
    {
        SoundCore *core = SoundCore::instance();
        MediaPlayer *player = MediaPlayer::instance();
        for(const Hotkey *hotkey : qAsConst(m_grabbedKeys))
        {
            if(hotkey->id != m->wParam)
                continue;

            qDebug("KeyFilterWidget: [%s] pressed",
                   qPrintable(HotkeyManager::getKeyString(hotkey->key, hotkey->mod)));

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

#include "moc_hotkeymanager.cpp"

#endif

