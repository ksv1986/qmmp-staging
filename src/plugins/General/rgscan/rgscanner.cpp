/***************************************************************************
 *   Copyright (C) 2013-2021 by Ilya Kotov                                 *
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

#include <QStringList>
#include <math.h>
#include <qmmp/inputsourcefactory.h>
#include <qmmp/decoderfactory.h>
#include <qmmp/audioconverter.h>
#include "rgscanner.h"

#define BUFFER_FRAMES 4096

RGScanner::RGScanner()
{
    m_gain = 0.;
    m_peak = 0.;
    m_user_stop = false;
    m_is_running = false;
    m_is_pending = false;
    m_has_values = false;
    m_handle = nullptr;
    m_decoder = nullptr;
    m_source = nullptr;
}

RGScanner::~RGScanner()
{
    stop();
    deinit();
    if(m_handle)
    {
        DeinitGainAnalysis(m_handle);
        m_handle = nullptr;
    }
}

bool RGScanner::prepare(const QString &url)
{
    m_is_pending = false;
    deinit();
    m_url = url;
    QString name = m_url.section("/", -1);
    InputSource *source = InputSource::create(url, nullptr);
    if(!source->initialize())
    {
        delete source;
        qWarning("RGScanner: Invalid url");
        return false;
    }

    if(source->ioDevice() && !source->ioDevice()->open(QIODevice::ReadOnly))
    {
        qWarning("RGScanner: [%s] unable to open input stream, error: %s",
                 qPrintable(name),
                 qPrintable(source->ioDevice()->errorString()));
        delete source;
        return false;
    }

    DecoderFactory *factory = Decoder::findByFilePath(source->path());

    if(!factory)
    {
        qWarning("RGScanner: [%s] unable to find factory", qPrintable(name));
        delete source;
        return false;
    }
    qDebug("RGScanner: [%s] selected decoder: %s",qPrintable(name),
           qPrintable(factory->properties().shortName));

    if(factory->properties().noInput && source->ioDevice())
        source->ioDevice()->close();

    Decoder *decoder = factory->create(source->path(), source->ioDevice());
    if(!decoder->initialize())
    {
        qWarning("RGScanner: [%s] invalid file format", qPrintable(name));
        delete source;
        delete decoder;
        return false;
    }
    if(decoder->audioParameters().channels() > 2)
    {
        qWarning("RGScanner: [%s] unsupported channel number: %d",
                 qPrintable(name),
                 decoder->audioParameters().channels());
        delete source;
        delete decoder;
        return false;
    }
    m_decoder = decoder;
    m_source = source;
    m_user_stop = false;
    m_has_values = false;
    m_is_pending = true;
    return true;
}

void RGScanner::stop()
{
    m_mutex.lock();
    m_user_stop = true;
    m_mutex.unlock();
}

bool RGScanner::isRunning() const
{
    return m_is_running;
}

bool RGScanner::isPending() const
{
    return m_is_pending;
}

bool RGScanner::hasValues() const
{
    return m_has_values;
}

QMap<Qmmp::ReplayGainKey, double> RGScanner::oldReplayGainInfo() const
{
    if(!m_decoder)
        return QMap<Qmmp::ReplayGainKey, double>();

    return m_decoder->replayGainInfo();
}

double RGScanner::gain() const
{
    return m_gain;
}

double RGScanner::peak() const
{
    return m_peak;
}

QString RGScanner::url() const
{
    return m_url;
}

GainHandle_t *RGScanner::handle()
{
    return m_handle;
}

void RGScanner::run()
{
    if(m_user_stop)
    {
        m_is_pending = false;
        return;
    }
    QString name = m_url.section("/", -1);
    qDebug("RGScanner: [%s] staring thread", qPrintable(name));
    m_is_running = true;
    m_is_pending = false;
    bool error = false;

    AudioParameters ap = m_decoder->audioParameters();
    AudioConverter converter;
    converter.configure(ap.format());
    //buffers
    double out_left[BUFFER_FRAMES] = {0}, out_right[BUFFER_FRAMES] = {0}; //replay gain buffers
    float float_buf[BUFFER_FRAMES*ap.channels()]; //float buffer
    qint64 buf_size = BUFFER_FRAMES*ap.frameSize();
    unsigned char char_buf[buf_size]; //char buffer


    //counters
    qint64 totalSamples = m_decoder->totalTime() * ap.sampleRate() * ap.channels() / 1000, len = 0;
    quint64 sample_counter = 0;
    quint64 samples = 0;
    double max = 0;

    if(m_handle)
        DeinitGainAnalysis(m_handle);
    InitGainAnalysis(&m_handle, ap.sampleRate());

    forever
    {
        len = m_decoder->read(char_buf, buf_size);

        if(len < 0)
        {
            error = true;
            break;
        }
        else if(len == 0)
            break;

        samples = len / ap.sampleSize();

        converter.toFloat(char_buf, float_buf, samples);

        if(ap.channels() == 2)
        {
            for(uint i = 0; i < (samples >> 1); ++i)
            {
                out_left[i] = float_buf[i*2]*32768.0;
                out_right[i] = float_buf[i*2+1]*32768.0;
                max = qMax(fabs(out_left[i]), max);
                max = qMax(fabs(out_right[i]), max);
            }
        }
        else if(ap.channels() == 1)
        {
            for(uint i = 0; i < samples; ++i)
            {
                out_left[i] = float_buf[i]*32768.0;
                max = qMax(fabs(out_left[i]), max);
            }
        }

        size_t samples_per_channel = samples >> ((ap.channels() == 2) ? 1 : 0);

        AnalyzeSamples(m_handle, out_left, out_right, samples_per_channel, ap.channels());
        sample_counter += samples;
        emit progress(100 * sample_counter / totalSamples);

        m_mutex.lock();
        if(m_user_stop)
        {
            m_mutex.unlock();
            break;
        }
        m_mutex.unlock();
    }

    if(error)
    {
        qWarning("RGScanner: [%s] finished with error", qPrintable(name));
    }
    else if(m_user_stop)
    {
        qDebug("RGScanner: [%s] stopped by user", qPrintable(name));
    }
    else
    {
        m_gain = GetTitleGain(m_handle);
        m_peak = max/32768.0;
        emit progress(100);
        qDebug("RGScanner: [%s] peak=%f, gain=%f", qPrintable(name), m_peak, m_gain);
        qDebug("RGScanner: [%s] finished with success ", qPrintable(name));
        m_has_values = true;
    }
    deinit();
    m_is_running = false;
    emit finished(m_url);
}

void RGScanner::deinit()
{
    if(m_decoder)
    {
        delete m_decoder;
        m_decoder = nullptr;
    }
    if(m_source)
    {
        delete m_source;
        m_source = nullptr;
    }
}
