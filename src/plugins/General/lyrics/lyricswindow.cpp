/***************************************************************************
 *   Copyright (C) 2009-2019 by Ilya Kotov                                 *
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

#include <QtDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QUrl>
#include <QRegExp>
#include <QFile>
#include <QDir>
#include <QCryptographicHash>
#include <qmmp/qmmpsettings.h>
#include <qmmp/qmmp.h>
#include "lyricswindow.h"

LyricsWindow::LyricsWindow(const TrackInfo *info, QWidget *parent)
        : QWidget(parent)
{
    m_ui.setupUi(this);
    setWindowFlags(Qt::Dialog);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_QuitOnClose, false);
    m_info = *info;
    m_cachePath = Qmmp::configDir() + "/lyrics/";
    //m_ui.artistLineEdit->setText(info);
    //m_ui.titleLineEdit->setText(title);
    m_http = new QNetworkAccessManager(this);
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
        m_http->setProxy(proxy);
    }
    connect(m_http, SIGNAL(finished (QNetworkReply *)), SLOT(onRequestFinished(QNetworkReply *)));

    if(!m_parser.load(":/ultimate_providers.xml"))
        qWarning("LyricsWindow: unable to load ultimate_providers.xml");

    QDir cacheDir(m_cachePath);
    if(!cacheDir.exists())
    {
        if(!cacheDir.mkpath(cacheDir.absolutePath()))
            qWarning("LyricsWindow: unable to create cache directory");
    }
    if(!loadFromCache())
        on_searchPushButton_clicked();
}


LyricsWindow::~LyricsWindow()
{
}

void LyricsWindow::onRequestFinished(QNetworkReply *reply)
{
    QString name = m_tasks.take(reply);
    QVariant redirectTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    if(reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200)
    {
        QByteArray data = reply->readAll();
        LyricsProvider *provider = m_parser.provider(name);
        if(provider)
        {
            QString content = provider->format(data);
            qDebug() << content;
        }

        qDebug() << "+++++";
    }
    else if(redirectTarget.isValid())
    {
        m_tasks.insert(m_http->get(QNetworkRequest(redirectTarget.toUrl())), name);
    }

    reply->deleteLater();
}

void LyricsWindow::on_searchPushButton_clicked()
{
    m_ui.stateLabel->setText(tr("Receiving"));

    for(LyricsProvider *provider : m_parser.providers())
    {
        QString url = provider->getUrl(m_info);
        qDebug() << provider->name();
        QNetworkRequest request;
        request.setUrl(url);
        request.setRawHeader("User-Agent", QString("qmmp/%1").arg(Qmmp::strVersion()).toLatin1());
        m_tasks.insert(m_http->get(request), provider->name());
    }
}

QString LyricsWindow::cacheFilePath() const
{
    QString name = m_ui.artistLineEdit->text() + "_" + m_ui.titleLineEdit->text();
    QByteArray hash = QCryptographicHash::hash(name.toUtf8(), QCryptographicHash::Md5);
    return m_cachePath + QString::fromLatin1(hash.toHex()) + ".html";
}

bool LyricsWindow::loadFromCache()
{
    QFile file(cacheFilePath());
    if(!file.exists())
        return false;

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning("LyricsWindow: unable to open cache file '%s', error: %s",
                 qPrintable(file.fileName()), qPrintable(file.errorString()));
        return false;
    }

    m_ui.textBrowser->setHtml(QString::fromUtf8(file.readAll()));
    m_ui.stateLabel->setText(tr("Done"));
    return true;
}

void LyricsWindow::saveToCache(const QString &text)
{
    QFile file(cacheFilePath());
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qWarning("LyricsWindow: unable to open cache file '%s', error: %s",
                 qPrintable(file.fileName()), qPrintable(file.errorString()));
        return;
    }
    file.write(text.toUtf8());
}
