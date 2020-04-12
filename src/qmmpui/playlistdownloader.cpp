/***************************************************************************
 *   Copyright (C) 2012-2020 by Ilya Kotov                                 *
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
#include <QNetworkProxy>
#include <QNetworkReply>
#include <qmmp/qmmp.h>
#include <qmmp/qmmpsettings.h>
#include "playlistparser.h"
#include "playlistdownloader.h"

PlayListDownloader::PlayListDownloader(QObject *parent) : QObject(parent)
{
    m_ua = QString("qmmp/%1").arg(Qmmp::strVersion()).toLatin1();
    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, SIGNAL(finished (QNetworkReply *)), SLOT(readResponse(QNetworkReply *)));
    //load global proxy settings
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
        m_manager->setProxy(proxy);
    }
}

void PlayListDownloader::start(const QUrl &url, PlayListModel *model)
{
    m_model = model;
    m_url = url;
    m_redirect_url.clear();

    QNetworkRequest r;
    r.setUrl(url);
    r.setRawHeader("User-Agent", m_ua);

    if(PlayListParser::findByUrl(url)) //is it playlist?
    {
        m_downloadReply = m_manager->get(r); //download playlist
    }
    else
    {
        m_checkReply = m_manager->get(r); //check playlist
        connect(m_checkReply, SIGNAL(downloadProgress(qint64, qint64)), SLOT(onDownloadProgress(qint64, qint64)));
    }
}

void PlayListDownloader::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    Q_UNUSED(bytesTotal);
    if(bytesReceived > 20480) //20к - maximum playlist size
    {
        QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
        if(reply)
            reply->abort();
    }
}

void PlayListDownloader::readResponse(QNetworkReply *reply)
{
    if(!m_model)
    {
        reply->deleteLater();
        return;
    }

    if(reply == m_downloadReply)
    {
        m_downloadReply = nullptr;

        if(reply->error() != QNetworkReply::NoError)
        {
            emit finished(false, reply->errorString() + " (" + reply->error() + ")");
            reply->deleteLater();
            return;
        }

        QUrl url = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
        if(!url.isEmpty() && m_url != url)
        {
            reply->deleteLater();
            qDebug("PlayListDownloader: redirect to %s", qPrintable(url.toString()));
            start(url, m_model);
            return;
        }

        QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
        qDebug("PlayListDownloader: content type: %s", qPrintable(contentType));
        PlayListFormat *fmt = PlayListParser::findByMime(contentType);
        if(!fmt)
            fmt = PlayListParser::findByUrl(m_url);

        if(fmt)
        {
            m_model->loadPlaylist(fmt->properties().shortName, reply->readAll());
            emit finished(true);
        }
        else
        {
            emit finished(false, tr("Unsupported playlist format"));
        }

        reply->deleteLater();
    }
    else if(reply == m_checkReply)
    {
        m_checkReply = nullptr;

        if(reply->error() != QNetworkReply::NoError) //playlist is not available, simply add URL
        {
            m_model->add(m_url.toString());
            reply->deleteLater();
            emit finished(true);
            return;
        }

        QUrl url = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
        if(!url.isEmpty() && m_url != url)
        {
            reply->deleteLater();
            qDebug("PlayListDownloader: redirect to %s", qPrintable(url.toString()));
            start(url, m_model);
            return;
        }

        QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
        qDebug("PlayListDownloader: content type: %s", qPrintable(contentType));
        PlayListFormat *fmt = PlayListParser::findByMime(contentType);
        if(!fmt)
            fmt = PlayListParser::findByUrl(m_url);

        if(fmt)
        {
            m_model->loadPlaylist(fmt->properties().shortName, reply->readAll());
            emit finished(true);
        }
        else
        {
            m_model->add(m_url.toString());
            emit finished(true);
        }

        reply->deleteLater();
    }
    else //unknown request
    {
        reply->deleteLater();
    }
}
