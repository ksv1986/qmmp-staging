/***************************************************************************
 *   Copyright (C) 2019 by Ilya Kotov                                      *
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
#ifndef LISTENBRAINZ_H
#define LISTENBRAINZ_H

#include <QMap>
#include <QObject>
#include <qmmp/qmmp.h>
#include "payloadcache.h"

class QNetworkAccessManager;
class QNetworkReply;
class QIODevice;
class QElapsedTimer;
class SoundCore;

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class ListenBrainz : public QObject
{
    Q_OBJECT
public:
    ListenBrainz(QObject *parent = nullptr);
    ~ListenBrainz();

private slots:
    void setState(Qmmp::State state);
    void updateMetaData();
    void processResponse(QNetworkReply *reply);
    void setupProxy();
    void submit();

private:
    enum { MIN_SONG_LENGTH = 30000 };

    void sendNotification(const TrackMetaData &metaData);
    TrackMetaData m_song;
    QList<TrackMetaData> m_cachedSongs;
    QByteArray m_ua;
    int m_submitedSongs = 0;
    QString m_token;
    QNetworkAccessManager *m_http;
    SoundCore *m_core;
    QNetworkReply *m_submitReply = nullptr, *m_notificationReply = nullptr;
    QElapsedTimer *m_time;
    PayloadCache *m_cache;
    Qmmp::State m_previousState = Qmmp::Stopped;
    qint64 m_elapsed = 0;
};

#endif //LISTENBRAINZ_H
