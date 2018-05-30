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

#include <QFile>
#include <QDBusMetaType>
#include <QDBusArgument>
#include <qmmp/soundcore.h>
#include <qmmp/metadatamanager.h>
#include <qmmpui/mediaplayer.h>
#include <qmmpui/playlistmanager.h>
#include <qmmpui/qmmpuisettings.h>
#include "playerobject.h"


//register << operator
QDBusArgument &operator << (QDBusArgument &arg, const PlayerStatus &status)
{
    arg.beginStructure();
    arg << status.state;
    arg << status.random;
    arg << status.repeat;
    arg << status.repeatPlayList;
    arg.endStructure();
    return arg;
}

//register >> operator
const QDBusArgument &operator >> (const QDBusArgument &arg, PlayerStatus &status)
{
    arg.beginStructure();
    arg >> status.state;
    arg >> status.random;
    arg >> status.repeat;
    arg >> status.repeatPlayList;
    arg.endStructure();
    return arg;
}

PlayerObject::PlayerObject(QObject *parent) : QObject(parent)
{
    qDBusRegisterMetaType<PlayerStatus>();
    m_core = SoundCore::instance();
    m_player = MediaPlayer::instance();
    m_pl_manager =  m_player->playListManager();
    m_ui_settings = QmmpUiSettings::instance();
    connect(m_core, SIGNAL(stateChanged (Qmmp::State)), SLOT(updateCaps()));
    connect(m_core, SIGNAL(trackInfoChanged()), SLOT(updateTrack()));
    connect(m_core, SIGNAL(stateChanged (Qmmp::State)), SLOT(updateStatus()));
    connect(m_ui_settings, SIGNAL(repeatableListChanged(bool)), SLOT(updateStatus()));
    connect(m_ui_settings, SIGNAL(shuffleChanged(bool)), SLOT(updateStatus()));
    connect(m_ui_settings, SIGNAL(repeatableTrackChanged(bool)), SLOT(updateStatus()));
}

PlayerObject::~PlayerObject()
{}

void PlayerObject::Next()
{
    m_player->next();
}

void PlayerObject::Prev()
{
    m_player->previous();
}

void PlayerObject::Pause()
{
    m_core->pause();
}

void PlayerObject::Stop()
{
    m_player->stop();
}

void PlayerObject::Play()
{
    m_player->play();
}

void PlayerObject::Repeat(bool in0)
{
    m_ui_settings->setRepeatableTrack(in0);
}

PlayerStatus PlayerObject::GetStatus()
{
    PlayerStatus st;
    switch (m_core->state())
    {
    case Qmmp::Stopped:
    case Qmmp::NormalError:
    case Qmmp::FatalError:
        st.state = 2;
        break;
    case Qmmp::Playing:
    case Qmmp::Buffering:
        st.state = 0;
        break;
    case Qmmp::Paused:
        st.state = 1;
    };
    st.random = int(m_ui_settings->isShuffle());
    st.repeat = int(m_ui_settings->isRepeatableTrack());
    st.repeatPlayList = int(m_ui_settings->isRepeatableList());
    return st;
}

QVariantMap PlayerObject::GetMetadata()
{
    TrackInfo info = m_core->trackInfo();
    QVariantMap map;

    if (info.path().contains("://"))
        map.insert("location", info.path());
    else
        map.insert("location", "file://" + info.path());
    map.insert("arturl", MetaDataManager::instance()->getCoverPath(info.path()));
    map.insert("title", info.value(Qmmp::TITLE));
    map.insert("artist", info.value(Qmmp::ARTIST));
    map.insert("albumartist", info.value(Qmmp::ALBUMARTIST));
    map.insert("album", info.value(Qmmp::ALBUM));
    map.insert("tracknumber", info.value(Qmmp::TRACK));
    map.insert("time", (quint32)info.duration()/1000);
    map.insert("mtime", (quint32)info.duration());
    map.insert("genre", info.value(Qmmp::GENRE));
    map.insert("comment", info.value(Qmmp::COMMENT));
    map.insert("audio-bitrate", (quint32)info.value(Qmmp::BITRATE).toInt());
    map.insert("audio-samplerate", (quint32)info.value(Qmmp::SAMPLERATE).toInt());
    map.insert("year", info.value(Qmmp::YEAR).toUInt());
    return map;
}

int PlayerObject::GetCaps()
{
    int caps = NONE;
    if (GetStatus().state == 0)
        caps |= CAN_PAUSE;
    else
        caps |= CAN_PLAY;
    if ((GetStatus().state < 2) && (m_core->duration() > 0))
        caps |= CAN_SEEK;
    caps |= CAN_GO_NEXT;
    caps |= CAN_GO_PREV;
    caps |= CAN_PROVIDE_METADATA;
    return caps;
}

void PlayerObject::VolumeSet(int volume)
{
    int balance = (VolumeGet() > 0) ? (m_core->rightVolume() - m_core->leftVolume()) * 100/VolumeGet() : 0;
    m_core->setVolume(volume - qMax(balance,0)*volume/100,
                      volume + qMin(balance,0)*volume/100);
}

int PlayerObject::VolumeGet()
{
    return qMax(m_core->leftVolume(), m_core->rightVolume());
}

void PlayerObject::PositionSet(int pos)
{
    m_core->seek(pos);
}

int PlayerObject::PositionGet()
{
    return m_core->elapsed();
}

void PlayerObject::updateCaps()
{
    emit CapsChange(GetCaps());
}

void PlayerObject::updateTrack()
{
    emit TrackChange(GetMetadata());
}

void PlayerObject::updateStatus()
{
    emit StatusChange(GetStatus());
}
