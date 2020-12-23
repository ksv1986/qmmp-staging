/***************************************************************************
 *   Copyright (C) 2006-2021 by Ilya Kotov                                 *
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
#ifndef HTTPSTREAMREADER_H
#define HTTPSTREAMREADER_H

#include <QObject>
#include <QIODevice>
#include <QUrl>
#include <QMutex>
#include <QString>
#include <QHash>
#include <QThread>
#include <curl/curl.h>
#ifdef WITH_ENCA
#include <enca.h>
#endif

class QTextCodec;
class QFileInfo;
class DownloadThread;
class HTTPInputSource;

/*! @internal
 *   @author Ilya Kotov <forkotov02@ya.ru>
 */
struct HttpStreamData
{
    char *buf = nullptr;
    size_t buf_fill = 0;
    size_t buf_size = 0;
    QString content_type;
    bool aborted = true;
    QHash <QString, QByteArray> header;
    bool icy_meta_data = false;
    size_t icy_metaint = 0;
};

/*! @internal
 *   @author Ilya Kotov <forkotov02@ya.ru>
 */
class HttpStreamReader : public QIODevice
{
    Q_OBJECT
public:
    HttpStreamReader(const QString &url, HTTPInputSource *parent);

    virtual ~HttpStreamReader();

    /**
     *  QIODevice API
     */
    bool atEnd () const override;
    qint64 bytesAvailable () const override;
    qint64 bytesToWrite () const override;
    void close () override;
    bool isSequential () const override;
    bool open (OpenMode mode) override;
    bool seek (qint64 pos) override;
    /**
     *  helper functions
     */
    QString contentType();
    void downloadFile();
    QMutex *mutex();
    HttpStreamData *stream();
    void checkBuffer();
    void run();

signals:
    void ready();
    void error();

protected:
    qint64 readData(char*, qint64) override;
    qint64 writeData(const char*, qint64) override;

private:
    void abort();
    qint64 readBuffer(char* data, qint64 maxlen);
    void readICYMetaData();
    void parseICYMetaData(char *data, qint64 size);
    void sendStreamInfo(QTextCodec *codec);
    CURL *m_handle = nullptr;
    QMutex m_mutex;
    HttpStreamData m_stream;
    QString m_url;
    QString m_userAgent;
    size_t m_metacount = 0;
    QString m_title;
    bool m_ready = false;
    bool m_meta_sent = false;
    size_t m_prebuffer_size;
    QTextCodec *m_codec;
    DownloadThread *m_thread;
    HTTPInputSource *m_parent;
#ifdef WITH_ENCA
    EncaAnalyser m_analyser = nullptr;
    QTextCodec *m_prevCodec = nullptr;
#endif
};

class DownloadThread : public QThread
{
    Q_OBJECT
public:
    explicit DownloadThread(HttpStreamReader *parent);
    virtual ~DownloadThread ();

private:
    virtual void run() override;
    HttpStreamReader *m_parent;
};

#endif
