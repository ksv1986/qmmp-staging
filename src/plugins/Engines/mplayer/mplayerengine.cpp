/***************************************************************************
 *   Copyright (C) 2008-2019 by Ilya Kotov                                 *
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

#include <QObject>
#include <QFile>
#include <QApplication>
#include <QAction>
#include <QKeyEvent>
#include <QMenu>
#include <QRegExp>
#include <QSettings>
#include <QFileInfo>
#include <QDebug>
#include <qmmp/trackinfo.h>
#include <qmmp/inputsource.h>
#include <qmmp/volumehandler.h>
#include "mplayerengine.h"

//#define MPLAYER_DEBUG

static QRegExp rx_av("^[AV]: *([0-9,:.-]+)");
static QRegExp rx_pause("^(.*)=(.*)PAUSE(.*)");
static QRegExp rx_end("^(.*)End of file(.*)");
static QRegExp rx_quit("^(.*)Quit(.*)");
static QRegExp rx_audio("^AUDIO: *([0-9,.]+) *Hz.*([0-9,.]+) *ch.*([0-9]+).* ([0-9,.]+) *kbit.*");
static QRegExp rx_audio2("^AUDIO: *([0-9,.]+) *Hz.*([0-9,.]+) *ch.*([a-z]+).* ([0-9,.]+) *kbit.*");

TrackInfo *MplayerInfo::createTrackInfo(const QString &path)
{
    QRegExp rx_id_length("^ID_LENGTH=([0-9,.]+)*");
    QRegExp rx_id_audio_bitrate("^ID_AUDIO_BITRATE=([0-9,.]+)*");
    QRegExp rx_id_audio_rate("^ID_AUDIO_RATE=([0-9,.]+)*");
    QRegExp rx_id_audio_nch("^ID_AUDIO_NCH=([0-9,.]+)*");
    QRegExp rx_id_audio_codec("^ID_AUDIO_CODEC=(.*)");
    QStringList args;
    args << "-slave";
    args << "-identify";
    args << "-frames";
    args << "0";
    args << "-vo";
    args << "null";
    args << "-ao";
    args << "null";
    args << path;
    QProcess mplayer_process;
    mplayer_process.start("mplayer", args);
    mplayer_process.waitForFinished(1500);
    mplayer_process.kill();
    QString str = QString::fromLocal8Bit(mplayer_process.readAll()).trimmed();
    TrackInfo *info = new TrackInfo(path);
    QStringList lines = str.split("\n");
    foreach(QString line, lines)
    {
        if(rx_id_length.indexIn(line) > -1)
            info->setDuration((qint64) rx_id_length.cap(1).toDouble() * 1000);
        else if(rx_id_audio_bitrate.indexIn(line) > -1)
            info->setValue(Qmmp::BITRATE, rx_id_audio_bitrate.cap(1).toDouble());
        else if(rx_id_audio_rate.indexIn(line) > -1)
            info->setValue(Qmmp::SAMPLERATE, rx_id_audio_rate.cap(1).toDouble());
        else if(rx_id_audio_nch.indexIn(line) > -1)
            info->setValue(Qmmp::CHANNELS, rx_id_audio_nch.cap(1).toInt());
        else if(rx_id_audio_codec.indexIn(line) > -1)
            info->setValue(Qmmp::FORMAT_NAME, rx_id_audio_codec.cap(1));
    }
    info->setValue(Qmmp::BITS_PER_SAMPLE, 32);
    info->setValue(Qmmp::DECODER, "mplayer");
    info->setValue(Qmmp::FILE_SIZE, QFileInfo(path).size());
#ifdef MPLAYER_DEBUG
    qDebug("%s",qPrintable(str));
#endif
    return info;
}

QStringList MplayerInfo::filters()
{
    QStringList filters;
    filters << "*.avi" << "*.mpg" << "*.mpeg" << "*.divx" << "*.qt" << "*.mov" << "*.wmv" << "*.asf"
            << "*.flv" << "*.3gp" << "*.mkv" << "*.mp4" << "*.webm";
    return filters;
}

MplayerEngine::MplayerEngine(QObject *parent)
        : AbstractEngine(parent)
{
    connect(VolumeHandler::instance(), SIGNAL(mutedChanged(bool)), SLOT(setMuted(bool)));
}

MplayerEngine::~MplayerEngine()
{
    qDebug("%s",__FUNCTION__);
    if(m_process)
        m_process->kill();
    while(!m_sources.isEmpty())
        m_sources.dequeue()->deleteLater();
}

bool MplayerEngine::play()
{
    m_user_stop = false;
    if(m_process && m_process->state() != QProcess::NotRunning)
        return false;
    startMplayerProcess();
    return true;
}

bool MplayerEngine::enqueue(InputSource *source)
{
    QStringList filters = MplayerInfo::filters();
    bool supports = false;
    foreach(QString filter, filters)
    {
        QRegExp regexp(filter, Qt::CaseInsensitive, QRegExp::Wildcard);
        supports = regexp.exactMatch(source->path());
        if(supports)
            break;
    }
    if(!supports)
        return false;

    if(!m_process || m_process->state() == QProcess::NotRunning)
        m_source = source;
    else
        m_sources.enqueue(source);
    return true;
}

bool MplayerEngine::initialize()
{
    TrackInfo *info = MplayerInfo::createTrackInfo(m_source->path());
    m_length = info->duration();
    delete info;
    m_args.clear();
    m_args << "-slave";
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    QString ao_str = settings.value("mplayer/ao","default").toString();
    QString vo_str = settings.value("mplayer/vo","default").toString();
    if (ao_str != "default")
        m_args << "-ao" << ao_str;
    if (vo_str != "default")
        m_args << "-vo" << vo_str;

    if (settings.value("autosync", false).toBool())
        m_args << "-autosync" << QString("%1").arg(settings.value("autosync_factor", 100).toInt());

    m_args << settings.value("cmd_options").toString().split(" ", QString::SkipEmptyParts);

    if(m_source->offset() > 0)
        m_args << "-ss" << QString("%1").arg(m_source->offset()/1000);
    m_args << m_source->path();
    return true;
}

void MplayerEngine::seek(qint64 pos)
{
    if (m_process && m_process->state() == QProcess::Running)
        m_process->write(QString("seek %1\n").arg(pos/1000 - m_currentTime).toLocal8Bit ());
}

void MplayerEngine::stop()
{
    while(!m_sources.isEmpty())
        m_sources.dequeue()->deleteLater();
    if(m_process && m_process->state() == QProcess::Running)
    {
        m_user_stop = true;
        m_process->write("quit\n");
        m_process->waitForFinished(3500);
        m_process->kill();
        StateHandler::instance()->dispatch(Qmmp::Stopped);
    }
}

void MplayerEngine::pause()
{
    if(m_process)
        m_process->write("pause\n");
}

void MplayerEngine::setMuted(bool muted)
{
    if(m_process && m_process->state() == QProcess::Running)
    {
        m_process->write(muted ? "mute 1\n" : "mute 0\n");
    }
}

void MplayerEngine::readStdOut()
{
    QString line = QString::fromLocal8Bit(m_process->readAll ()).trimmed();
    QStringList lines = line.split("\n");
    foreach(line, lines)
    {
        if (rx_av.indexIn(line) > -1)
        {
            StateHandler::instance()->dispatch(Qmmp::Playing);
            m_currentTime = (qint64) rx_av.cap(1).toDouble();
            StateHandler::instance()->dispatch(m_currentTime * 1000, m_bitrate);
        }
        else if (rx_pause.indexIn(line) > -1)
        {
            StateHandler::instance()->dispatch(Qmmp::Paused);
        }
        else if (rx_end.indexIn(line) > -1)
        {
            if (m_process->state() == QProcess::Running)
                m_process->waitForFinished(3500);
            StateHandler::instance()->sendFinished();
            if(!m_sources.isEmpty())
            {
                StateHandler::instance()->dispatch(Qmmp::Stopped);
                m_source = m_sources.dequeue();
                startMplayerProcess();
            }
            else
            {
                StateHandler::instance()->dispatch(Qmmp::Stopped);
                return;
            }
        }
        else if (rx_quit.indexIn(line) > -1 && !m_user_stop)
        {
            if (m_process->state() == QProcess::Running)
            {
                m_process->waitForFinished(1500);
                m_process->kill();
            }
            StateHandler::instance()->dispatch(Qmmp::Stopped);
        }
        else if (rx_audio.indexIn(line) > -1)
        {
            m_samplerate = rx_audio.cap(1).toInt();
            m_channels = rx_audio.cap(2).toInt();
            m_bitsPerSample = rx_audio.cap(3).toDouble();
            m_bitrate = rx_audio.cap(4).toDouble();
            AudioParameters ap(m_samplerate, ChannelMap(m_channels), AudioParameters::findAudioFormat(m_bitsPerSample));
            StateHandler::instance()->dispatch(ap);
        }
        else if (rx_audio2.indexIn(line) > -1)
        {
            m_samplerate = rx_audio2.cap(1).toInt();
            m_channels = rx_audio2.cap(2).toInt();
            m_bitsPerSample = 32;
            m_bitrate = rx_audio2.cap(4).toDouble();
            AudioParameters ap(m_samplerate, ChannelMap(m_channels), AudioParameters::findAudioFormat(m_bitsPerSample));
            StateHandler::instance()->dispatch(ap);
        }
#ifdef MPLAYER_DEBUG
        else
            qDebug("%s",qPrintable(line));
#endif
    }
}

void MplayerEngine::onError(QProcess::ProcessError error)
{
    if(error == QProcess::FailedToStart || error == QProcess::Crashed)
        StateHandler::instance()->dispatch(Qmmp::FatalError);
    qWarning("MplayerEngine: process error: %d", error);
}

void MplayerEngine::onStateChanged(QProcess::ProcessState state)
{
    if(state == QProcess::Running)
        setMuted(VolumeHandler::instance()->isMuted());
}

void MplayerEngine::startMplayerProcess()
{
    initialize();
    if(m_process)
        delete m_process;
    m_process = new QProcess(this);
    connect(m_process, SIGNAL(readyReadStandardOutput()), SLOT(readStdOut()));
    connect(m_process, SIGNAL(error(QProcess::ProcessError)), SLOT(onError(QProcess::ProcessError)));
    connect(m_process, SIGNAL(stateChanged(QProcess::ProcessState)), SLOT(onStateChanged(QProcess::ProcessState)));
    m_process->start ("mplayer", m_args);
    StateHandler::instance()->dispatch(Qmmp::Playing);
    StateHandler::instance()->dispatch(m_length);
    TrackInfo *info = MplayerInfo::createTrackInfo(m_source->path());
    StateHandler::instance()->dispatch(*info);
    delete info;
    m_source->deleteLater();
    m_source = nullptr;
    m_currentTime = 0;
}
