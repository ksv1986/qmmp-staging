/***************************************************************************
 *   Copyright (C) 2009-2021 by Ilya Kotov                                 *
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

#ifndef OUTPUTWAVEOUT_H
#define OUTPUTWAVEOUT_H

#include <stdio.h>
#include <windows.h>
#include <qmmp/volume.h>
#include <qmmp/output.h>

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class OutputWaveOut : public Output
{
public:
    OutputWaveOut();
    ~OutputWaveOut();

    bool initialize(quint32, ChannelMap map,  Qmmp::AudioFormat format) override;

    //output api
    qint64 latency() override;
    qint64 writeAudio(unsigned char *data, qint64 size) override;
    void drain() override;
    void suspend() override;
    void resume() override;
    void reset() override;

private:
    // helper functions
    void status();
    void uninitialize();
    qint64 m_totalWritten = 0;
    qint32 m_frameSize = 0;
};

class VolumeWaveOut : public Volume
{
public:
    VolumeWaveOut();
    ~VolumeWaveOut();

    void setVolume(const VolumeSettings &vol) override;
    VolumeSettings volume() const override;

    bool isSupported() const;
};


#endif // OUTPUTWAVEOUT_H
