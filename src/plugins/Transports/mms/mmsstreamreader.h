/***************************************************************************
 *   Copyright (C) 2006-2012 by Ilya Kotov                                 *
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
#ifndef MMSSTREAMREADER_H
#define MMSSTREAMREADER_H

#include <QObject>
#include <QIODevice>
#include <QUrl>
#include <QMutex>
#include <QThread>
#define this var
#include <libmms/mmsx.h>
#undef this

class QFileInfo;
class DownloadThread;
class MMSInputSource;

/*! @internal
 *   @author Ilya Kotov <forkotov02@ya.ru>
 */
class MMSStreamReader : public QIODevice
{
    Q_OBJECT
public:
    MMSStreamReader(const QString &url, MMSInputSource *parent);

    ~MMSStreamReader();

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
    void downloadFile();


    QMutex *mutex();
    void abort();
    void checkBuffer();
    void run();

signals:
    void ready();
    void error();

private:
    virtual qint64 readData(char*, qint64) override;
    virtual qint64 writeData(const char*, qint64) override;

    QMutex m_mutex;
    QString m_url;
    mmsx_t *m_handle = nullptr;
    bool m_aborted = false;
    qint64 m_buffer_size, m_prebuf_size;
    char *m_buffer;
    qint64 m_buffer_at = 0;
    bool m_ready = false;
    DownloadThread *m_thread;
    MMSInputSource *m_parent;
};

class DownloadThread : public QThread
{
    Q_OBJECT
public:
    explicit DownloadThread(MMSStreamReader *parent);
    virtual ~DownloadThread ();
    static void usleep(unsigned long usecs)
        {
            QThread::usleep(usecs);
        }

private:
    virtual void run() override;
    MMSStreamReader *m_parent;
};

#endif
