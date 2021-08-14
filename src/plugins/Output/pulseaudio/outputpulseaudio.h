/***************************************************************************
 *   Copyright (C) 2006-2021 by Ilya Kotov                                 *
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

#ifndef OUTPUTPULSEAUDIO_H
#define OUTPUTPULSEAUDIO_H

#include <QObject>
#include <QHash>
extern "C"{
#include <pulse/pulseaudio.h>
}
#include <qmmp/output.h>
#include <qmmp/volume.h>

class VolumePulseAudio;

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class OutputPulseAudio : public Output
{
public:
    OutputPulseAudio();
    ~OutputPulseAudio();

    bool initialize(quint32, ChannelMap map, Qmmp::AudioFormat format) override;
    //output api
    qint64 latency() override;
    qint64 writeAudio(unsigned char *data, qint64 maxSize) override;
    void drain() override;
    void reset() override;
    void suspend() override;
    void resume() override;
    void setMuted(bool mute);
    void setVolume(const VolumeSettings &v);

    static OutputPulseAudio *instance;
    static VolumePulseAudio *volumeControl;

private:
    // helper functions
    void uninitialize();
    bool isReady() const;
    void poll();
    bool process(pa_operation *op);
    //callbacks
    static void subscribe_cb(pa_context *ctx, pa_subscription_event_type t, uint32_t index, void *data);
    static void info_cb(pa_context *ctx, const pa_sink_input_info * info, int, void * data);
    static void context_success_cb(pa_context *, int success, void *data);
    static void stream_success_cb(pa_stream *, int success, void *data);

    pa_mainloop *m_loop = nullptr;
    pa_context *m_ctx = nullptr;
    pa_stream *m_stream = nullptr;
    QHash <Qmmp::ChannelPosition, pa_channel_position_t> m_pa_channels;
};

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class VolumePulseAudio : public Volume
{
public:
    VolumePulseAudio();
    ~VolumePulseAudio();

    void updateVolume(const pa_cvolume &v, bool muted);
    void setVolume(const VolumeSettings &vol) override;
    VolumeSettings volume() const override;
    bool isMuted() const override;
    void setMuted(bool mute) override;
    VolumeFlags flags() const override;
    static VolumeSettings cvolumeToVolumeSettings(const pa_cvolume &v);
    static pa_cvolume volumeSettingsToCvolume(const VolumeSettings &v, int channels);

private:
    VolumeSettings m_volume;
    bool m_muted = false;
};

#endif // OUTPUTPULSEAUDIO_H
