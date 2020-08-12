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

#ifndef OUTPUTQTMULTIMEDIA_H
#define OUTPUTQTMULTIMEDIA_H

#include <qmmp/output.h>
#include <QScopedPointer>
#include <QObject>

/**
    @author Ivan Ponomarev ivantrue@gmail.com
*/
class QAudioOutput;
class QIODevice;
class OutputControl;

class OutputQtMultimedia : public Output
{
public:
    OutputQtMultimedia();
    ~OutputQtMultimedia();

    virtual bool initialize(quint32, ChannelMap map, Qmmp::AudioFormat format) override;
    virtual qint64 latency() override;
    virtual qint64 writeAudio(unsigned char *data, qint64 maxSize) override;
    virtual void drain() override;
    virtual void reset() override;
    virtual void suspend() override;
    virtual void resume() override;

private:
    QAudioOutput *m_output = nullptr;
    OutputControl *m_control = nullptr;
    QIODevice *m_buffer;
    qint64 m_bytes_per_second = 0;
};

class OutputControl : public QObject
{
    Q_OBJECT

public:
    explicit OutputControl(QAudioOutput *o);

public slots:
    void suspend();
    void resume();
    void stop();

private:
    QAudioOutput *m_output;

};


#endif // OUTPUTQTMULTIMEDIA_H
