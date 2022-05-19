/***************************************************************************
 *   Copyright (C) 2006-2022 by Ilya Kotov                                 *
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
#include <QApplication>
#include <QStringList>
#include <QDir>
#include <QSettings>
#include <stdint.h>
#include <stdlib.h>
#include <qmmp/qmmpsettings.h>
#include <qmmp/qmmp.h>
#include <qmmp/statehandler.h>
#include <qmmp/inputsource.h>
#include <qmmp/qmmptextcodec.h>
#include "httpinputsource.h"
#include "httpstreamreader.h"

#define MAX_BUFFER_SIZE 150000000UL //bytes

//curl callbacks
static size_t curl_write_data(void *data, size_t size, size_t nmemb,
                              void *pointer)
{
    HttpStreamReader *dl = static_cast<HttpStreamReader *>(pointer);
    dl->mutex()->lock();

    if(dl->stream()->buf_fill > MAX_BUFFER_SIZE)
    {
        qWarning("HttpStreamReader: buffer has reached the maximum size, disconnecting...");
        dl->stream()->aborted = true;
        dl->mutex()->unlock();
        return 0;
    }

    size_t data_size = size * nmemb;
    if(dl->stream()->buf_fill + data_size > dl->stream()->buf_size)
    {
        char *prev = dl->stream()->buf;
        dl->stream()->buf = (char *)realloc(dl->stream()->buf, dl->stream()->buf_fill + data_size);
        if(!dl->stream()->buf)
        {
            qWarning("HttpStreamReader: unable to allocate %zu bytes",  dl->stream()->buf_fill + data_size);
            if(prev)
                free(prev);

            dl->stream()->buf_fill = 0;
            dl->stream()->buf_size = 0;
            dl->stream()->aborted = true;
            dl->mutex()->unlock();
            return 0;
        }
        dl->stream()->buf_size = dl->stream()->buf_fill + data_size;
    }
    memcpy(dl->stream()->buf + dl->stream()->buf_fill, data, data_size);
    dl->stream()->buf_fill += data_size;
    dl->mutex()->unlock();
    dl->checkBuffer();
    return data_size;
}

static size_t curl_header(void *data, size_t size, size_t nmemb,
                          void *pointer)
{
    HttpStreamReader *dl = static_cast<HttpStreamReader *>(pointer);
    dl->mutex()->lock ();
    size_t data_size = size * nmemb;
    if (data_size < 3)
    {
        dl->mutex()->unlock();
        return data_size;
    }

    //qDebug("header received: %s", (char*) data);
    QByteArray header((char *) data, data_size);
    header = header.trimmed ();
    if (header.left(4).contains("HTTP"))
    {
        qDebug("HttpStreamReader: header received");
        //TODO open metadata socket
    }
    else if (header.left(4).contains("ICY"))
    {
        qDebug("HttpStreamReader: shoutcast header received");
        //dl->stream()->icy_meta_data = true;
    }
    else
    {
        QString key = QString::fromLatin1(header.left(header.indexOf(":")).trimmed().toLower());
        QByteArray value = header.right(header.size() - header.indexOf(":") - 1).trimmed();
        dl->stream()->header.insert(key, value);
        qDebug("HttpStreamReader: key=%s, value=%s",qPrintable(key), value.constData());

        if (key == "icy-metaint")
        {
            dl->stream()->icy_metaint = value.toInt();
            dl->stream()->icy_meta_data = true;
        }
        else if(key == "icy-name")
        {
            dl->stream()->icy_meta_data = true;
        }
    }
    dl->mutex()->unlock();
    return data_size;
}

int curl_progress(void *pointer, double dltotal, double dlnow, double ultotal, double ulnow)
{
    Q_UNUSED(dltotal);
    Q_UNUSED(dlnow);
    Q_UNUSED(ultotal);
    Q_UNUSED(ulnow);
    HttpStreamReader *dl = static_cast<HttpStreamReader *>(pointer);
    dl->mutex()->lock ();
    bool aborted = dl->stream()->aborted;
    dl->mutex()->unlock();
    if (aborted)
        return -1;
    return 0;
}

HttpStreamReader::HttpStreamReader(const QString &url, HTTPInputSource *parent) : QIODevice(parent),
    m_url(url),
    m_parent(parent)
{
    curl_global_init(CURL_GLOBAL_ALL);
    m_thread = new DownloadThread(this);
    QSettings settings;
    settings.beginGroup("HTTP");
    m_codec = new QmmpTextCodec(settings.value("icy_encoding","UTF-8").toByteArray ());
    m_prebuffer_size = settings.value("buffer_size",384).toInt() * 1000;
    if(settings.value("override_user_agent",false).toBool())
        m_userAgent = settings.value("user_agent").toString();
    if(m_userAgent.isEmpty())
        m_userAgent = QString("qmmp/%1").arg(Qmmp::strVersion());;
#ifdef WITH_ENCA
    if(settings.value("use_enca", false).toBool())
        m_analyser = enca_analyser_alloc(settings.value("enca_lang").toByteArray ().constData());
    if(m_analyser)
        enca_set_threshold(m_analyser, 1.38);
#endif
    settings.endGroup();
}

HttpStreamReader::~HttpStreamReader()
{
    abort();
    curl_global_cleanup();
    m_stream.aborted = true;
    m_stream.buf_fill = 0;
    m_stream.buf_size = 0;
    if (m_stream.buf)
        free(m_stream.buf);

    m_stream.buf = nullptr;
#ifdef WITH_ENCA
    if(m_analyser)
        enca_analyser_free(m_analyser);
#endif
    if(m_codec)
        delete m_codec;
}

bool HttpStreamReader::atEnd () const
{
    return false;
}

qint64 HttpStreamReader::bytesToWrite () const
{
    return -1;
}

void HttpStreamReader::close ()
{
    abort();
    QIODevice::close();
}

bool HttpStreamReader::isSequential () const
{
    return true;
}

bool HttpStreamReader::open(OpenMode mode)
{
    if(m_ready && mode == QIODevice::ReadOnly)
    {
        QIODevice::open(mode);
        return true;
    }

    return false;
}

bool HttpStreamReader::seek (qint64 pos)
{
    Q_UNUSED(pos);
    return false;
}

qint64 HttpStreamReader::writeData(const char*, qint64)
{
    return -1;
}

void HttpStreamReader::downloadFile()
{
    m_thread->start();
}

qint64 HttpStreamReader::readData(char* data, qint64 maxlen)
{

    qint64 len = 0;
    m_mutex.lock();
    if(m_stream.buf_fill == 0)
    {
        m_mutex.unlock();
        return 0;
    }
    if (m_stream.icy_metaint == 0)
        len = readBuffer(data, maxlen);
    else
    {
        size_t nread = 0;
        while (size_t(maxlen) > nread && m_stream.buf_fill > nread)
        {
            size_t to_read = qMin<size_t>(m_stream.icy_metaint - m_metacount, maxlen - nread);
            qint64 res = readBuffer(data + nread, to_read);
            nread += res;
            m_metacount += res;
            if (m_metacount == m_stream.icy_metaint)
            {
                m_metacount = 0;
                m_mutex.unlock();
                readICYMetaData();
                m_mutex.lock();
            }

        }
        len = nread;

    }
    m_mutex.unlock();
    return len;
}

HttpStreamData *HttpStreamReader::stream()
{
    return &m_stream;
}

QMutex *HttpStreamReader::mutex()
{
    return &m_mutex;
}

QString HttpStreamReader::contentType()
{
    if (m_stream.header.contains("content-type"))
        return m_stream.header.value("content-type").toLower();
    return QString();
}

void HttpStreamReader::abort()
{
    m_mutex.lock();
    m_ready = false;
    if (m_stream.aborted)
    {
        m_mutex.unlock();
        return;
    }
    m_stream.aborted = true;
    m_mutex.unlock();
    if(m_thread->isRunning())
        m_thread->wait();
    if (m_handle)
    {
        curl_easy_cleanup(m_handle);
        m_handle = nullptr;
    }
    QIODevice::close();
}

qint64 HttpStreamReader::bytesAvailable() const
{
    return QIODevice::bytesAvailable() + m_stream.buf_fill;
}

void HttpStreamReader::run()
{
    qDebug("HttpStreamReader: starting download thread");
    char errorBuffer[CURL_ERROR_SIZE];
    memset(errorBuffer, 0, sizeof(errorBuffer));
    m_handle = curl_easy_init();
    //proxy
    if (QmmpSettings::instance()->isProxyEnabled())
    {
        curl_easy_setopt(m_handle, CURLOPT_PROXY,
                         strdup((QmmpSettings::instance()->proxy().host() + ":" +
                                 QString("%1").arg(QmmpSettings::instance()->proxy().port())).
                                toLatin1 ().constData ()));

        if(QmmpSettings::instance()->proxyType() == QmmpSettings::SOCKS5_PROXY)
            curl_easy_setopt(m_handle, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5);
    }
    else
    {
        curl_easy_setopt(m_handle, CURLOPT_NOPROXY, "*");
    }

    if (QmmpSettings::instance()->useProxyAuth())
        curl_easy_setopt(m_handle, CURLOPT_PROXYUSERPWD,
                         strdup((QmmpSettings::instance()->proxy().userName() + ":" +
                                 QmmpSettings::instance()->proxy().password()).
                                toLatin1 ().constData ()));

    // Set url to download
    curl_easy_setopt(m_handle, CURLOPT_URL, strdup(m_url.toLatin1().constData()));
    // callback for wrting
    curl_easy_setopt(m_handle, CURLOPT_WRITEFUNCTION, curl_write_data);
    // Set destination file
    curl_easy_setopt(m_handle, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(m_handle, CURLOPT_HEADERDATA, this);
    curl_easy_setopt(m_handle, CURLOPT_HEADERFUNCTION, curl_header);
    // Disable SSL
    curl_easy_setopt(m_handle, CURLOPT_SSL_VERIFYPEER, false);
    curl_easy_setopt(m_handle, CURLOPT_SSL_VERIFYHOST, 0);
    // Enable progress meter
    curl_easy_setopt(m_handle, CURLOPT_NOPROGRESS, 0);
    curl_easy_setopt(m_handle, CURLOPT_PROGRESSDATA, this);
    curl_easy_setopt(m_handle, CURLOPT_PROGRESSFUNCTION, curl_progress);
    // Any kind of authentication
    curl_easy_setopt(m_handle, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
    curl_easy_setopt(m_handle, CURLOPT_VERBOSE, 1);
    // Auto referrer
    curl_easy_setopt(m_handle, CURLOPT_AUTOREFERER, 1);
    // Follow redirections
    curl_easy_setopt(m_handle, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(m_handle, CURLOPT_FAILONERROR, 1);
    curl_easy_setopt(m_handle, CURLOPT_MAXREDIRS, 15);
    // user agent
    curl_easy_setopt(m_handle, CURLOPT_USERAGENT, qPrintable(m_userAgent));
    curl_easy_setopt(m_handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_0);
    // error message
    curl_easy_setopt(m_handle, CURLOPT_ERRORBUFFER, errorBuffer);

    struct curl_slist *http200_aliases = curl_slist_append(nullptr, "ICY");
    struct curl_slist *http_headers = curl_slist_append(nullptr, "Icy-MetaData: 1");
    curl_easy_setopt(m_handle, CURLOPT_HTTP200ALIASES, http200_aliases);
    curl_easy_setopt(m_handle, CURLOPT_HTTPHEADER, http_headers);
    m_mutex.lock();
    if(m_stream.buf)
    {
        free(m_stream.buf);
        m_stream.buf = nullptr;
    }
    m_stream.buf_fill = 0;
    m_stream.buf_size = m_prebuffer_size * 2;
    m_stream.buf = (char *)malloc(m_stream.buf_size); //initial buffer
    m_stream.aborted = false;
    m_stream.header.clear();
    m_ready  = false;
    int return_code;
    qDebug("HttpStreamReader: starting libcurl");
    m_mutex.unlock();
    return_code = curl_easy_perform(m_handle);
    qDebug("HttpStreamReader: curl thread finished with code %d (%s)", return_code, errorBuffer);
    if(!m_stream.aborted && !m_ready)
    {
        setErrorString(QString::fromLocal8Bit(errorBuffer));
        emit error();
        QIODevice::close();
    }
}

qint64 HttpStreamReader::readBuffer(char* data, qint64 maxlen)
{
    if (m_stream.buf_fill > 0 && !m_stream.aborted)
    {
        int len = qMin<qint64>(m_stream.buf_fill, maxlen);
        memcpy(data, m_stream.buf, len);
        m_stream.buf_fill -= len;
        memmove(m_stream.buf, m_stream.buf + len, m_stream.buf_fill);
        return len;
    }
    else if (m_stream.aborted)
        return -1;
    else
        return 0;
}

void HttpStreamReader::checkBuffer()
{
    if(m_stream.aborted || m_ready)
        return;

    if (m_stream.buf_fill > m_prebuffer_size)
    {
        m_ready  = true;
        qDebug("HttpStreamReader: ready");
        if(!m_meta_sent)
        {
            QMap<Qmmp::MetaData, QString> metaData;
            if(stream()->icy_meta_data)
            {
                metaData.insert(Qmmp::TITLE, m_stream.header.value("icy-name"));
                metaData.insert(Qmmp::GENRE, m_stream.header.value("icy-genre"));
                m_parent->addMetaData(metaData);
                m_parent->setProperty(Qmmp::BITRATE, m_stream.header.value("icy-br"));
            }
            sendStreamInfo(m_codec);
        }
        emit ready();
    }
    else
    {
        StateHandler::instance()->dispatchBuffer(100 * m_stream.buf_fill / m_prebuffer_size);
        qApp->processEvents();
    }
}

void HttpStreamReader::readICYMetaData()
{
    uint8_t packet_size = 0;
    m_metacount = 0;
    m_mutex.lock();

    while (m_stream.buf_fill < 1 && m_thread->isRunning())
    {
        m_mutex.unlock();
        qApp->processEvents();
        m_mutex.lock();
    }
    readBuffer((char *)&packet_size, 1);
    if (packet_size != 0)
    {
        size_t size = packet_size * 16;
        char packet[size];
        while (m_stream.buf_fill < size && m_thread->isRunning())
        {
            m_mutex.unlock();
            qApp->processEvents();
            m_mutex.lock();
        }
        qint64 l = readBuffer(packet, size);
        qDebug("HttpStreamReader: ICY metadata: %s", packet);
        parseICYMetaData(packet, l);
    }
    m_mutex.unlock();
}

void HttpStreamReader::parseICYMetaData(char *data, qint64 size)
{
    if(!size)
        return;
#ifdef WITH_ENCA
    if(m_analyser)
    {
        EncaEncoding encoding = enca_analyse(m_analyser, (uchar *) data, size);
        if(encoding.charset != ENCA_CS_UNKNOWN)
        {
            qDebug("HttpStreamReader: detected charset: %s",
                   enca_charset_name(encoding.charset,ENCA_NAME_STYLE_ENCA));
            delete m_codec;
            m_codec = new QmmpTextCodec(enca_charset_name(encoding.charset,ENCA_NAME_STYLE_ENCA));
        }
    }
#endif
    QString str = m_codec->toUnicode(data).trimmed();
    const QStringList list(str.split(";", Qt::SkipEmptyParts));
    for(QString line : qAsConst(list))
    {
        if (line.contains("StreamTitle="))
        {
            line = line.right(line.size() - line.indexOf("=") - 1).trimmed();
            m_title = line.remove("'");
            QMap<Qmmp::MetaData, QString> metaData;
            if (!m_title.isEmpty())
            {
                QStringList l = m_title.split(" - ");
                if(l.count() > 1)
                {
                    metaData.insert(Qmmp::ARTIST, l.at(0));
                    metaData.insert(Qmmp::TITLE, l.at(1));
                }
                else
                    metaData.insert(Qmmp::TITLE, m_title);
            }
            else
                metaData.insert(Qmmp::TITLE, m_codec->toUnicode(m_stream.header.value("icy-name")));
            metaData.insert(Qmmp::GENRE, m_codec->toUnicode(m_stream.header.value("icy-genre")));
            m_parent->addMetaData(metaData);
            sendStreamInfo(m_codec);
            m_meta_sent = true;
            break;
        }
    }
}

void HttpStreamReader::sendStreamInfo(QmmpTextCodec *codec)
{
    QHash<QString, QString> info;
    for(const QString &key : m_stream.header.keys())
    {
        info.insert(key, codec->toUnicode(m_stream.header.value(key)));
    }
    m_parent->addStreamInfo(info);
}

DownloadThread::DownloadThread(HttpStreamReader *parent) : QThread(parent)
{
    m_parent = parent;
}

DownloadThread::~DownloadThread (){}

void DownloadThread::run()
{
    m_parent->run();
}
