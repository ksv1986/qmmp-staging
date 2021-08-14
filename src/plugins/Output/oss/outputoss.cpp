/***************************************************************************
 *   Copyright (C) 2007 by Uriy Zhuravlev stalkerg@gmail.com               *
 *                                                                         *
 *   Copyright (c) 2000-2001 Brad Hughes bhughes@trolltech.com             *
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

#include <QApplication>
#include <QSettings>
#include <QDir>

extern "C"
{
#ifdef HAVE_SYS_SOUNDCARD_H
#include <sys/soundcard.h>
#else
#include <soundcard.h>
#endif
}

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <qmmp/buffer.h>
#include <qmmp/visual.h>
#include "outputoss.h"

OutputOSS::OutputOSS() : Output()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_audio_device = settings.value("OSS/device","/dev/dsp").toString();
}

OutputOSS::~OutputOSS()
{
    if (m_audio_fd >= 0)
    {
        ioctl(m_audio_fd, SNDCTL_DSP_RESET, 0);
        close(m_audio_fd);
        m_audio_fd = -1;
    }
}

void OutputOSS::post()
{
    ioctl(m_audio_fd, SNDCTL_DSP_POST, 0);
}

void OutputOSS::sync()
{
    ioctl(m_audio_fd, SNDCTL_DSP_SYNC, 0);
}

bool OutputOSS::initialize(quint32 freq, ChannelMap map, Qmmp::AudioFormat format)
{
    m_audio_fd = open(m_audio_device.toLatin1().constData(), O_WRONLY);

    if (m_audio_fd < 0)
    {
        qWarning("OSSOutput: failed to open output device '%s'", qPrintable(m_audio_device));
        return false;
    }

    ioctl(m_audio_fd, SNDCTL_DSP_RESET, 0);

    int p;
    int chan = map.count();
    switch (format)
    {
    case Qmmp::PCM_S16LE:
#ifdef AFMT_S16_NE
    p = AFMT_S16_NE;
#else
    p = AFMT_S16_LE;
#endif
        break;
    case Qmmp::PCM_S8:
        p = AFMT_S8;
        break;
    default:
        qWarning("OutputOSS: unsupported audio format");
        return false;
    }

    if (ioctl(m_audio_fd, SNDCTL_DSP_SETFMT, &p) == -1)
        qWarning("OutputOSS: ioctl SNDCTL_DSP_SETFMT failed: %s",strerror(errno));


    if(ioctl(m_audio_fd, SNDCTL_DSP_CHANNELS, &chan) == -1)
        qWarning("OutputOSS: ioctl SNDCTL_DSP_CHANNELS failed: %s", strerror(errno));

    if(chan <= 2)
    {
        int param = chan - 1;
        if(ioctl(m_audio_fd, SNDCTL_DSP_STEREO, &param) == -1)
            qWarning("OutputOSS: ioctl SNDCTL_DSP_STEREO failed: %s", strerror(errno));
        chan = param + 1;
    }


    if (ioctl(m_audio_fd, SNDCTL_DSP_SPEED, &freq) < 0)
        qWarning("OutputOSS: ioctl SNDCTL_DSP_SPEED failed: %s", strerror(errno));

    ioctl(m_audio_fd, SNDCTL_DSP_RESET, 0);

    configure(freq, ChannelMap(chan), format);
    return true;
}

qint64 OutputOSS::latency()
{
    return 0;
}

qint64 OutputOSS::writeAudio(unsigned char *data, qint64 maxSize)
{
    qint64 m = write(m_audio_fd, data, maxSize);
    post();
    return m;
}

void OutputOSS::drain()
{
    ioctl(m_audio_fd, SNDCTL_DSP_SYNC, 0);
}

void OutputOSS::reset()
{
    ioctl(m_audio_fd, SNDCTL_DSP_RESET, 0);
}

/***** MIXER *****/
VolumeOSS::VolumeOSS()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_mixer_device = settings.value("OSS/mixer_device","/dev/mixer").toString();
    openMixer();

}

VolumeOSS::~VolumeOSS()
{
    if (m_mixer_fd >= 0)
    {
        close(m_mixer_fd);
        m_mixer_fd = -1;
    }
}

void VolumeOSS::setVolume(const VolumeSettings &vol)
{
    if (m_mixer_fd < 0)
        return;

    long cmd;
    int devs = 0;
    ioctl(m_mixer_fd, SOUND_MIXER_READ_DEVMASK, &devs);
    if ((devs & SOUND_MASK_PCM) && !m_master)
        cmd = SOUND_MIXER_WRITE_PCM;
    else if ((devs & SOUND_MASK_VOLUME) && m_master)
        cmd = SOUND_MIXER_WRITE_VOLUME;
    else
    {
        //close(mifd);
        return;
    }
    int v = (vol.right << 8) | vol.left;
    ioctl(m_mixer_fd, cmd, &v);
}

VolumeSettings VolumeOSS::volume() const
{
    VolumeSettings vol;
    if(m_mixer_fd < 0)
        return vol;
    int cmd;
    int v, devs = 0;
    ioctl(m_mixer_fd, SOUND_MIXER_READ_DEVMASK, &devs);
    if ((devs & SOUND_MASK_PCM) && !m_master)
        cmd = SOUND_MIXER_READ_PCM;

    else if ((devs & SOUND_MASK_VOLUME) && m_master)
        cmd = SOUND_MIXER_READ_VOLUME;
    else
        return vol;

    ioctl(m_mixer_fd, cmd, &v);
    vol.left = (v & 0xFF00) >> 8;
    vol.right = (v & 0x00FF);
    return vol;
}

void VolumeOSS::openMixer()
{
    if (m_mixer_fd >= 0)
        return;
    m_mixer_fd = open(m_mixer_device.toLatin1().constData(), O_RDWR);
    if (m_mixer_fd < 0)
    {
        qWarning("VolumeControlOSS: unable to open mixer device '%s'", qPrintable(m_mixer_device));
        return;
    }
}
