/***************************************************************************
 *   Copyright (C) 2015 by Ivan Ponomarev                                  *
 *   ivantrue@gmail.com                                                    *
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

#include "outputqtmultimedia.h"
#include <qmmp/audioparameters.h>
#include <QMetaObject>
#include <QAudioOutput>
#include <QAudioFormat>
#include <QAudioDevice>
#include <QAudioSink>
#include <QMediaDevices>
#include <QSettings>
#include <QDebug>
#include <unistd.h>


OutputQtMultimedia::OutputQtMultimedia() : Output(), m_buffer(nullptr)
{
}

OutputQtMultimedia::~OutputQtMultimedia()
{
    if(m_output && m_control)
        QMetaObject::invokeMethod(m_control, "stop", Qt::QueuedConnection);

    if(m_output)
        m_output->deleteLater();
    if(m_control)
        m_control->deleteLater();
}

bool OutputQtMultimedia::initialize(quint32 freq, ChannelMap map, Qmmp::AudioFormat format)
{
    QAudioFormat qformat;
    qformat.setSampleRate(freq);
    qformat.setChannelCount(map.size());

    //Size of sample representation in input data. For 24-bit is 4, high byte is ignored.
    qint64 bytes_per_sample = AudioParameters::sampleSize(format);

    if(format == Qmmp::PCM_S24LE)
        format = Qmmp::PCM_S32LE;

    switch (format)
    {
    case Qmmp::PCM_U8:
    case Qmmp::PCM_S8:
        qformat.setSampleFormat(QAudioFormat::UInt8);
        break;
    case Qmmp::PCM_S16LE:
        qformat.setSampleFormat(QAudioFormat::Int16);
        break;
    case Qmmp::PCM_S32LE:
        qformat.setSampleFormat(QAudioFormat::Int32);
        break;
    case Qmmp::PCM_FLOAT:
        qformat.setSampleFormat(QAudioFormat::Int32);
        break;
    default:
        break;
    }

#if Q_BYTE_ORDER == Q_BIG_ENDIAN
    if(format == Qmmp::PCM_S16LE)
        format = Qmmp::PCM_S16BE;
    else if(format == Qmmp::PCM_S32LE)
        format = Qmmp::PCM_S32BE;
#endif

    if (!qformat.isValid())
        return false;

    const QSettings settings;
    const QByteArray saved_device_name = settings.value("QTMULTIMEDIA/device").toByteArray();
    m_bytes_per_second = bytes_per_sample * freq * qformat.channelCount();

    QAudioDevice device_info;
    if (!saved_device_name.isEmpty())
    {
        const QList<QAudioDevice> devices = QMediaDevices::audioOutputs();
        for(const QAudioDevice &info : devices)
        {
            if (info.id() == saved_device_name)
            {
                if (info.isFormatSupported(qformat))
                {
                    device_info = info;
                    break;
                }
                else
                    qDebug() << "OutputQtMultimedia: Output device: " << saved_device_name << " is not supported";
            }
        }
    }

    if (device_info.isNull())
    {
        device_info = QMediaDevices::defaultAudioOutput();
        if (!device_info.isFormatSupported(qformat))
            return false;
    }

    qDebug() << "OutputQtMultimedia: Using output device: " << device_info.description();

    m_output = new QAudioSink(device_info, qformat);
    m_output->setBufferSize(4096);
    m_buffer = m_output->start();
    m_control = new OutputControl(m_output);

    configure(freq, map, format);
    return true;
}


qint64 OutputQtMultimedia::latency()
{
    return (m_output->bufferSize() - m_output->bytesFree()) * 1000 / sampleSize() / channels() / sampleRate();
}

qint64 OutputQtMultimedia::writeAudio(unsigned char *data, qint64 maxSize)
{
    if (!m_output->bytesFree()) {
        //If the buffer is full, waiting for some bytes to be played:
        //trying to play maxSize bytes, but not more than half of buffer.
        usleep(qMin(maxSize, static_cast<qint64>(m_output->bufferSize() / 2)) * 1000000 / m_bytes_per_second);
    }

    return m_buffer->write((const char*)data, maxSize);
}

void OutputQtMultimedia::drain()
{
    m_buffer->waitForBytesWritten(-1);
}

void OutputQtMultimedia::reset()
{
    m_buffer->reset();
}

void OutputQtMultimedia::suspend()
{
    QMetaObject::invokeMethod(m_control, "suspend", Qt::QueuedConnection);
}

void OutputQtMultimedia::resume()
{
    QMetaObject::invokeMethod(m_control, "resume", Qt::QueuedConnection);
}

OutputControl::OutputControl(QAudioSink *o) : m_output(o)
{}

void OutputControl::suspend()
{
    m_output->suspend();
}

void OutputControl::resume()
{
    m_output->resume();
}

void OutputControl::stop()
{
    m_output->stop();
}
