/***************************************************************************
 *   Copyright (C) 2006-2018 by Ilya Kotov                                 *
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

extern "C"{
#include <pulse/error.h>
}
#include <math.h>
#include <QSettings>
#include "outputpulseaudio.h"

OutputPulseAudio *OutputPulseAudio::instance = 0;
VolumePulseAudio *OutputPulseAudio::volumeControl = 0;

OutputPulseAudio::OutputPulseAudio(): Output()
{
    m_loop = 0;
    m_ctx = 0;
    m_stream = 0;

    m_pa_channels[Qmmp::CHAN_NULL] = PA_CHANNEL_POSITION_INVALID;
    m_pa_channels[Qmmp::CHAN_FRONT_CENTER] = PA_CHANNEL_POSITION_MONO;
    m_pa_channels[Qmmp::CHAN_FRONT_LEFT] = PA_CHANNEL_POSITION_FRONT_LEFT;
    m_pa_channels[Qmmp::CHAN_FRONT_RIGHT] = PA_CHANNEL_POSITION_FRONT_RIGHT;
    m_pa_channels[Qmmp::CHAN_REAR_LEFT] = PA_CHANNEL_POSITION_REAR_LEFT;
    m_pa_channels[Qmmp::CHAN_REAR_RIGHT] = PA_CHANNEL_POSITION_REAR_RIGHT;
    m_pa_channels[Qmmp::CHAN_FRONT_CENTER] = PA_CHANNEL_POSITION_FRONT_CENTER;
    m_pa_channels[Qmmp::CHAN_LFE] = PA_CHANNEL_POSITION_LFE;
    m_pa_channels[Qmmp::CHAN_SIDE_LEFT] = PA_CHANNEL_POSITION_SIDE_LEFT;
    m_pa_channels[Qmmp::CHAN_SIDE_RIGHT] = PA_CHANNEL_POSITION_SIDE_RIGHT;
    m_pa_channels[Qmmp::CHAN_REAR_CENTER] = PA_CHANNEL_POSITION_REAR_CENTER;

    instance = this;
}

OutputPulseAudio::~OutputPulseAudio()
{
    uninitialize();
    instance = 0;
}

bool OutputPulseAudio::initialize(quint32 freq, ChannelMap map, Qmmp::AudioFormat format)
{
    if(!(m_loop = pa_mainloop_new()))
    {
        qWarning("OutputPulseAudio: unable to allocate a new main loop object");
        return false;
    }

    if(!(m_ctx = pa_context_new(pa_mainloop_get_api(m_loop), "Qmmp")))
    {
        qWarning("OutputPulseAudio: unable to instantiate a new connection context");
        return false;
    }

    if(pa_context_connect(m_ctx, 0, (pa_context_flags_t)0, 0) < 0)
    {
        qWarning("OutputPulseAudio: unable to connect the context: %s", pa_strerror(pa_context_errno(m_ctx)));
        return false;
    }

    //waiting for context connection
    pa_context_state_t context_state = PA_CONTEXT_UNCONNECTED;
    while((context_state = pa_context_get_state(m_ctx)) != PA_CONTEXT_READY)
    {
        if (context_state == PA_CONTEXT_TERMINATED || context_state == PA_CONTEXT_FAILED)
        {
            qWarning("OutputPulseAudio: unable to connect the context: %s", pa_strerror(pa_context_errno(m_ctx)));
            return false;
        }
        poll();
    }

    pa_sample_spec ss;

    switch (format)
    {
    case Qmmp::PCM_S8:
        ss.format = PA_SAMPLE_U8;
        break;
    case Qmmp::PCM_S16LE:
        ss.format = PA_SAMPLE_S16LE;
        break;
    case Qmmp::PCM_S24LE:
        ss.format = PA_SAMPLE_S24_32LE;
        break;
    case Qmmp::PCM_S32LE:
        ss.format = PA_SAMPLE_S32LE;
        break;
    default:
        ss.format = PA_SAMPLE_S16LE;
    }

    ss.channels = map.count();
    ss.rate = freq;

    pa_channel_map pa_map;
    pa_map.channels = map.count();
    for(int i = 0; i < map.count(); i++)
        pa_map.map[i] = m_pa_channels[map.value(i)];

    if(!(m_stream = pa_stream_new(m_ctx, "Qmmp", &ss, &pa_map)))
    {
        qWarning("OutputPulseAudio: unable to create stream: %s", pa_strerror(pa_context_errno(m_ctx)));
        return false;
    }

    pa_buffer_attr attr;
    attr.maxlength = (uint32_t) -1;
    attr.tlength = (uint32_t) pa_usec_to_bytes((pa_usec_t)500 * 1000, &ss); //500 ms
    attr.prebuf = (uint32_t) -1;
    attr.minreq = (uint32_t) -1;
    attr.fragsize = attr.tlength;

    pa_stream_flags_t flags = pa_stream_flags_t(PA_STREAM_INTERPOLATE_TIMING | PA_STREAM_AUTO_TIMING_UPDATE);
    pa_cvolume *pvol = 0;
    pa_cvolume vol;
    if(volumeControl)
    {
        vol = VolumePulseAudio::volumeSettingsToCvolume(volumeControl->volume(), map.count());
        pvol = &vol;
    }
    if(pa_stream_connect_playback(m_stream, 0, &attr, flags, pvol, 0) < 0)
    {
        qWarning("OutputPulseAudio: unable to connect playback: %s", pa_strerror(pa_context_errno(m_ctx)));
        return false;
    }
    //waiting for stream connection
    pa_stream_state_t stream_state = PA_STREAM_UNCONNECTED;
    while((stream_state = pa_stream_get_state(m_stream)) != PA_STREAM_READY)
    {
        if (stream_state == PA_STREAM_FAILED || stream_state == PA_STREAM_TERMINATED)
        {
            qWarning("OutputPulseAudio: unable to connect playback: %s", pa_strerror(pa_context_errno(m_ctx)));
            return false;
        }
        poll();
    }

    pa_context_set_subscribe_callback(m_ctx, OutputPulseAudio::subscribe_cb, this);

    bool success = false;
    pa_operation *op = pa_context_subscribe(m_ctx, PA_SUBSCRIPTION_MASK_SINK_INPUT, OutputPulseAudio::context_success_cb, &success);
    if(!op)
    {
        qWarning("OutputPulseAudio: pa_context_subscribe failed: %s", pa_strerror(pa_context_errno(m_ctx)));
        return false;
    }

    pa_operation_state_t op_state;
    while((op_state = pa_operation_get_state(op)) != PA_OPERATION_DONE && isReady())
    {
       poll();
    }
    pa_operation_unref(op);

    if(op_state != PA_OPERATION_DONE || !success)
    {
        qWarning("OutputPulseAudio: pa_context_subscribe failed: %s", pa_strerror(pa_context_errno(m_ctx)));
        return false;
    }
    success = false;
    if(!(op = pa_context_get_sink_input_info(m_ctx, pa_stream_get_index(m_stream), OutputPulseAudio::info_cb, &success)))
    {
        qWarning("OutputPulseAudio: pa_context_get_sink_input_info failed: %s", pa_strerror(pa_context_errno(m_ctx)));
        return false;
    }

    while((op_state = pa_operation_get_state(op)) != PA_OPERATION_DONE && isReady())
    {
        poll();
    }
    pa_operation_unref(op);

    if(op_state != PA_OPERATION_DONE || !success)
    {
        qWarning("OutputPulseAudio: pa_context_get_sink_input_info failed: %s", pa_strerror(pa_context_errno(m_ctx)));
        return false;
    }

    Output::configure(freq, map, format);
    return true;
}

qint64 OutputPulseAudio::latency()
{
    pa_usec_t usec;
    int negative;

    int r = pa_stream_get_latency(m_stream, &usec, &negative);
    return (r == PA_OK && negative == 0) ? (usec / 1000) : 0;
}

qint64 OutputPulseAudio::writeAudio(unsigned char *data, qint64 maxSize)
{
    while(!pa_stream_writable_size(m_stream) || !isReady())
    {
        poll();
    }

    size_t length = qMin(size_t(maxSize), pa_stream_writable_size(m_stream));
    if(pa_stream_write(m_stream, data, length, 0, 0, PA_SEEK_RELATIVE) < 0)
    {
        qWarning("OutputPulseAudio: pa_stream_write failed: %s", pa_strerror(pa_context_errno(m_ctx)));
        return -1;
    }
    return qint64(length);
}

void OutputPulseAudio::drain()
{
    pa_operation *op = pa_stream_drain(m_stream, OutputPulseAudio::stream_success_cb, 0);

    while(pa_operation_get_state(op) != PA_OPERATION_DONE && isReady())
    {
        poll();
    }
    pa_operation_unref(op);
}

void OutputPulseAudio::reset()
{
    pa_operation *op = pa_stream_flush(m_stream, OutputPulseAudio::stream_success_cb, 0);

    while(pa_operation_get_state(op) != PA_OPERATION_DONE && isReady())
    {
       poll();
    }
    pa_operation_unref(op);
}

void OutputPulseAudio::setVolume(const VolumeSettings &v)
{
    pa_cvolume volume = VolumePulseAudio::volumeSettingsToCvolume(v, audioParameters().channels());
    pa_operation *op = pa_context_set_sink_input_volume(m_ctx, pa_stream_get_index(m_stream), &volume,
                                                        OutputPulseAudio::context_success_cb, 0);
    pa_operation_unref(op);
}

void OutputPulseAudio::uninitialize()
{    
    if(m_stream)
    {
        pa_stream_disconnect(m_stream);
        pa_stream_unref(m_stream);
        m_stream = 0;
    }

    if(m_ctx)
    {
        pa_context_disconnect(m_ctx);
        pa_context_unref(m_ctx);
        m_ctx = 0;
    }

    if(m_loop)
    {
        pa_mainloop_free(m_loop);
        m_loop = 0;
    }
}

bool OutputPulseAudio::isReady() const
{
    return m_ctx && m_stream &&
            pa_context_get_state(m_ctx) == PA_CONTEXT_READY &&
            pa_stream_get_state(m_stream) == PA_STREAM_READY;
}

void OutputPulseAudio::poll()
{
    pa_mainloop_prepare(m_loop, -1);
    pa_mainloop_poll(m_loop);
    pa_mainloop_dispatch(m_loop);
}
//callbacks
void OutputPulseAudio::subscribe_cb(pa_context *ctx, pa_subscription_event_type t, uint32_t index, void *data)
{
    pa_operation *op;
    OutputPulseAudio *output = (OutputPulseAudio *)data;

    if (!output || !output->m_stream || index != pa_stream_get_index(output->m_stream) ||
            (t != (PA_SUBSCRIPTION_EVENT_SINK_INPUT | PA_SUBSCRIPTION_EVENT_NEW) &&
             t != (PA_SUBSCRIPTION_EVENT_SINK_INPUT | PA_SUBSCRIPTION_EVENT_CHANGE)))
        return;

    if(!(op = pa_context_get_sink_input_info (ctx, index, OutputPulseAudio::info_cb, 0)))
    {
        qWarning("OutputPulseAudio: pa_context_get_sink_input_info failed: %s", pa_strerror(pa_context_errno(ctx)));
        return;
    }

    pa_operation_unref(op);
}

void OutputPulseAudio::info_cb(pa_context *ctx, const pa_sink_input_info *info, int, void *data)
{
    if(!info)
        return;

    if(volumeControl && pa_context_get_state(ctx) == PA_CONTEXT_READY)
        volumeControl->updateVolume(info->volume);

    if(data)
        *(bool *) data = true;
}

void OutputPulseAudio::context_success_cb(pa_context *, int success, void *data)
{
    if(data)
        *(bool *)data = success != 0;
}

void OutputPulseAudio::stream_success_cb(pa_stream *, int success, void *data)
{
    if(data)
        *(bool *)data = success != 0;
}

//volume control
VolumePulseAudio::VolumePulseAudio()
{
    OutputPulseAudio::volumeControl = this;
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_volume.left = settings.value("PulseAudio/left_volume", 100).toInt();
    m_volume.right = settings.value("PulseAudio/right_volume", 100).toInt();
}

VolumePulseAudio::~VolumePulseAudio()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("PulseAudio/left_volume", m_volume.left);
    settings.setValue("PulseAudio/right_volume", m_volume.right);
    OutputPulseAudio::volumeControl = 0;
}

void VolumePulseAudio::updateVolume(const pa_cvolume &v)
{
    m_volume = cvolumeToVolumeSettings(v);
    emit changed();
}

void VolumePulseAudio::setVolume(const VolumeSettings &vol)
{
    if(OutputPulseAudio::instance)
        OutputPulseAudio::instance->setVolume(vol);
    m_volume = vol;
}

VolumeSettings VolumePulseAudio::volume() const
{
    return m_volume;
}

bool VolumePulseAudio::hasNotifySignal() const
{
    return true;
}

VolumeSettings VolumePulseAudio::cvolumeToVolumeSettings(const pa_cvolume &v)
{
    VolumeSettings volume;
    if(v.channels == 2)
    {
        volume.left = ceilf(float(v.values[0]) * 100 / PA_VOLUME_NORM);
        volume.right = ceilf(float(v.values[1]) * 100 / PA_VOLUME_NORM);
    }
    else
    {
        volume.left = ceilf(float(pa_cvolume_avg(&v)) * 100 / PA_VOLUME_NORM);
        volume.right = volume.left;
    }
    return volume;
}

pa_cvolume VolumePulseAudio::volumeSettingsToCvolume(const VolumeSettings &v, int channels)
{
    pa_cvolume volume;

    if(channels == 2)
    {
        volume.values[0] = v.left * PA_VOLUME_NORM / 100;
        volume.values[1] = v.right * PA_VOLUME_NORM / 100;
        volume.channels = 2;
    }
    else
    {
        for(int i = 0; i < channels; ++i)
            volume.values[i] = qMax(v.left, v.right) * PA_VOLUME_NORM / 100;
        volume.channels = channels;
    }

    return volume;
}
