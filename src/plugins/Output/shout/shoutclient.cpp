/***************************************************************************
 *   Copyright (C) 2017 by Ilya Kotov                                      *
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

#include <QTimer>
#include <QSettings>
#include <qmmp/qmmp.h>
#include "shoutclient.h"

ShoutClient::ShoutClient(QObject *parent) :
    QObject(parent)
{
    shout_init();
    m_shout_conn = shout_new();
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    m_timer->setInterval(4000);
    QObject::connect(m_timer, SIGNAL(timeout()), SLOT(close()));
    readSettings();
}

ShoutClient::~ShoutClient()
{
    close();
    shout_free(m_shout_conn);
    shout_shutdown();
}

void ShoutClient::readSettings()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Shout");
    shout_set_host(m_shout_conn, settings.value("host", "127.0.0.1").toString().toLatin1().constData());
    shout_set_port(m_shout_conn, settings.value("port", 8000).toInt());
    shout_set_password(m_shout_conn, settings.value("passw", "hackme").toString().toLatin1().constData());
    shout_set_mount(m_shout_conn, QString("/%1").arg(settings.value("mount", "qmmp.out").toString()).
                    toLatin1().constData());
    shout_set_name(m_shout_conn, "qmmp");
    shout_set_user(m_shout_conn, settings.value("user", "source").toString().toLatin1().constData());
    shout_set_public(m_shout_conn, settings.value("public", false).toBool() ? 1 : 0);
    shout_set_format(m_shout_conn, SHOUT_FORMAT_OGG);
    shout_set_protocol(m_shout_conn, SHOUT_PROTOCOL_HTTP);
    shout_set_agent(m_shout_conn, "qmmp");
    shout_set_audio_info(m_shout_conn, SHOUT_AI_CHANNELS, "2");
    shout_set_audio_info(m_shout_conn, SHOUT_AI_QUALITY,
                         QString::number(settings.value("vorbis_quality", 0.8).toDouble(), 'f').toLatin1().constData());
    shout_set_audio_info(m_shout_conn, SHOUT_AI_SAMPLERATE,
                         QString::number(settings.value("sample_rate", 44100).toInt()).toLatin1().constData());
    settings.endGroup();
}

bool ShoutClient::open()
{
    QMetaObject::invokeMethod(m_timer, "stop", Qt::QueuedConnection);
    int r = shout_open(m_shout_conn);

    if(r == SHOUTERR_SUCCESS || r == SHOUTERR_CONNECTED)
    {
        shout_sync(m_shout_conn);
        qDebug("ShoutClient: connected");
        return true;
    }

    qWarning("ShoutClient: unable to connect: %s", shout_get_error(m_shout_conn));
    return false;
}

bool ShoutClient::send(const unsigned char *data, int len)
{
    shout_sync(m_shout_conn);
    if(shout_send(m_shout_conn, data, len) != SHOUTERR_SUCCESS)
    {
        qWarning("ShoutClient: unable to send data: %s", shout_get_error(m_shout_conn));
        return false;
    }
    return true;
}

qint64 ShoutClient::latency() const
{
    return shout_delay(m_shout_conn);
}

void ShoutClient::closeLater()
{
    QMetaObject::invokeMethod(m_timer, "start", Qt::QueuedConnection);
}

void ShoutClient::close()
{
    qDebug("%s", Q_FUNC_INFO);
    shout_close(m_shout_conn);
}
