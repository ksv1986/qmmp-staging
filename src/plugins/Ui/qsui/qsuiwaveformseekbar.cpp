/***************************************************************************
 *   Copyright (C) 2020 by Ilya Kotov                                      *
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

#include <QPainter>
#include <QPaintEvent>
#include <QtDebug>
#include <cmath>
#include <qmmp/soundcore.h>
#include <qmmp/inputsource.h>
#include <qmmp/decoder.h>
#include <qmmp/decoderfactory.h>
#include <qmmp/audioconverter.h>
#include <qmmp/buffer.h>
#include "qsuiwaveformseekbar.h"

QSUIWaveformSeekBar::QSUIWaveformSeekBar(QWidget *parent) : QWidget(parent)
{
    m_core = SoundCore::instance();
    connect(m_core, SIGNAL(trackInfoChanged()), SLOT(onTrackInfoChanged()));
    connect(m_core, SIGNAL(stateChanged(Qmmp::State)), SLOT(onStateChanged(Qmmp::State)));
    connect(m_core, SIGNAL(elapsedChanged(qint64)), SLOT(onElapsedChanged(qint64)));
}

QSize QSUIWaveformSeekBar::sizeHint() const
{
    return QSize(200, 100);
}

void QSUIWaveformSeekBar::onTrackInfoChanged()
{
    m_core->trackInfo().path();
}

void QSUIWaveformSeekBar::onStateChanged(Qmmp::State state)
{
    switch (state)
    {
    case Qmmp::Playing:
    {
        if(!m_scanner)
        {
            m_scanner = new QSUIWaveformScanner(this);
            connect(m_scanner, SIGNAL(finished()), SLOT(onScanFinished()));
        }
        m_scanner->scan(m_core->path());
    }
        break;
    case Qmmp::Stopped:
    case Qmmp::FatalError:
    case Qmmp::NormalError:
    {
        if(m_scanner)
        {
            m_scanner->stop();
            delete m_scanner;
            m_scanner = nullptr;
        }
        m_data.clear();
        m_elapsed = 0;
        m_duration = 0;
        update();
    }
        break;
    default:
        break;
    }
}

void QSUIWaveformSeekBar::onScanFinished()
{
    if(!m_scanner)
        return;

    m_data = m_scanner->data();
    m_channels = m_scanner->audioParameters().channels();
    delete m_scanner;
    m_scanner = nullptr;
    update();
}

void QSUIWaveformSeekBar::onElapsedChanged(qint64 elapsed)
{
    m_elapsed = elapsed;
    m_duration = m_core->duration();
    update();
}

void QSUIWaveformSeekBar::paintEvent(QPaintEvent *e)
{
    QPainter painter (this);
    painter.fillRect(e->rect(), Qt::black);
    painter.setPen("#BECBFF");

    if(m_data.isEmpty())
        return;

    float step = (float)width() / (m_data.count() / 3 / m_channels);

    painter.setPen("#BECBFF");

    for(int i = 0; i < m_data.count(); i+=3)
    {
        int ch = (i / 3) % m_channels;
        float x = step * i / 3 / m_channels;

        if(ch == 0 && m_channels == 1)
        {
            float y1 = height()/2 - m_data[i] * (height() / 4) / 1000;
            float y2 = height()/2 - m_data[i+1] * (height() / 4) / 1000;

            painter.drawLine(x, y1, x, y2);

        }
        else if(ch == 0)
        {
            float y1 = height()/4 - m_data[i] * (height() / 8) / 1000;
            float y2 = height()/4 - m_data[i+1] * (height() / 8) / 1000;

            painter.drawLine(x, y1, x, y2);

        }
        else if(ch == 1)
        {
            float y1 = 3*height()/4 - m_data[i] * (height() / 8) / 1000;
            float y2 = 3*height()/4 - m_data[i+1] * (height() / 8) / 1000;

            painter.drawLine(x, y1, x, y2);

        }
    }

    painter.setPen("#DDDDDD");

    for(int i = 0; i < m_data.count(); i+=3)
    {
        int ch = (i / 3) % m_channels;
        float x = step * i / 3 / m_channels;

        if(ch == 0 && m_channels == 1)
        {
            float y3 = height()/2 - m_data[i+2] * (height() / 4) / 1000;
            float y4 = height()/2 + m_data[i+2] * (height() / 4) / 1000;
            painter.drawLine(x, y3, x, y4);
        }
        else if(ch == 0)
        {
            float y3 = height()/4 - m_data[i+2] * (height() / 8) / 1000;
            float y4 = height()/4 + m_data[i+2] * (height() / 8) / 1000;
            painter.drawLine(x, y3, x, y4);
        }
        else if(ch == 1)
        {
            float y3 = 3*height()/4 - m_data[i+2] * (height() / 8) / 1000;
            float y4 = 3*height()/4 + m_data[i+2] * (height() / 8) / 1000;
            painter.drawLine(x, y3, x, y4);
        }
    }

    if(m_duration > 0)
    {
        QColor color(Qt::white);
        color.setAlpha(120);
        QBrush brush(color);
        painter.fillRect(0, 0, width() * m_elapsed / m_duration, height(), brush);
    }
}

QSUIWaveformScanner::QSUIWaveformScanner(QObject *parent) : QThread(parent)
{

}

QSUIWaveformScanner::~QSUIWaveformScanner()
{
    stop();
}

bool QSUIWaveformScanner::scan(const QString &path)
{
    InputSource *source = InputSource::create(path, this);
    if(!source->initialize())
    {
        delete source;
        qWarning("QSUIWaveformScanner: invalid path");
        return false;
    }

    if(source->ioDevice() && !source->ioDevice()->open(QIODevice::ReadOnly))
    {
        source->deleteLater();
        qWarning("QSUIWaveformScanner: cannot open input stream, error: %s",
                 qPrintable(source->ioDevice()->errorString()));
        return false;

    }

    DecoderFactory *factory = nullptr;

    if(!source->path().contains("://"))
        factory = Decoder::findByFilePath(source->path());
    if(!factory)
        factory = Decoder::findByMime(source->contentType());
    if(!factory && source->ioDevice() && source->path().contains("://")) //ignore content of local files
        factory = Decoder::findByContent(source->ioDevice());
    if(!factory && source->path().contains("://"))
        factory = Decoder::findByProtocol(source->path().section("://",0,0));
    if(!factory)
    {
        qWarning("QSUIWaveformScanner: unsupported file format");
        source->deleteLater();
        return false;
    }
    qDebug("QSUIWaveformScanner: selected decoder: %s",qPrintable(factory->properties().shortName));
    if(factory->properties().noInput && source->ioDevice())
        source->ioDevice()->close();
    Decoder *decoder = factory->create(source->path(), source->ioDevice());
    if(!decoder->initialize())
    {
        qWarning("QSUIWaveformScanner: invalid file format");
        source->deleteLater();
        delete decoder;
        return false;
    }
    m_decoder = decoder;
    m_input = source;
    if(!decoder->totalTime())
        source->setOffset(-1);
    m_user_stop = false;
    start();
    return true;
}

void QSUIWaveformScanner::stop()
{
    if(isRunning())
    {
        m_mutex.lock();
        m_user_stop = true;
        m_mutex.unlock();
        wait();
    }

    if(m_decoder)
    {
        delete m_decoder;
        m_decoder = nullptr;
    }

    if(m_input)
    {
        delete m_input;
        m_input = nullptr;
    }
}

const QList<int> &QSUIWaveformScanner::data() const
{
    return m_data;
}

const AudioParameters &QSUIWaveformScanner::audioParameters() const
{
    return m_ap;
}

void QSUIWaveformScanner::run()
{
    m_ap = m_decoder->audioParameters();
    unsigned char tmp[QMMP_BLOCK_FRAMES * m_ap.frameSize() * 4];
    float out[QMMP_BLOCK_FRAMES * m_ap.channels() * sizeof(float)];
    AudioConverter converter;
    converter.configure(m_ap.format());
    m_data.clear();

    qint64 frames = m_decoder->totalTime() * m_ap.sampleRate() / 1000;
    int samplesForCalculation = frames / 4096 * m_ap.channels();

    m_mutex.lock();
    float max[m_ap.channels()] = { -1.0 }, min[m_ap.channels()] = { 1.0 }, rms[m_ap.channels()] = { 0 };
    int counter = 0;
    while (!m_user_stop)
    {
        m_mutex.unlock();
        qint64 len = m_decoder->read(tmp, sizeof(tmp));
        if(len > 0)
        {
            converter.toFloat(tmp, out, len / m_ap.sampleSize());

            for(uint sample = 0; sample < len / sizeof(float); sample++)
            {
                int ch = sample % m_ap.channels();
                min[ch] = qMin(min[ch], out[sample]);
                max[ch] = qMax(max[ch], out[sample]);
                rms[ch] += (out[sample] * out[sample]);

                counter++;
                if(counter >= samplesForCalculation)
                {
                    for(int ch = 0; ch < m_ap.channels(); ++ch)
                    {
                        m_data << max[ch] * 1000;
                        m_data << min[ch] * 1000;
                        m_data << std::sqrt(rms[ch] / (counter / m_ap.channels())) * 1000;
                        max[ch] = -1.0;
                        min[ch] = 1.0;
                        rms[ch] = 0;
                    }
                    counter = 0;
                }
            }
        }
        else
        {
            m_mutex.lock();
            qDebug("finished! %d", m_data.count());
            break;
        }

        m_mutex.lock();
    }

    m_mutex.unlock();
}
