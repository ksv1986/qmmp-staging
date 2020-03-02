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
#include <QSettings>
#include <QToolTip>
#include <QMenu>
#include <QAction>
#include <QMutexLocker>
#include <cmath>
#include <qmmp/soundcore.h>
#include <qmmp/inputsource.h>
#include <qmmp/decoder.h>
#include <qmmp/decoderfactory.h>
#include <qmmp/audioconverter.h>
#include <qmmp/buffer.h>
#include <qmmpui/metadataformatter.h>
#include "qsuiwaveformseekbar.h"

#define NUMBER_OF_VALUES (4096)

QSUiWaveformSeekBar::QSUiWaveformSeekBar(QWidget *parent) : QWidget(parent)
{
    m_core = SoundCore::instance();
    connect(m_core, SIGNAL(stateChanged(Qmmp::State)), SLOT(onStateChanged(Qmmp::State)));
    connect(m_core, SIGNAL(elapsedChanged(qint64)), SLOT(onElapsedChanged(qint64)));
    createMenu();
    readSettings();
}

QSize QSUiWaveformSeekBar::sizeHint() const
{
    return QSize(200, 100);
}

void QSUiWaveformSeekBar::readSettings()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Simple");
    m_bgColor.setNamedColor(settings.value("wfsb_bg_color", "Black").toString());
    m_rmsColor.setNamedColor(settings.value("wfsb_rms_color", "#DDDDDD").toString());
    m_waveFormColor.setNamedColor(settings.value("wfsb_waveform_color", "#BECBFF").toString());
    m_progressBar.setNamedColor(settings.value("wfsb_progressbar_color", "#9633CA10").toString());
    if(!m_update)
    {
        m_update = true;
        m_showTwoChannelsAction->setChecked(settings.value("wfsb_show_two_channels", true).toBool());
        m_showRmsAction->setChecked(settings.value("wfsb_show_rms", true).toBool());
    }
    settings.endGroup();
    drawWaveform();
}


void QSUiWaveformSeekBar::onStateChanged(Qmmp::State state)
{
    switch (state)
    {
    case Qmmp::Playing:
    {
        if(!m_scanner && isVisible())
        {
            m_scanner = new QSUiWaveformScanner(this);
            connect(m_scanner, SIGNAL(finished()), SLOT(onScanFinished()));
            connect(m_scanner, SIGNAL(dataChanged()), SLOT(onDataChanged()));
        }
        if(m_scanner)
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
        drawWaveform();
    }
        break;
    default:
        break;
    }
}

void QSUiWaveformSeekBar::onScanFinished()
{
    if(!m_scanner)
        return;

    m_data = m_scanner->data();
    m_channels = m_scanner->audioParameters().channels();
    delete m_scanner;
    m_scanner = nullptr;
    drawWaveform();
}

void QSUiWaveformSeekBar::onDataChanged()
{
    if(!m_scanner || !m_scanner->isRunning())
        return;

    m_data = m_scanner->data();
    m_channels = m_scanner->audioParameters().channels();
    drawWaveform();
}

void QSUiWaveformSeekBar::onElapsedChanged(qint64 elapsed)
{
    m_elapsed = elapsed;
    m_duration = m_core->duration();
    if(isVisible())
        update();
}

void QSUiWaveformSeekBar::writeSettings()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Simple");
    settings.setValue("wfsb_show_two_channels", m_showTwoChannelsAction->isChecked());
    settings.setValue("wfsb_show_rms", m_showRmsAction->isChecked());
    settings.endGroup();
    drawWaveform();
}

void QSUiWaveformSeekBar::paintEvent(QPaintEvent *e)
{
    QPainter painter (this);
    painter.fillRect(e->rect(), m_bgColor);

    if(!m_pixmap.isNull())
        painter.drawPixmap(0, 0, width(), height(), m_pixmap);

    if(m_duration > 0)
    {
        int x = m_pressedPos >= 0 ? m_pressedPos : (width() * m_elapsed / m_duration);
        QColor color = m_progressBar;
        QBrush brush(color);
        painter.fillRect(0, 0, x, height(), brush);
        color.setAlpha(255);
        painter.setPen(color);
        painter.drawLine(x, 0, x, height());
    }
}

void QSUiWaveformSeekBar::resizeEvent(QResizeEvent *)
{
    drawWaveform();
}

void QSUiWaveformSeekBar::showEvent(QShowEvent *)
{
    if(m_data.isEmpty() && (m_core->state() == Qmmp::Playing || m_core->state() == Qmmp::Paused))
        onStateChanged(Qmmp::Playing); //force redraw
}

void QSUiWaveformSeekBar::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        m_pressedPos = e->pos().x();
        update();
    }
    else if(e->button() == Qt::RightButton)
        m_menu->exec(e->globalPos());
}

void QSUiWaveformSeekBar::mouseReleaseEvent(QMouseEvent *)
{
    if(m_pressedPos >= 0)
    {
        if(m_duration > 0)
            m_core->seek(m_pressedPos * m_duration / width());
        m_pressedPos = -1;
    }
}

void QSUiWaveformSeekBar::mouseMoveEvent(QMouseEvent *e)
{
    if(m_pressedPos >= 0)
    {
        m_pressedPos = qBound(0, e->pos().x(), width());
        QToolTip::showText(mapToGlobal(e->pos()),
                           MetaDataFormatter::formatDuration(m_pressedPos * m_duration / width()),
                           this, QRect());
        update();
    }
}

void QSUiWaveformSeekBar::drawWaveform()
{
    if(m_data.isEmpty())
    {
        m_pixmap = QPixmap();
        update();
        return;
    }

    bool showTwoChannels = m_showTwoChannelsAction->isChecked();
    bool showRms = m_showRmsAction->isChecked();

    m_pixmap = QPixmap(width(), height());
    m_pixmap.fill(m_bgColor);

    float step = float(width()) / NUMBER_OF_VALUES;

    QPainter painter(&m_pixmap);
    painter.setPen(m_waveFormColor);
    painter.setBrush(m_waveFormColor);

    for(int i = 0; i < m_data.size() - m_channels * 3; i+=3)
    {
        int ch = (i / 3) % m_channels;
        float x1 = step * (i / m_channels / 3);
        float x2 = step * (i / m_channels / 3 + 1);
        bool draw = false;
        float zeroPos = 0, ratio = 0;
        if(ch == 0 && (m_channels == 1 || !showTwoChannels))
        {
            zeroPos = height() / 2;
            ratio = float(height() / 4) / 1000;
            draw = true;
        }
        else if(ch == 0 || (ch == 1 && showTwoChannels))
        {
            zeroPos = ((ch == 0) ? 1 : 3) * height() / 4;
            ratio = float(height() / 8) / 1000;
            draw = true;
        }

        if(draw)
        {
            float y1 = zeroPos - m_data[i] * ratio;
            float y2 = zeroPos - m_data[i + 1] * ratio;
            float y3 = zeroPos - m_data[i + m_channels * 3] * ratio;
            float y4 = zeroPos - m_data[i + m_channels * 3 + 1] * ratio;

            QPointF points[4] = {
                { x1, y1 },
                { x1, y2 },
                { x2, y4 },
                { x2, y3 }
            };

            painter.drawPolygon(points, 4);
        }
    }

    if(!showRms)
    {
        update();
        return;
    }

    painter.setPen(m_rmsColor);
    painter.setBrush(m_rmsColor);

    for(int i = 0; i < m_data.size() - m_channels * 3; i+=3)
    {
        int ch = (i / 3) % m_channels;
        float x1 = step * (i / m_channels / 3);
        float x2 = step * (i / m_channels / 3 + 1);
        bool draw = false;
        float zeroPos = 0, ratio = 0;
        if(ch == 0 && (m_channels == 1 || !showTwoChannels))
        {
            zeroPos = height() / 2;
            ratio = float(height() / 4) / 1000;
            draw = true;
        }
        else if(ch == 0 || (ch == 1 && showTwoChannels))
        {
            zeroPos = ((ch == 0) ? 1 : 3) * height() / 4;
            ratio = float(height() / 8) / 1000;
            draw = true;
        }

        if(draw)
        {
            float y1 = zeroPos + m_data[i + 2] * ratio;
            float y2 = zeroPos - m_data[i + 2] * ratio;
            float y3 = zeroPos + m_data[i + m_channels * 3 + 2] * ratio;
            float y4 = zeroPos - m_data[i + m_channels * 3 + 2] * ratio;

            QPointF points[4] = {
                { x1, y1 },
                { x1, y2 },
                { x2, y4 },
                { x2, y3 }
            };

            painter.drawPolygon(points, 4);
        }
    }
    update();
}

void QSUiWaveformSeekBar::createMenu()
{
    m_menu = new QMenu(this);
    m_showTwoChannelsAction = m_menu->addAction(tr("2 Channels"), this, SLOT(writeSettings()));
    m_showTwoChannelsAction->setCheckable(true);
    //: Root mean square
    m_showRmsAction = m_menu->addAction(tr("RMS"), this, SLOT(writeSettings()));
    m_showRmsAction->setCheckable(true);
}

QSUiWaveformScanner::QSUiWaveformScanner(QObject *parent) : QThread(parent)
{}

QSUiWaveformScanner::~QSUiWaveformScanner()
{
    stop();
}

bool QSUiWaveformScanner::scan(const QString &path)
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

void QSUiWaveformScanner::stop()
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

const QList<int> &QSUiWaveformScanner::data() const
{
    QMutexLocker locker(&m_mutex);
    return m_data;
}

const AudioParameters &QSUiWaveformScanner::audioParameters() const
{
    return m_ap;
}

void QSUiWaveformScanner::run()
{
    m_ap = m_decoder->audioParameters();
    unsigned char tmp[QMMP_BLOCK_FRAMES * m_ap.frameSize() * 4];
    float out[QMMP_BLOCK_FRAMES * m_ap.channels() * sizeof(float)];
    AudioConverter converter;
    converter.configure(m_ap.format());
    m_data.clear();

    qint64 frames = m_decoder->totalTime() * m_ap.sampleRate() / 1000;
    int samplesPerValue = frames / NUMBER_OF_VALUES * m_ap.channels();

    m_mutex.lock();
    float *max = new float[m_ap.channels()]{ -1.0 };
    float *min = new float[m_ap.channels()]{ 1.0 };
    float *rms = new float[m_ap.channels()]{ 0 };
    int counter = 0;
    int channels = m_ap.channels();
    while (!m_user_stop)
    {
        m_mutex.unlock();
        qint64 len = m_decoder->read(tmp, sizeof(tmp));
        if(len > 0)
        {
            converter.toFloat(tmp, out, len / m_ap.sampleSize());

            for(uint sample = 0; sample < len / sizeof(float); sample++)
            {
                int ch = sample % channels;
                min[ch] = qMin(min[ch], out[sample]);
                max[ch] = qMax(max[ch], out[sample]);
                rms[ch] += (out[sample] * out[sample]);

                counter++;
                if(counter >= samplesPerValue)
                {
                    m_mutex.lock();
                    for(int ch = 0; ch < channels; ++ch)
                    {
                        m_data << max[ch] * 1000;
                        m_data << min[ch] * 1000;
                        m_data << std::sqrt(rms[ch] / (counter / channels)) * 1000;
                        max[ch] = -1.0;
                        min[ch] = 1.0;
                        rms[ch] = 0;
                    }
                    if(m_data.size() / 3 / channels % (NUMBER_OF_VALUES / 64) == 0)
                        emit dataChanged();
                    m_mutex.unlock();
                    counter = 0;
                }
            }
        }
        else
        {
            m_mutex.lock();
            break;
        }

        m_mutex.lock();
    }
    delete [] min;
    delete [] max;
    delete [] rms;
    m_mutex.unlock();
}
