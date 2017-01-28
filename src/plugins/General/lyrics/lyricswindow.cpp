/***************************************************************************
 *   Copyright (C) 2009-2017 by Ilya Kotov                                 *
 *   forkotov02@hotmail.ru                                                 *
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

LyricsWindow::LyricsWindow(const QString &artist, const QString &title, QWidget *parent)
        : QWidget(parent)
{
    m_ui.setupUi(this);
    setWindowFlags(Qt::Dialog);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_QuitOnClose, false);
    m_requestReply = 0;
    m_cachePath = Qmmp::configDir() + "lyrics/";
    m_ui.artistLineEdit->setText(artist);
    m_ui.titleLineEdit->setText(title);
    m_http = new QNetworkAccessManager(this);
     //load global proxy settings
    QmmpSettings *gs = QmmpSettings::instance();
    if (gs->isProxyEnabled())
    {
        QNetworkProxy proxy(QNetworkProxy::HttpProxy, gs->proxy().host(),  gs->proxy().port());
        if(gs->useProxyAuth())
        {
            proxy.setUser(gs->proxy().userName());
            proxy.setPassword(gs->proxy().password());
        }
        m_http->setProxy(proxy);
    }
    connect(m_http, SIGNAL(finished (QNetworkReply *)), SLOT(showText(QNetworkReply *)));

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

void LyricsWindow::showText(QNetworkReply *reply)
{
    m_ui.stateLabel->setText(tr("Done"));
    if (reply->error() != QNetworkReply::NoError)
    {
        m_ui.stateLabel->setText(tr("Error"));
        m_ui.textBrowser->setText(reply->errorString());
        m_requestReply = 0;
        reply->deleteLater();
        return;
    }
    QString content = QString::fromUtf8(reply->readAll().constData());
    //qDebug("=%s=",qPrintable(content));

    if(m_requestReply == reply)
    {
        m_requestReply = 0;
        reply->deleteLater();
        QRegExp artist_regexp("<artist>(.*)</artist>");
        artist_regexp.setMinimal(true);
        QRegExp song_regexp("<song>(.*)</song>");
        song_regexp.setMinimal(true);
        QRegExp lyrics_regexp("<lyrics>(.*)</lyrics>");
        lyrics_regexp.setMinimal(true);
        QRegExp url_regexp("<url>(.*)</url>");
        url_regexp.setMinimal(true);

        if(artist_regexp.indexIn(content) < 0)
        {
            m_ui.textBrowser->setHtml("<b>" + tr("Error") + "</b>");
            return;
        }

        if(artist_regexp.indexIn(content) < 0)
        {
            m_ui.textBrowser->setHtml("<b>" + tr("Error") + "</b>");
            return;
        }
        else
            m_artist = artist_regexp.cap(1);

        if(song_regexp.indexIn(content) < 0)
        {
            m_ui.textBrowser->setHtml("<b>" + tr("Error") + "</b>");
            return;
        }
        else
            m_title = song_regexp.cap(1);

        if(lyrics_regexp.indexIn(content) < 0)
        {
            m_ui.textBrowser->setHtml("<b>" + tr("Error") + "</b>");
            return;
        }
        else if(lyrics_regexp.cap(1) == "Not found")
        {
            m_ui.textBrowser->setHtml("<b>" + tr("Not found") + "</b>");
            return;
        }

        if(url_regexp.indexIn(content) < 0)
        {
            m_ui.textBrowser->setHtml("<b>" + tr("Error") + "</b>");
            return;
        }

        QString temp = url_regexp.cap(1).toLatin1();
        qDebug("LyricsWindow: received url = %s", qPrintable(temp));
        temp.replace("http://lyrics.wikia.com/","http://lyrics.wikia.com/index.php?title=");
        temp.append("&action=edit");

        QUrl url = QUrl::fromEncoded(temp.toLatin1());
        QString referer = url_regexp.cap(1);
        qDebug("LyricsWindow: request url = %s", url.toEncoded().constData());
        QNetworkRequest request;
        request.setUrl(url);
        request.setRawHeader("Referer", referer.toLatin1());
        m_ui.stateLabel->setText(tr("Receiving"));
        m_http->get(request);
        reply->deleteLater();
        return;
    }
    content.replace("&lt;", "<");
    QRegExp lyrics_regexp("<lyrics>(.*)</lyrics>");
    lyrics_regexp.indexIn(content);

    QString text = "<h2>" + m_artist + " - " + m_title + "</h2>";
    QString lyrics = lyrics_regexp.cap(1);
    lyrics = lyrics.trimmed();
    lyrics.replace("\n", "<br>");
    if(lyrics.isEmpty())
        m_ui.textBrowser->setHtml("<b>" + tr("Not found") + "</b>");
    else
    {
        text += lyrics;
        m_ui.textBrowser->setHtml(text);
        saveToCache(text);

    }
    reply->deleteLater();
}

void LyricsWindow::on_searchPushButton_clicked()
{
    m_ui.stateLabel->setText(tr("Receiving"));
    setWindowTitle(QString(tr("Lyrics: %1 - %2")).arg(m_ui.artistLineEdit->text())
                   .arg(m_ui.titleLineEdit->text()));
    QNetworkRequest request;
    request.setUrl(QUrl("http://lyrics.wikia.com/api.php?action=lyrics&artist=" +
                        m_ui.artistLineEdit->text()+"&song=" + m_ui.titleLineEdit->text() + "&fmt=xml"));
    request.setRawHeader("User-Agent", QString("qmmp/%1").arg(Qmmp::strVersion()).toLatin1());
    m_requestReply = m_http->get(request);
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
