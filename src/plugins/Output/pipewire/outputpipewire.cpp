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

#include <QtDebug>
#include <QSettings>
#include <QThread>
#include <QtMath>
#include "outputpipewire.h"

OutputPipeWire::OutputPipeWire(): Output()
{
   m_pw_channels = {
        { Qmmp::CHAN_NULL, SPA_AUDIO_CHANNEL_NA  },
        { Qmmp::CHAN_FRONT_CENTER, SPA_AUDIO_CHANNEL_MONO  },
        { Qmmp::CHAN_FRONT_LEFT, SPA_AUDIO_CHANNEL_FL  },
        { Qmmp::CHAN_FRONT_RIGHT, SPA_AUDIO_CHANNEL_FR  },
        { Qmmp::CHAN_REAR_LEFT, SPA_AUDIO_CHANNEL_RL  },
        { Qmmp::CHAN_REAR_RIGHT, SPA_AUDIO_CHANNEL_RR  },
        { Qmmp::CHAN_FRONT_CENTER, SPA_AUDIO_CHANNEL_FC  },
        { Qmmp::CHAN_LFE, SPA_AUDIO_CHANNEL_LFE  },
        { Qmmp::CHAN_SIDE_LEFT, SPA_AUDIO_CHANNEL_SL  },
        { Qmmp::CHAN_SIDE_RIGHT, SPA_AUDIO_CHANNEL_SR  },
        { Qmmp::CHAN_REAR_CENTER, SPA_AUDIO_CHANNEL_RC  }
    };

   pw_init(nullptr, nullptr);
}

OutputPipeWire::~OutputPipeWire()
{
    uninitialize();
}

void OutputPipeWire::onStateChanged (void *data, enum pw_stream_state old,
                       enum pw_stream_state state, const char *error)
{
    qDebug() << Q_FUNC_INFO << state << error;
    OutputPipeWire *o = static_cast<OutputPipeWire *>(data);
    pw_thread_loop_signal(o->m_loop, false);
}

bool OutputPipeWire::initialize(quint32 freq, ChannelMap map, Qmmp::AudioFormat format)
{
    static const pw_core_events coreEvents = {
        .version = PW_VERSION_CORE_EVENTS,
        .done = OutputPipeWire::onCoreEventDone
    };

    static const pw_registry_events registryEvents = {
        .version = PW_VERSION_REGISTRY_EVENTS,
        .global = OutputPipeWire::onRegistryEventGlobal
    };

    static const struct pw_stream_events streamEvents = {
        .version = PW_VERSION_STREAM_EVENTS,
        .state_changed = OutputPipeWire::onStateChanged,
        .process = OutputPipeWire::onProcess,
    };

    if(!(m_loop = pw_thread_loop_new("pipewire-main-loop", nullptr)))
    {
        qWarning("OutputPipeWire: unable to create main loop");
        return false;
    }

    if (!(m_context = pw_context_new(pw_thread_loop_get_loop(m_loop), nullptr, 0)))
    {
        qWarning("OutputPipeWire: unable to create context");
        return false;
    }

    if(!(m_core = pw_context_connect(m_context, nullptr, 0)))
    {
        qWarning("OutputPipeWire: unable to connect context");
        return false;
    }
    pw_core_add_listener(m_core, &m_coreListener, &coreEvents, this);

    if(!(m_registry = pw_core_get_registry(m_core, PW_VERSION_REGISTRY, 0)))
    {
        qWarning("OutputPipeWire: unable to get registry interface");
        return false;
    }
    pw_registry_add_listener(m_registry, &m_registryListener, &registryEvents, this);

    m_coreInitSeq = pw_core_sync(m_core, PW_ID_CORE, m_coreInitSeq);


    if (pw_thread_loop_start(m_loop) != 0)
    {
        //m_err = true;
        return false;
    }

    pw_thread_loop_lock(m_loop);
    while (!m_initDone)
    {
        if (pw_thread_loop_timed_wait(m_loop, 2) != 0)
            break;
    }
    pw_thread_loop_unlock(m_loop);

    if(!m_initDone || !m_hasSinks)
        return false;

    m_stride = AudioParameters::sampleSize(format) * map.count();
    m_nFrames = qBound(64, qCeil((2048.0 / 48000.0) * freq), 8192);
    m_bufferSize = m_nFrames * m_stride;
    m_writeBufferPos = 0;
    m_buffer = new unsigned char[m_bufferSize];

    auto props = pw_properties_new(
                PW_KEY_MEDIA_TYPE, "Audio",
                PW_KEY_MEDIA_CATEGORY, "Playback",
                PW_KEY_MEDIA_ROLE, "Music",
                PW_KEY_MEDIA_ICON_NAME, "qmmp",
                nullptr
                );
    pw_properties_setf(props, PW_KEY_NODE_LATENCY, "%u/%u", m_nFrames, freq);

    pw_thread_loop_lock(m_loop);

    m_stream = pw_stream_new(m_core, "Playback", props);
    if (!(m_stream = pw_stream_new(m_core, "Playback", props)))
    {
        qWarning("OutputPipeWire: unable to create stream");
        pw_thread_loop_unlock(m_loop);
        return false;
    }

    m_streamListener = {};
    pw_stream_add_listener(m_stream, &m_streamListener, &streamEvents, this);

    uint8_t buffer[1024];
    spa_pod_builder b = {
        .data = buffer,
        .size = sizeof(buffer)
    };

    spa_audio_info_raw info = {
        .format = SPA_AUDIO_FORMAT_F32,
        .flags = SPA_AUDIO_FLAG_NONE,
        .rate = freq,
        .channels = map.count()
    };

    switch (format)
    {
        case Qmmp::PCM_S8:
            info.format = SPA_AUDIO_FORMAT_S8;
            break;
        case Qmmp::PCM_S16LE:
            info.format = SPA_AUDIO_FORMAT_S16_LE;
            break;
        case Qmmp::PCM_S24LE:
            info.format = SPA_AUDIO_FORMAT_S24_32_LE;
            break;
        case Qmmp::PCM_S32LE:
            info.format = SPA_AUDIO_FORMAT_S32_LE;
            break;
        case Qmmp::PCM_FLOAT:
            info.format = SPA_AUDIO_FORMAT_F32_LE;
            break;
        default:
            info.format = SPA_AUDIO_FORMAT_S16_LE;
    }

    for(int i = 0; i < map.count(); i++)
        info.position[i] = m_pw_channels[map.value(i)];

    const spa_pod *params[2];
    params[0] = spa_format_audio_raw_build(
                &b,
                SPA_PARAM_EnumFormat,
                &info
                );
    /*params[1] = static_cast<spa_pod *>(spa_pod_builder_add_object(
                                                &b,
                                                SPA_TYPE_OBJECT_ParamBuffers, SPA_PARAM_Buffers,
                                                SPA_PARAM_BUFFERS_size, m_bufferSize)
                                                );*/




    qDebug() << pw_stream_connect(m_stream,
                                  PW_DIRECTION_OUTPUT,
                                  PW_ID_ANY,
                                  static_cast<pw_stream_flags>(PW_STREAM_FLAG_AUTOCONNECT |
                                                               PW_STREAM_FLAG_MAP_BUFFERS |
                                                               PW_STREAM_FLAG_RT_PROCESS),
                      params, 1);



    Output::configure(freq, map, format);
    pw_thread_loop_unlock(m_loop);
    qDebug("OutputPipeWire: ready");
    return true;
}

qint64 OutputPipeWire::latency()
{
    /*pa_usec_t usec;
    int negative;

    int r = pa_stream_get_latency(m_stream, &usec, &negative);
    return (r == PA_OK && negative == 0) ? (usec / 1000) : 0;*/
    return 0;
}

qint64 OutputPipeWire::writeAudio(unsigned char *data, qint64 maxSize)
{
    //qDebug() << Q_FUNC_INFO;
    pw_thread_loop_lock(m_loop);

    if(m_buffer_at == m_bufferSize)
    {
        if (pw_thread_loop_timed_wait(m_loop, 1) != 0)
        {
            pw_thread_loop_unlock(m_loop);
            return 0;
        }
    }

    qint64 size = qMin(qint64(m_bufferSize - m_buffer_at), maxSize);
    memcpy(m_buffer + m_buffer_at, data, size);
    m_buffer_at += size;
    pw_thread_loop_unlock(m_loop);
    return size;
}

void OutputPipeWire::drain()
{
    //pa_operation *op = pa_stream_drain(m_stream, OutputPipeWire::stream_success_cb, nullptr);
    //process(op);
}

void OutputPipeWire::reset()
{
    //pa_operation *op = pa_stream_flush(m_stream, OutputPipeWire::stream_success_cb, nullptr);
    //process(op);
}

void OutputPipeWire::suspend()
{
    //pa_operation *op = pa_stream_cork(m_stream, 1, OutputPipeWire::stream_success_cb, nullptr);
    //process(op);
}

void OutputPipeWire::resume()
{
    //pa_operation *op = pa_stream_cork(m_stream, 0, OutputPipeWire::stream_success_cb, nullptr);
    //process(op);
}

void OutputPipeWire::setMuted(bool mute)
{
    //pa_operation *op = pa_context_set_sink_input_mute(m_ctx, pa_stream_get_index(m_stream), mute,
    //                                                  OutputPipeWire::context_success_cb, nullptr);
    //pa_operation_unref(op);
}

void OutputPipeWire::uninitialize()
{
    if(m_loop)
    {
        pw_thread_loop_stop(m_loop);
        pw_thread_loop_destroy(m_loop);
        m_loop = nullptr;
    }

    if(m_stream)
    {
        pw_stream_disconnect(m_stream);
        pw_stream_destroy(m_stream);
        m_stream = nullptr;
    }

    pw_deinit();
}

//callbacks
void OutputPipeWire::onProcess(void *data)
{
    OutputPipeWire *o = static_cast<OutputPipeWire *>(data);

    if(!o->m_buffer_at)
    {
        pw_thread_loop_signal(o->m_loop, false);
        return;
    }

    struct pw_buffer *b = pw_stream_dequeue_buffer(o->m_stream);
    struct spa_buffer *buf = b->buffer;

    uint32_t size = qMin(buf->datas[0].maxsize, uint32_t(o->m_buffer_at));
    memcpy(buf->datas[0].data, o->m_buffer, size);
    o->m_buffer_at -= size;
    memmove(o->m_buffer, o->m_buffer + size, o->m_buffer_at);

    b->buffer->datas[0].chunk->offset = 0;
    b->buffer->datas[0].chunk->size = o->m_bufferSize;
    b->buffer->datas[0].chunk->stride = o->m_stride;

    pw_stream_queue_buffer(o->m_stream, b);
    pw_thread_loop_signal(o->m_loop, false);
}

void OutputPipeWire::onCoreEventDone(void *data, uint32_t id, int seq)
{
    OutputPipeWire *o = static_cast<OutputPipeWire *>(data);

    if (id == PW_ID_CORE && seq == o->m_coreInitSeq)
    {
        spa_hook_remove(&o->m_registryListener);
        spa_hook_remove(&o->m_coreListener);

        o->m_initDone = true;
        pw_thread_loop_signal(o->m_loop, false);
    }
}

void OutputPipeWire::onRegistryEventGlobal(void *data, uint32_t id, uint32_t permissions, const char *type, uint32_t version, const spa_dict *props)
{
    Q_UNUSED(id)
    Q_UNUSED(permissions)
    Q_UNUSED(version)

    OutputPipeWire *o = static_cast<OutputPipeWire *>(data);

    if (qstrcmp(type, PW_TYPE_INTERFACE_Node) != 0)
        return;

    auto media_class = spa_dict_lookup(props, PW_KEY_MEDIA_CLASS);
    if (!media_class)
        return;

    if (qstrcmp(media_class, "Audio/Sink") != 0)
        return;

    o->m_hasSinks = true;

    o->m_coreInitSeq = pw_core_sync(o->m_core, PW_ID_CORE, o->m_coreInitSeq);
}
