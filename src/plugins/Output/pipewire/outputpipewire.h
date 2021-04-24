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

#ifndef OUTPUTPIPEWIRE_H
#define OUTPUTPIPEWIRE_H

#include <QObject>
#include <QHash>
#include <pipewire/pipewire.h>
#include <spa/param/audio/format-utils.h>
#include <atomic>
#include <qmmp/output.h>
#include <qmmp/volume.h>

class VolumePulseAudio;

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class OutputPipeWire : public Output
{
public:
    OutputPipeWire();
    ~OutputPipeWire();

    bool initialize(quint32, ChannelMap map, Qmmp::AudioFormat format) override;
    //output api
    qint64 latency() override;
    qint64 writeAudio(unsigned char *data, qint64 maxSize) override;
    void drain() override;
    void reset() override;
    void suspend() override;
    void resume() override;
    void setMuted(bool mute);

private:
    // helper functions
    void uninitialize();
    //callbacks
    static void onProcess(void *data);
    static void onCoreEventDone(void *data, uint32_t id, int seq);
    static void onRegistryEventGlobal(void *data, uint32_t id,
                                      uint32_t permissions, const char *type, uint32_t version,
                                      const struct spa_dict *props);

    static void onStateChanged (void *data, enum pw_stream_state old,
                           enum pw_stream_state state, const char *error);

    struct pw_thread_loop *m_loop = nullptr;
    struct pw_stream *m_stream  = nullptr;
    pw_context *m_context = nullptr;
    pw_core *m_core = nullptr;
    pw_registry *m_registry = nullptr;
    spa_hook m_coreListener = {};
    spa_hook m_streamListener = {};
    spa_hook m_registryListener = {};

    uint32_t m_stride = 0;
    uint32_t m_nFrames = 0;
    uint32_t m_bufferSize = 0;
    uint32_t m_writeBufferPos = 0;

    int m_coreInitSeq = 0;

    std::atomic_bool m_hasSinks {false};
    std::atomic_bool m_initDone {false};
    std::atomic_bool m_bufferHasData {false};
    std::atomic_bool m_processed {false};
    std::atomic_bool m_paused {false};
    std::atomic_bool m_silence {false};
    std::atomic_bool m_streamPaused {false};
    std::atomic_bool m_ignoreStateChange {false};
    std::atomic_bool m_err {false};


    //uint8_t m_stream_buffer[1024];
    //unsigned char m_buffer[4096];
    unsigned char *m_buffer = nullptr;
    uint32_t m_buffer_at = 0;
    QHash <Qmmp::ChannelPosition, spa_audio_channel> m_pw_channels;
};

#endif // OUTPUTPIPEWIRE_H
