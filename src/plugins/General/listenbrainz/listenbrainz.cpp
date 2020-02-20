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

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QByteArray>
#include <QCryptographicHash>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QElapsedTimer>
#include <QTimer>
#include <QDesktopServices>
#include <QSettings>
#include <QDateTime>
#include <qmmp/soundcore.h>
#include <qmmp/qmmpsettings.h>
#include <qmmp/qmmp.h>
#include "listenbrainz.h"

#define API_ROOT "https://api.listenbrainz.org"

ListenBrainz::ListenBrainz(QObject *parent)
    : QObject(parent)
{
    m_time = new QElapsedTimer();
    m_cache = new PayloadCache(Qmmp::configDir() +"/listenbrainz.cache");
    m_ua = QString("qmmp-plugins/%1").arg(Qmmp::strVersion().toLower()).toLatin1();
    m_http = new QNetworkAccessManager(this);
    m_core = SoundCore::instance();
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_token = settings.value("ListenBrainz/user_token").toString().trimmed();

    connect(m_http, SIGNAL(finished (QNetworkReply *)), SLOT(processResponse(QNetworkReply *)));
    connect(QmmpSettings::instance(), SIGNAL(networkSettingsChanged()), SLOT(setupProxy()));
    connect(m_core, SIGNAL(trackInfoChanged()), SLOT(updateMetaData()));
    connect(m_core, SIGNAL(stateChanged (Qmmp::State)), SLOT(setState(Qmmp::State)));

    setupProxy();
    m_cachedSongs = m_cache->load();

    if(!m_token.isEmpty())
    {
        submit();
        if(m_core->state() == Qmmp::Playing)
        {
            setState(Qmmp::Playing);
            updateMetaData();
        }
    }
}

ListenBrainz::~ListenBrainz()
{
    m_cache->save(m_cachedSongs);
    delete m_time;
    delete m_cache;
}

void ListenBrainz::setState(Qmmp::State state)
{
    if(state == Qmmp::Playing && m_previousState == Qmmp::Paused)
    {
        qDebug("ListenBrainz: resuming from %d seconds played", int(m_elapsed / 1000));
        m_time->restart();
    }
    else if(state == Qmmp::Paused)
    {
        m_elapsed += m_time->elapsed();
        qDebug("ListenBrainz: pausing after %d seconds played", int(m_elapsed / 1000));
    }
    else if(state == Qmmp::Stopped && !m_song.metaData().isEmpty())
    {
        if(m_previousState == Qmmp::Playing)
            m_elapsed += m_time->elapsed();

        if((m_elapsed > 240000) || (m_elapsed > MIN_SONG_LENGTH && m_song.duration() == 0) ||
                (m_elapsed > int(m_song.duration() / 2) && m_song.duration() > MIN_SONG_LENGTH))
        {
            m_cachedSongs << m_song;
            m_cache->save(m_cachedSongs);
        }

        submit();
        m_song.clear();
        m_elapsed = 0;
    }
    m_previousState = state;
}

void ListenBrainz::updateMetaData()
{
    TrackInfo info = m_core->trackInfo();
    if(m_core->state() != Qmmp::Playing)
        return;

    if(!m_song.metaData().isEmpty() && m_song.metaData() != info.metaData())
    {
        int elapsed = (m_elapsed + m_time->elapsed());
        if((elapsed > 240000) || (elapsed > MIN_SONG_LENGTH && m_song.duration() == 0) ||
                (elapsed > int(m_song.duration() / 2) && m_song.duration() > MIN_SONG_LENGTH))
        {
            m_cachedSongs << m_song;
            m_cache->save(m_cachedSongs);
        }

        submit();
        m_song.clear();
    }

    if(!info.value(Qmmp::TITLE).isEmpty() && !info.value(Qmmp::ARTIST).isEmpty())
    {
        m_song = TrackMetaData(info);
        m_song.setTimeStamp(QDateTime::currentDateTime().toTime_t());
        sendNotification(m_song);
    }
    m_time->restart();
    m_elapsed = 0;
}

void ListenBrainz::processResponse(QNetworkReply *reply)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    if (reply->networkError() != QNetworkReply::NoError)
#else
    if (reply->error() != QNetworkReply::NoError)
#endif
    {
        qWarning("ListenBrainz: http error: %s", qPrintable(reply->errorString()));
    }

    QByteArray data = reply->readAll();
    QJsonDocument document = QJsonDocument::fromJson(data);
    QString status = document.object().value("status").toString();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    if(status != "ok" || reply->networkError() != QNetworkReply::NoError)
#else
    if(status != "ok" || reply->error() != QNetworkReply::NoError)
#endif
    {
        status.clear();
        qWarning("ListenBrainz: server reply: %s", data.constData());
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
        if(reply->networkError() == QNetworkReply::AuthenticationRequiredError)
#else
        if(reply->error() == QNetworkReply::AuthenticationRequiredError)
#endif
        {
            m_token.clear();
            qWarning("ListenBrainz: invalid user token, submitting has been disabled");
        }
    }

    if(reply == m_submitReply)
    {
        m_submitReply = nullptr;
        if(status == "ok")
        {
            qDebug("ListenBrainz: submited %d song(s)", m_submitedSongs);
            while (m_submitedSongs)
            {
                m_submitedSongs--;
                m_cachedSongs.removeFirst ();
            }
            if (!m_cachedSongs.isEmpty()) //submit remaining songs
            {
                submit();
            }
            else
            {
                m_cache->save(m_cachedSongs); // update the cache file to reflect the empty cache
                updateMetaData();
            }
        }
        else
        {
            QTimer::singleShot(120000, this, SLOT(submit()));
        }
    }
    else if(reply == m_notificationReply)
    {
        m_notificationReply = nullptr;
        if(status == "ok")
            qDebug("ListenBrainz: Now-Playing notification done");
    }
    reply->deleteLater();
}

void ListenBrainz::setupProxy()
{
    QmmpSettings *gs = QmmpSettings::instance();
    if (gs->isProxyEnabled())
    {
        QNetworkProxy proxy(QNetworkProxy::HttpProxy, gs->proxy().host(),  gs->proxy().port());
        if(gs->proxyType() == QmmpSettings::SOCKS5_PROXY)
            proxy.setType(QNetworkProxy::Socks5Proxy);
        if(gs->useProxyAuth())
        {
            proxy.setUser(gs->proxy().userName());
            proxy.setPassword(gs->proxy().password());
        }
        m_http->setProxy(proxy);
    }
    else
        m_http->setProxy(QNetworkProxy::NoProxy);
}

void ListenBrainz::submit()
{
    if (m_cachedSongs.isEmpty() || m_token.isEmpty() || m_submitReply)
        return;

    qDebug("ListenBrainz: submit request");
    m_submitedSongs = qMin(m_cachedSongs.size(), 20);

    QJsonArray payload;
    for (int i = 0; i < m_submitedSongs; ++i)
    {
        TrackMetaData metaData = m_cachedSongs[i];

        QJsonObject track_metadata
        {
            { "artist_name", metaData.value(Qmmp::ARTIST) },
            { "track_name", metaData.value(Qmmp::TITLE) }
        };

        if(metaData.value(Qmmp::TRACK).toInt() > 0)
        {
            QJsonObject additional_info
            {
                { "tracknumber", metaData.value(Qmmp::TRACK).toInt() }
            };

            track_metadata["additional_info"] = additional_info;
        };

        QJsonObject payloadItem
        {
            { "listened_at", qint64(metaData.timeStamp()) },
            { "track_metadata", track_metadata }
        };

        payload.append(payloadItem);
    }

    QJsonObject json { { "listen_type", "import" }, { "payload", payload } };

    QJsonDocument document(json);
    QByteArray body = document.toJson(QJsonDocument::Compact);

    QUrl url(QString("%1/1/submit-listens").arg(API_ROOT));
    url.setPort(443);

    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", m_ua);
    request.setRawHeader("Host", url.host().toLatin1());
    request.setRawHeader("Accept", "*/*");
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Authorization", QString("Token %1").arg(m_token).toLatin1());
    request.setHeader(QNetworkRequest::ContentLengthHeader,  body.size());
    m_submitReply = m_http->post(request, body);
}

void ListenBrainz::sendNotification(const TrackMetaData &metaData)
{
    if(m_token.isEmpty() || m_notificationReply)
        return;

    qDebug("ListenBrainz: sending notification...");

    QJsonObject track_metadata
    {
        { "artist_name", metaData.value(Qmmp::ARTIST) },
        { "track_name", metaData.value(Qmmp::TITLE) }
    };

    if(metaData.value(Qmmp::TRACK).toInt() > 0)
    {
        QJsonObject additional_info
        {
            { "tracknumber", metaData.value(Qmmp::TRACK).toInt() }
        };

        track_metadata["additional_info"] = additional_info;
    };

    QJsonObject payloadItem { { "track_metadata", track_metadata } };

    QJsonArray payload = { payloadItem };
    QJsonObject json { { "listen_type", "playing_now" }, { "payload", payload } };

    QJsonDocument document(json);
    QByteArray body = document.toJson(QJsonDocument::Compact);

    QUrl url(QString("%1/1/submit-listens").arg(API_ROOT));
    url.setPort(443);

    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", m_ua);
    request.setRawHeader("Host", url.host().toLatin1());
    request.setRawHeader("Accept", "*/*");
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Authorization", QString("Token %1").arg(m_token).toLatin1());
    request.setHeader(QNetworkRequest::ContentLengthHeader,  body.size());
    m_notificationReply = m_http->post(request, body);
}
