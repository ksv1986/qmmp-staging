/***************************************************************************
 *   Copyright (C) 2016 by Ilya Kotov                                      *
 *   forkotov02@hotmail.ru                                                 *
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

#ifndef OUTPUTWASAPI_H
#define OUTPUTWASAPI_H

#include <stdio.h>
#include <windows.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <qmmp/volume.h>
#include <qmmp/output.h>

//class VolumeDirectSound;

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class OutputWASAPI : public Output
{
public:
    OutputWASAPI();
    ~OutputWASAPI();

    bool initialize(quint32, ChannelMap map,  Qmmp::AudioFormat format);

    //output api
    qint64 latency();
    qint64 writeAudio(unsigned char *data, qint64 size);
    void drain();
    void suspend();
    void resume();
    void reset();

    //volume control
    static OutputWASAPI *instance;
    //static VolumeDirectSound *volumeControl;
    //IDirectSoundBuffer8 *secondaryBuffer();


private:
    // helper functions
    void status();
    void uninitialize();
    DWORD bytesToWrite();

    IMMDeviceEnumerator *m_pEnumerator;
    IMMDevice *m_pDevice;
    IAudioClient *m_pAudioClient;
    IAudioRenderClient *m_pRenderClient;

    //IDirectSound8 *m_ds;
    //IDirectSoundBuffer *m_primaryBuffer;
    //IDirectSoundBuffer8 *m_dsBuffer;
    DWORD m_dsBufferAt;
    UINT32 m_bufferSize;

    typedef struct
    {
        Qmmp::ChannelPosition pos;
        DWORD chan_mask;
    } DSoundChannels;

    static DSoundChannels m_dsound_pos[10];
};

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
/*class VolumeDirectSound : public Volume
{
public:
    VolumeDirectSound();
    ~VolumeDirectSound();

    void setVolume(const VolumeSettings &vol);
    VolumeSettings volume() const;
    void restore();

private:
    VolumeSettings m_volume;
};*/


#endif // OUTPUTWASAPI_H
