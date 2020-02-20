/***************************************************************************
 *   Copyright (C) 2009-2020 by Ilya Kotov                                 *
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
#include <QFile>
#include <QDir>
#include <QSettings>
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
    m_cachePath = Qmmp::configDir() + "/lyrics/";
    m_ui.editWidget->setVisible(false);
    m_ui.titleLineEdit->setText(info->value(Qmmp::TITLE));
    m_ui.artistLineEdit->setText(info->value(Qmmp::ARTIST));
    m_ui.albumLineEdit->setText(info->value(Qmmp::ALBUM));
    m_ui.trackSpinBox->setValue(info->value(Qmmp::TRACK).toInt());
    m_ui.yearSpinBox->setValue(info->value(Qmmp::YEAR).toInt());

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
    {
        qWarning("LyricsWindow: unable to load ultimate_providers.xml");
        m_ui.textBrowser->setText(m_parser.errorString());
        return;
    }

    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    restoreGeometry(settings.value("Lyrics/geometry").toByteArray());
    m_enabledProviders = settings.value("Lyrics/enabled_providers", m_parser.defaultProviders()).toStringList();

    QDir cacheDir(m_cachePath);
    if(!cacheDir.exists())
    {
        if(!cacheDir.mkpath(cacheDir.absolutePath()))
            qWarning("LyricsWindow: unable to create cache directory");
    }
    if(!loadFromCache())
        on_refreshButton_clicked();
}

LyricsWindow::~LyricsWindow()
{
}

void LyricsWindow::onRequestFinished(QNetworkReply *reply)
{
    QString name = m_tasks.take(reply);
    QVariant redirectTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    if(reply->networkError() == QNetworkReply::NoError && code == 200)
#else
    if(reply->error() == QNetworkReply::NoError && code == 200)
#endif
    {
        QByteArray data = reply->readAll();
        LyricsProvider *provider = m_parser.provider(name);
        if(provider)
        {
            QString content = provider->format(data, m_info);

            if(content.startsWith("http") || content.startsWith("https"))
            {
                QNetworkRequest request;
                request.setUrl(content);
                request.setRawHeader("User-Agent", QString("qmmp/%1").arg(Qmmp::strVersion()).toLatin1());
                m_tasks.insert(m_http->get(request), provider->name());
                provider->skipRules(true);
            }
            else if(!content.isEmpty())
            {
                content.prepend(tr("<h2>%1 - %2</h2>").arg(m_info.value(Qmmp::ARTIST)).arg(m_info.value(Qmmp::TITLE)));
                m_ui.providerComboBox->addItem(name, content);
                if(m_ui.providerComboBox->count() == 1)
                {
                    m_ui.providerComboBox->setCurrentIndex(0);
                    m_ui.textBrowser->setHtml(content);
                    saveToCache(content);
                }
            }
            else if(m_tasks.isEmpty() && m_ui.providerComboBox->count() == 0)
            {
                m_ui.textBrowser->setHtml("<b>"+ tr("Not found") +"</b>");
            }
        }
    }
    else if(redirectTarget.isValid())
    {
        m_tasks.insert(m_http->get(QNetworkRequest(redirectTarget.toUrl())), name);
    }
    else if(m_tasks.isEmpty() && m_ui.providerComboBox->count() == 0)
    {
        m_ui.textBrowser->setText(tr("Error: %1 - %2").arg(code).arg(reply->errorString()));
        qWarning() << "error:" << reply->errorString();
    }
    else
    {
        qWarning() << "error:" << reply->errorString();
    }

    reply->deleteLater();
}

void LyricsWindow::on_refreshButton_clicked()
{
    m_ui.textBrowser->setHtml(QString("<b>%1</b>").arg(tr("Receiving")));
    m_ui.providerComboBox->clear();

    m_info.clear();
    m_info.setValue(Qmmp::TITLE,  m_ui.titleLineEdit->text());
    m_info.setValue(Qmmp::ARTIST, m_ui.artistLineEdit->text());
    m_info.setValue(Qmmp::ALBUM, m_ui.albumLineEdit->text());
    m_info.setValue(Qmmp::TRACK, m_ui.trackSpinBox->value());
    m_info.setValue(Qmmp::YEAR, m_ui.yearSpinBox->value());

    for(LyricsProvider *provider : m_parser.providers())
    {
        if(m_enabledProviders.contains(provider->name()))
        {
            QString url = provider->getUrl(m_info);
            QNetworkRequest request;
            request.setUrl(url);
            request.setRawHeader("User-Agent", QString("qmmp/%1").arg(Qmmp::strVersion()).toLatin1());
            m_tasks.insert(m_http->get(request), provider->name());
            provider->skipRules(false);
        }
    }
}

void LyricsWindow::on_editButton_clicked(bool checked)
{
    m_ui.editWidget->setVisible(checked);
}

void LyricsWindow::on_providerComboBox_activated(int index)
{
    m_ui.textBrowser->setHtml(m_ui.providerComboBox->itemData(index).toString());
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
    m_ui.providerComboBox->addItem(tr("cache"));
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

void LyricsWindow::closeEvent(QCloseEvent *)
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("Lyrics/geometry", saveGeometry());
}
