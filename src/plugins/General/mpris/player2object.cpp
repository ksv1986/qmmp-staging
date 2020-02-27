/***************************************************************************
 *   Copyright (C) 2010-2020 by Ilya Kotov                                 *
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
#include <QDBusMessage>
#include <QDBusConnection>
#include <qmmp/soundcore.h>
#include <qmmp/metadatamanager.h>
#include <qmmpui/mediaplayer.h>
#include <qmmpui/playlistmanager.h>
#include <qmmpui/qmmpuisettings.h>
#include "player2object.h"

Player2Object::Player2Object(QObject *parent) : QDBusAbstractAdaptor(parent)
{
    m_prev_track = nullptr;
    m_previous_pos = 0;
    m_core = SoundCore::instance();
    m_player = MediaPlayer::instance();
    m_pl_manager =  m_player->playListManager();
    m_ui_settings = QmmpUiSettings::instance();
    connect(m_core, SIGNAL(trackInfoChanged()), SLOT(updateId()));
    connect(m_core, SIGNAL(trackInfoChanged()), SLOT(emitPropertiesChanged()));
    connect(m_core, SIGNAL(stateChanged (Qmmp::State)), SLOT(checkState(Qmmp::State)));
    connect(m_core, SIGNAL(stateChanged (Qmmp::State)), SLOT(emitPropertiesChanged()));
    connect(m_core, SIGNAL(volumeChanged(int,int)), SLOT(emitPropertiesChanged()));
    connect(m_core, SIGNAL(elapsedChanged(qint64)), SLOT(checkSeeking(qint64)));
    connect(m_ui_settings, SIGNAL(repeatableListChanged(bool)), SLOT(emitPropertiesChanged()));
    connect(m_ui_settings, SIGNAL(repeatableTrackChanged(bool)), SLOT(emitPropertiesChanged()));
    connect(m_ui_settings, SIGNAL(shuffleChanged(bool)), SLOT(emitPropertiesChanged()));
    connect(m_pl_manager, SIGNAL(currentPlayListChanged(PlayListModel*,PlayListModel*)),
            SLOT(setModel(PlayListModel*,PlayListModel*)));
    setModel(m_pl_manager->currentPlayList(), nullptr);
    updateId();
    syncProperties();
}

Player2Object::~Player2Object()
{}

bool Player2Object::canControl() const
{
    return true;
}

bool Player2Object::canGoNext() const
{
    return m_pl_manager->currentPlayList()->nextTrack() != nullptr;
}

bool Player2Object::canGoPrevious() const
{
    return m_pl_manager->currentPlayList()->currentIndex() > 0;
}

bool Player2Object::canPause() const
{
    return !m_pl_manager->currentPlayList()->isEmpty();
}
bool Player2Object::canPlay() const
{
    return !m_pl_manager->currentPlayList()->isEmpty();
}

bool Player2Object::canSeek() const
{
    return m_core->duration() > 0;
}

QString Player2Object::loopStatus() const
{
    if(m_ui_settings->isRepeatableTrack())
        return "Track";
    else if(m_ui_settings->isRepeatableList())
        return "Playlist";
    else
        return "None";
}

void Player2Object::setLoopStatus(const QString &value)
{
    if(value == "Track")
    {
        m_ui_settings->setRepeatableList(false);
        m_ui_settings->setRepeatableTrack(true);
    }
    else if(value == "Playlist")
    {
        m_ui_settings->setRepeatableList(true);
        m_ui_settings->setRepeatableTrack(false);
    }
    else
    {
        m_ui_settings->setRepeatableList(false);
        m_ui_settings->setRepeatableTrack(false);
    }
}

double Player2Object::maximumRate() const
{
    return 1.0;
}

QVariantMap Player2Object::metadata() const
{
    PlayListTrack *track = m_pl_manager->currentPlayList()->currentTrack();
    if(!track || m_core->path().isEmpty())
        return QVariantMap();
    QVariantMap map;
    TrackInfo info = m_core->trackInfo();
    map["mpris:length"] = qMax(m_core->duration() * 1000 , qint64(0));
    if(!MetaDataManager::instance()->getCoverPath(info.path()).isEmpty())
    {
        map["mpris:artUrl"] = QUrl::fromLocalFile(
                    MetaDataManager::instance()->getCoverPath(info.path())).toString();
    }
    if(!info.value(Qmmp::ALBUM).isEmpty())
        map["xesam:album"] = info.value(Qmmp::ALBUM);
    if(!info.value(Qmmp::ARTIST).isEmpty())
        map["xesam:artist"] = QStringList() << info.value(Qmmp::ARTIST);
    if(!info.value(Qmmp::ALBUMARTIST).isEmpty())
        map["xesam:albumArtist"] = QStringList() << info.value(Qmmp::ALBUMARTIST);
    if(!info.value(Qmmp::COMMENT).isEmpty())
        map["xesam:comment"] = QStringList() << info.value(Qmmp::COMMENT);
    if(!info.value(Qmmp::COMPOSER).isEmpty())
        map["xesam:composer"] = QStringList() << info.value(Qmmp::COMPOSER);
    if(!info.value(Qmmp::DISCNUMBER).isEmpty())
        map["xesam:discNumber"] = info.value(Qmmp::DISCNUMBER).toInt();
    if(!info.value(Qmmp::GENRE).isEmpty())
        map["xesam:genre"] = QStringList() << info.value(Qmmp::GENRE);
    if(!info.value(Qmmp::TITLE).isEmpty())
        map["xesam:title"] = info.value(Qmmp::TITLE);
    if(!info.value(Qmmp::TRACK).isEmpty())
        map["xesam:trackNumber"] = info.value(Qmmp::TRACK).toInt();
    if(!info.value(Qmmp::YEAR).isEmpty())
        map["xesam:contentCreated"] = info.value(Qmmp::YEAR);
    map["mpris:trackid"] = QVariant::fromValue<QDBusObjectPath>(m_trackID);
    if(info.path().startsWith("/"))
        map["xesam:url"] =  QUrl::fromLocalFile(info.path()).toString();
    else
        map["xesam:url"] = info.path();
    return map;
}

double Player2Object::minimumRate() const
{
    return 1.0;
}

QString Player2Object::playbackStatus() const
{
    if(m_core->state() == Qmmp::Playing)
        return "Playing";
    else if (m_core->state() == Qmmp::Paused)
        return "Paused";
    return "Stopped";
}

qlonglong Player2Object::position() const
{
    return qMax(m_core->elapsed() * 1000, qint64(0));
}

double Player2Object::rate() const
{
    return 1.0;
}

void Player2Object::setRate(double value)
{
    Q_UNUSED(value)
}

bool Player2Object::shuffle() const
{
    return m_ui_settings->isShuffle();
}

void Player2Object::setShuffle(bool value)
{
    m_ui_settings->setShuffle(value);
}

double Player2Object::volume() const
{
    return qMax(m_core->leftVolume(), m_core->rightVolume())/100.0;
}

void Player2Object::Player2Object::setVolume(double value)
{
    value = qBound(0.0, value ,1.0);
    int balance = (volume() > 0) ? (m_core->rightVolume() - m_core->leftVolume())/volume() : 0;
    m_core->setVolume(value*100 - qMax(balance,0)*value,
                      value*100 + qMin(balance,0)*value);
}

void Player2Object::Next()
{
    m_player->next();
}

void Player2Object::OpenUri(const QString &in0)
{
    QString path = in0;
    if(in0.startsWith("file://"))
    {
        path = QUrl(in0).toLocalFile ();
        if(!QFile::exists(path))
            return; //error
    }
    if(!m_pl_manager->currentPlayList()->isLoaderRunning())
    {
        m_pl_manager->selectPlayList(m_pl_manager->currentPlayList());
        connect(m_pl_manager->currentPlayList(), SIGNAL(trackAdded(PlayListTrack*)),
                SLOT(playTrack(PlayListTrack*)));
        connect(m_pl_manager->currentPlayList(), SIGNAL(loaderFinished()), this, SLOT(disconnectPl()));
    }
    m_pl_manager->currentPlayList()->add(path);
}

void Player2Object::Pause()
{
    if(m_core->state() == Qmmp::Playing)
        m_core->pause();
}

void Player2Object::Play()
{
    if(m_core->state() == Qmmp::Paused)
        m_core->pause();
    else if(m_core->state() != Qmmp::Playing && m_core->state() != Qmmp::Buffering)
        m_player->play();
}

void Player2Object::PlayPause()
{
    if(m_core->state() == Qmmp::Playing || m_core->state() == Qmmp::Paused)
        m_core->pause();
    else if(m_core->state() != Qmmp::Playing && m_core->state() != Qmmp::Buffering)
        m_player->play();
}

void Player2Object::Previous()
{
    m_player->previous();
}

void Player2Object::Seek(qlonglong Offset)
{
    m_core->seek(qMax(qint64(0), m_core->elapsed() +  Offset/1000));
}
void Player2Object::SetPosition(const QDBusObjectPath &TrackId, qlonglong Position)
{
    if(m_trackID == TrackId)
        m_core->seek(Position/1000);
    else
        qWarning("Player2Object: SetPosition() called with a invalid trackId");
}

void Player2Object::Stop()
{
    m_core->stop();
}

void Player2Object::emitPropertiesChanged()
{
    QList<QByteArray> changedProps;
    if(m_props["CanGoNext"] != canGoNext())
        changedProps << "CanGoNext";
    if(m_props["CanGoPrevious"] != canGoPrevious())
        changedProps << "CanGoPrevious";
    if(m_props["CanPause"] != canPause())
        changedProps << "CanPause";
    if(m_props["CanPlay"] != canPlay())
        changedProps << "CanPlay";
    if(m_props["CanSeek"] != canSeek())
        changedProps << "CanSeek";
    if(m_props["LoopStatus"] != loopStatus())
        changedProps << "LoopStatus";
    if(m_props["MaximumRate"] != maximumRate())
        changedProps << "MaximumRate";
    if(m_props["MinimumRate"] != minimumRate())
        changedProps << "MinimumRate";
    if(m_props["PlaybackStatus"] != playbackStatus())
        changedProps << "PlaybackStatus";
    if(m_props["Rate"] != rate())
        changedProps << "Rate";
    if(m_props["Shuffle"] != shuffle())
        changedProps << "Shuffle";
    if(m_props["Volume"] != volume())
        changedProps << "Volume";
    if(m_props["Metadata"] != metadata())
        changedProps << "Metadata";

    if(changedProps.isEmpty())
        return;

    syncProperties();

    QVariantMap map;
    for(const QByteArray &name : qAsConst(changedProps))
        map.insert(name, m_props.value(name));

    QDBusMessage msg = QDBusMessage::createSignal("/org/mpris/MediaPlayer2",
                                                  "org.freedesktop.DBus.Properties", "PropertiesChanged");
    msg << "org.mpris.MediaPlayer2.Player";
    msg << map;
    msg << QStringList();
    QDBusConnection::sessionBus().send(msg);
}

void Player2Object::updateId()
{
    if(m_prev_track != m_pl_manager->currentPlayList()->currentTrack())
    {
        m_trackID = QDBusObjectPath(QString("%1/Track/%2").arg("/org/qmmp/MediaPlayer2").arg(qrand()));
        m_prev_track = m_pl_manager->currentPlayList()->currentTrack();
    }
}

void Player2Object::checkState(Qmmp::State state)
{
    if(state == Qmmp::Playing)
    {
        updateId();
        m_previous_pos = 0;
    }
}

void Player2Object::checkSeeking(qint64 elapsed)
{
    if(abs(elapsed - m_previous_pos) > 2000)
    {
        emit Seeked(elapsed * 1000);
    }
    m_previous_pos = elapsed;
}

void Player2Object::playTrack(PlayListTrack *item)
{
    m_pl_manager->selectPlayList((PlayListModel*)sender());
    m_pl_manager->activatePlayList((PlayListModel*)sender());
    disconnect(sender(), SIGNAL(trackAdded(PlayListTrack*)), this, SLOT(playTrack(PlayListTrack*)));
    if(!m_pl_manager->currentPlayList()->setCurrent(item))
        return;
    m_core->stop();
    m_player->play();
}

void Player2Object::disconnectPl()
{
    disconnect(sender(), SIGNAL(trackAdded(PlayListTrack*)),
               this, SLOT(playTrack(PlayListTrack*)));
}

void Player2Object::setModel(PlayListModel *selected, PlayListModel *previous)
{
    if(previous)
        disconnect(previous, nullptr, this, nullptr); //disconnect previous model
    connect(selected, SIGNAL(listChanged(int)), SLOT(emitPropertiesChanged()));
}

void Player2Object::syncProperties()
{
    m_props["CanGoNext"] = canGoNext();
    m_props["CanGoPrevious"] = canGoPrevious();
    m_props["CanPause"] = canPause();
    m_props["CanPlay"] = canPlay();
    m_props["CanSeek"] = canSeek();
    m_props["LoopStatus"] = loopStatus();
    m_props["MaximumRate"] = maximumRate();
    m_props["MinimumRate"] = minimumRate();
    m_props["PlaybackStatus"] = playbackStatus();
    m_props["Rate"] = rate();
    m_props["Shuffle"] = shuffle();
    m_props["Volume"] = volume();
    m_props["Metadata"] = metadata();
}
