/***************************************************************************
 *   Copyright (C) 2010-2022 by Ilya Kotov                                 *
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
#include <QXmlStreamReader>
#include <QUrl>
#include <QUrlQuery>
#include <QElapsedTimer>
#include <QTimer>
#include <QDesktopServices>
#include <QSettings>
#include <QDateTime>
#include <qmmp/soundcore.h>
#include <qmmp/qmmpsettings.h>
#include <qmmp/qmmp.h>
#include "scrobbler.h"

#define API_KEY "d71c6f01b2ea562d7042bd5f5970041f"
#define SECRET "32d47bc0010473d40e1d38bdcff20968"

void ScrobblerResponse::parse(QIODevice *device)
{
    QXmlStreamReader reader(device);
    QStringList tags;
    while(!reader.atEnd())
    {
        reader.readNext();
        if(reader.isStartElement())
        {
            tags << reader.name().toString();
            if(tags.last() == "lfm")
                status = reader.attributes().value("status").toString();
            else if(tags.last() == "error")
                code = reader.attributes().value("code").toString();
        }
        else if(reader.isCharacters() && !reader.isWhitespace())
        {
            if(tags.last() == "token")
                token = reader.text().toString();
            else if(tags.last() == "error")
                error = reader.text().toString();
            if(tags.count() >= 2 && tags.at(tags.count() - 2) == "session")
            {
                if(tags.last() == "key")
                    key = reader.text().toString();
                else if(tags.last() == "name")
                    name = reader.text().toString();
                else if(tags.last() == "subscriber")
                    subscriber = reader.text().toString();
            }
        }
        else if(reader.isEndElement())
        {
            tags.takeLast();
        }
    }
}

Scrobbler::Scrobbler(const QString &scrobblerUrl, const QString &name, QObject *parent)
    : QObject(parent),
      m_ua(QString("qmmp-plugins/%1").arg(Qmmp::strVersion().toLower()).toLatin1()),
      m_http(new QNetworkAccessManager(this)),
      m_core(SoundCore::instance()),
      m_time(new QElapsedTimer()),
      m_cache(new ListenCache(Qmmp::cacheDir() + "/scrobbler_" + name + ".cache")),
      m_scrobblerUrl(scrobblerUrl),
      m_name(name)
{

    QSettings settings;
    m_session = settings.value("Scrobbler/"+name+"_session").toString();

    connect(m_http, SIGNAL(finished (QNetworkReply *)), SLOT(processResponse(QNetworkReply *)));
    connect(QmmpSettings::instance(), SIGNAL(networkSettingsChanged()), SLOT(setupProxy()));
    connect(m_core, SIGNAL(trackInfoChanged()), SLOT(updateMetaData()));
    connect(m_core, SIGNAL(stateChanged (Qmmp::State)), SLOT(setState(Qmmp::State)));

    setupProxy();
    m_cachedSongs = m_cache->load();

    if(!m_session.isEmpty())
    {
        submit();
        if(m_core->state() == Qmmp::Playing)
        {
            setState(Qmmp::Playing);
            updateMetaData();
        }
    }
}

Scrobbler::~Scrobbler()
{
    m_cache->save(m_cachedSongs);
    delete m_time;
    delete m_cache;

}

void Scrobbler::setState(Qmmp::State state)
{
    if(state == Qmmp::Playing && m_previousState == Qmmp::Paused)
    {
        qDebug("Scrobbler[%s]: resuming from %d seconds played", qPrintable(m_name), int(m_elapsed / 1000));
        m_time->restart();
    }
    else if(state == Qmmp::Paused)
    {
        m_elapsed += m_time->elapsed();
        qDebug("Scrobbler[%s]: pausing after %d seconds played", qPrintable(m_name), int(m_elapsed / 1000));
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

void Scrobbler::updateMetaData()
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
        m_song = SongInfo(info);
        m_song.setTimeStamp(QDateTime::currentDateTime().toSecsSinceEpoch());
        sendNotification(m_song);
    }
    m_time->restart();
    m_elapsed = 0;
}

void Scrobbler::processResponse(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning("Scrobbler[%s]: http error: %s", qPrintable(m_name), qPrintable(reply->errorString()));
    }

    ScrobblerResponse response;
    response.parse(reply);

    QString error_code;
    if(response.status != "ok" && !response.status.isEmpty())
    {
        if(!response.error.isEmpty())
        {
            qWarning("Scrobbler[%s]: status=%s, %s-%s", qPrintable(m_name), qPrintable(response.status),
                     qPrintable(response.code), qPrintable(response.error));
            error_code = response.code;
        }
        else
            qWarning("Scrobbler[%s]: invalid content", qPrintable(m_name));
    }

    if (reply == m_submitReply)
    {
        m_submitReply = nullptr;
        if (response.status == "ok")
        {
            qDebug("Scrobbler[%s]: submited %d song(s)", qPrintable(m_name), m_submitedSongs);
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
        else if(error_code == "9") //invalid session key
        {
            m_session.clear();
            qWarning("Scrobbler[%s]: invalid session key, scrobbling disabled", qPrintable(m_name));
        }
        else if(error_code == "11" || error_code == "16" || error_code.isEmpty()) //unavailable
        {
            QTimer::singleShot(120000, this, SLOT(submit()));
        }
        else
        {
            m_session.clear();
            qWarning("Scrobbler[%s]: service returned unrecoverable error, scrobbling disabled",
                     qPrintable(m_name));
        }
    }
    else if (reply == m_notificationReply)
    {
        m_notificationReply = nullptr;
        if(response.status == "ok")
        {
            qDebug("Scrobbler[%s]: Now-Playing notification done", qPrintable(m_name));
        }
        else if(error_code == "9") //invalid session key
        {
            m_session.clear();
            qWarning("Scrobbler[%s]: invalid session key, scrobbling has been disabled", qPrintable(m_name));
        }
    }
    reply->deleteLater();
}

void Scrobbler::setupProxy()
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

void Scrobbler::submit()
{
    if (m_cachedSongs.isEmpty() || m_session.isEmpty() || m_submitReply)
        return;

    qDebug("Scrobbler[%s]: submit request", qPrintable(m_name));
    m_submitedSongs = qMin(m_cachedSongs.size(),25);

    QMap <QString, QString> params;
    for (int i = 0; i < m_submitedSongs; ++i)
    {
        SongInfo info = m_cachedSongs[i];
        params.insert(QString("track[%1]").arg(i),info.value(Qmmp::TITLE));
        params.insert(QString("timestamp[%1]").arg(i),QString("%1").arg(info.timeStamp()));
        params.insert(QString("artist[%1]").arg(i),info.value(Qmmp::ARTIST));
        params.insert(QString("album[%1]").arg(i),info.value(Qmmp::ALBUM));
        params.insert(QString("trackNumber[%1]").arg(i),info.value(Qmmp::TRACK));
        if(info.duration() > 0)
            params.insert(QString("duration[%1]").arg(i),QString("%1").arg(info.duration() / 1000));
    }
    params.insert("api_key", API_KEY);
    params.insert("method", "track.scrobble");
    params.insert("sk", m_session);

    const QStringList keys = params.keys();
    for(const QString &key : qAsConst(keys)) //removes empty keys
    {
        if(params.value(key).isEmpty() || params.value(key) == "0")
            params.remove(key);
    }

    QUrl url(m_scrobblerUrl);
    url.setPort(m_scrobblerUrl.startsWith("https") ? 443 : 80);

    QUrlQuery body("");
    QByteArray data;
    for(const QString &key : params.keys())
    {
        body.addQueryItem(key, params.value(key));
        data.append(key.toUtf8() + params.value(key).toUtf8());
    }
    data.append(SECRET);
    body.addQueryItem("api_sig", QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex());
    QByteArray bodyData = body.query(QUrl::FullyEncoded).toUtf8();
    bodyData.replace("+", QUrl::toPercentEncoding("+"));

    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", m_ua);
    request.setRawHeader("Host", url.host().toLatin1());
    request.setRawHeader("Accept", "*/*");
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    request.setHeader(QNetworkRequest::ContentLengthHeader, bodyData.size());
    m_submitReply = m_http->post(request, bodyData);
}

void Scrobbler::sendNotification(const SongInfo &info)
{
    if(m_session.isEmpty() || m_notificationReply)
        return;
    qDebug("Scrobbler[%s]: sending notification", qPrintable(m_name));

    QMap <QString, QString> params;
    params.insert("track", info.value(Qmmp::TITLE));
    params.insert("artist", info.value(Qmmp::ARTIST));
    if(!info.value(Qmmp::ALBUM).isEmpty())
        params.insert("album", info.value(Qmmp::ALBUM));
    if(!info.value(Qmmp::TRACK).isEmpty())
        params.insert("trackNumber", info.value(Qmmp::TRACK));
    if(info.duration() > 0)
        params.insert("duration", QString("%1").arg(info.duration() / 1000));
    params.insert("api_key", API_KEY);
    params.insert("method", "track.updateNowPlaying");
    params.insert("sk", m_session);

    const QStringList keys = params.keys();
    for(const QString &key : qAsConst(keys)) //removes empty keys
    {
        if(params.value(key).isEmpty())
            params.remove(key);
    }

    QUrl url(m_scrobblerUrl);
    url.setPort(m_scrobblerUrl.startsWith("https") ? 443 : 80);

    QUrlQuery body("");
    QByteArray data;
    for(const QString &key : params.keys())
    {
        body.addQueryItem(key, params.value(key));
        data.append(key.toUtf8() + params.value(key).toUtf8());
    }
    data.append(SECRET);
    body.addQueryItem("api_sig", QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex());
    QByteArray bodyData = body.query(QUrl::FullyEncoded).toUtf8();
    bodyData.replace("+", QUrl::toPercentEncoding("+"));

    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", m_ua);
    request.setRawHeader("Host", url.host().toLatin1());
    request.setRawHeader("Accept", "*/*");
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    request.setHeader(QNetworkRequest::ContentLengthHeader,  bodyData.size());
    m_notificationReply = m_http->post(request, bodyData);
}

ScrobblerAuth::ScrobblerAuth(const QString &scrobblerUrl, const QString &authUrl,
                             const QString &name, QObject *parent) : QObject(parent),
    m_ua(QString("qmmp-plugins/%1").arg(Qmmp::strVersion().toLower()).toLatin1()),
    m_http(new QNetworkAccessManager(this)),
    m_scrobblerUrl(scrobblerUrl),
    m_authUrl(authUrl),
    m_name(name)
{
    connect(m_http, SIGNAL(finished (QNetworkReply *)), SLOT(processResponse(QNetworkReply *)));

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

void ScrobblerAuth::getToken()
{
    qDebug("ScrobblerAuth[%s]: new token request", qPrintable(m_name));
    m_session.clear();
    QUrl url(m_scrobblerUrl + "?");
    url.setPort(m_scrobblerUrl.startsWith("https") ? 443 : 80);
    QUrlQuery q;
    q.addQueryItem("method", "auth.getToken");
    q.addQueryItem("api_key", API_KEY);

    QByteArray data;
    data.append("api_key" API_KEY);
    data.append("methodauth.getToken");
    data.append(SECRET);
    q.addQueryItem("api_sig", QCryptographicHash::hash(data,QCryptographicHash::Md5).toHex());
    url.setQuery(q);

    QNetworkRequest request(url);
    request.setRawHeader("User-Agent",  m_ua);
    request.setRawHeader("Host",url.host().toLatin1());
    request.setRawHeader("Accept", "*/*");
    m_getTokenReply = m_http->get(request);
}

void ScrobblerAuth::getSession()
{
    qDebug("ScrobblerAuth[%s]: new session request", qPrintable(m_name));
    QUrl url(m_scrobblerUrl + "?");
    url.setPort(m_scrobblerUrl.startsWith("https") ? 443 : 80);
    QUrlQuery q;
    q.addQueryItem("api_key", API_KEY);
    q.addQueryItem("method", "auth.getSession");
    q.addQueryItem("token", m_token);

    QByteArray data;
    data.append("api_key" API_KEY);
    data.append("methodauth.getSession");
    data.append("token" + m_token.toUtf8());
    data.append(SECRET);
    q.addQueryItem("api_sig", QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex());
    url.setQuery(q);

    QNetworkRequest request(url);
    request.setRawHeader("User-Agent",  m_ua);
    request.setRawHeader("Host",url.host().toLatin1());
    request.setRawHeader("Accept", "*/*");
    m_getSessionReply = m_http->get(request);
}

void ScrobblerAuth::checkSession(const QString &session)
{
    qDebug("ScrobblerAuth[%s]: checking session...", qPrintable(m_name));
    m_session = session;
    QMap <QString, QString> params;
    params.insert("api_key", API_KEY);
    params.insert("sk", session);
    params.insert("method", "user.getInfo");

    QUrl url(m_scrobblerUrl);
    url.setPort(m_scrobblerUrl.startsWith("https") ? 443 : 80);

    QUrlQuery body("");
    QByteArray data;
    for(const QString &key : params.keys())
    {
        body.addQueryItem(key, params.value(key));
        data.append(key.toUtf8() + params.value(key).toUtf8());
    }
    data.append(SECRET);
    body.addQueryItem("api_sig", QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex());
    QByteArray bodyData = body.query(QUrl::FullyEncoded).toUtf8();
    bodyData.replace("+", QUrl::toPercentEncoding("+"));

    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", m_ua);
    request.setRawHeader("Host", url.host().toLatin1());
    request.setRawHeader("Accept", "*/*");
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    request.setHeader(QNetworkRequest::ContentLengthHeader,  bodyData.size());
    m_checkSessionReply = m_http->post(request, bodyData);
}

QString ScrobblerAuth::session() const
{
    return m_session;
}

void ScrobblerAuth::processResponse(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning("ScrobblerAuth[%s]: http error: %s", qPrintable(m_name), qPrintable(reply->errorString()));
    }

    ScrobblerResponse response;
    response.parse(reply);

    QString error_code;
    if(response.status != "ok" && !response.status.isEmpty())
    {
        if(!response.error.isEmpty())
        {
            qWarning("ScrobblerAuth[%s]: status=%s, %s-%s", qPrintable(m_name), qPrintable(response.status),
                     qPrintable(response.code), qPrintable(response.error));
            error_code = response.code;
        }
        else
            qWarning("ScrobblerAuth[%s]: invalid content", qPrintable(m_name));
    }

    if (reply == m_getTokenReply)
    {
        m_getTokenReply = nullptr;
        if(response.status == "ok")
        {
            m_token = response.token;
            qDebug("ScrobblerAuth[%s]: token: %s", qPrintable(m_name), qPrintable(m_token));
            QDesktopServices::openUrl(m_authUrl + "?api_key=" API_KEY "&token="+m_token);
            emit(tokenRequestFinished(NO_ERROR));
        }
        else if(error_code.isEmpty())
        {
             m_token.clear();
             emit tokenRequestFinished(NETWORK_ERROR);
        }
        else if(error_code == "8" || error_code == "7" ||  error_code == "11" || error_code.isEmpty())
        {
            m_token.clear();
            emit tokenRequestFinished(LASTFM_ERROR);
        }
        else
        {
            m_token.clear();
            emit tokenRequestFinished(LASTFM_ERROR);
        }
    }
    else if(reply == m_getSessionReply)
    {
        m_getSessionReply = nullptr;
        m_session.clear();
        if(response.status == "ok")
        {
            m_session = response.key;
            qDebug("ScrobblerAuth[%s]: name: %s", qPrintable(m_name), qPrintable(response.name));
            qDebug("ScrobblerAuth[%s]: key: %s", qPrintable(m_name), qPrintable(m_session));
            qDebug("ScrobblerAuth[%s]: subscriber: %s", qPrintable(m_name), qPrintable(response.subscriber));
            emit sessionRequestFinished(NO_ERROR);
        }
        else if(error_code == "4" || error_code == "15") //invalid token
        {
            m_token.clear();
            emit sessionRequestFinished(LASTFM_ERROR);
        }
        else if(error_code == "11") //service offline
        {
            m_token.clear();
            emit sessionRequestFinished(LASTFM_ERROR);
        }
        else if(error_code == "14") // unauthorized token
        {
            m_token.clear();
            emit sessionRequestFinished(LASTFM_ERROR);
        }
        else if (error_code.isEmpty()) //network error
        {
            m_token.clear();
            emit sessionRequestFinished(NETWORK_ERROR);
        }
        else
        {
            m_token.clear();
            emit sessionRequestFinished(LASTFM_ERROR);
        }
    }
    else if(reply == m_checkSessionReply)
    {
        m_checkSessionReply = nullptr;
        if(response.status == "ok")
        {
            qDebug("ScrobblerAuth[%s]: session ok", qPrintable(m_name));
            emit checkSessionFinished(NO_ERROR);
        }
        else if(error_code.isEmpty())
        {
            qWarning("ScrobblerAuth[%s]: network error", qPrintable(m_name));
            emit checkSessionFinished(NETWORK_ERROR);
        }
        else
        {
            qWarning("ScrobblerAuth[%s]: received last.fm error (code=%s)",
                     qPrintable(m_name), qPrintable(error_code));
            emit checkSessionFinished(LASTFM_ERROR);
        }
    }
    reply->deleteLater();
}
