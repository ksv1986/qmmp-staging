/***************************************************************************
 *   Copyright (C) 2009-2021 by Ilya Kotov <forkotov02@ya.ru>              *
 *   Copyright (C) 2009 by Sebastian Pipping <sebastian@pipping.org>       *
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

#include <QSettings>
#include <qmmp/qmmp.h>
#include <qmmp/statehandler.h>
#include <qmmp/soundcore.h>
#include "crossfadeplugin.h"

CrossfadePlugin::CrossfadePlugin() : Effect()
{
    m_buffer = nullptr;
    m_buffer_at = 0;
    m_buffer_size = 0;
    m_core = SoundCore::instance() ;
    m_handler = StateHandler::instance();
    m_state = WAITING;
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_overlap = settings.value("Crossfade/overlap", 6000).toLongLong();
}

CrossfadePlugin::~CrossfadePlugin()
{
    if(m_buffer)
        free(m_buffer);
}

void CrossfadePlugin::applyEffect(Buffer *b)
{
    switch (m_state)
    {
    case WAITING:
        if((m_core->duration() > m_overlap + 2000)
                && (m_core->duration() - m_handler->elapsed() < m_overlap + 2000))
        {
            StateHandler::instance()->sendNextTrackRequest();
            m_state = CHECKING;
        }
        break;
    case CHECKING:
        //next source has been received and current engine will be used to play it
        if(SoundCore::instance()->nextTrackAccepted())
            m_state = PREPARING;
        break;
    case PREPARING:
        if(m_core->duration() && (m_core->duration() - m_handler->elapsed() <  m_overlap))
        {
            if(m_buffer_at + b->samples > m_buffer_size)
            {
                m_buffer_size = m_buffer_at + b->samples;
                float *tmp = m_buffer;
                m_buffer = (float *)realloc(m_buffer, m_buffer_size * sizeof(float));
                if(!m_buffer)
                {
                    qWarning("CrossfadePlugin: unable to allocate  %zu bytes", m_buffer_size);
                    m_buffer_size = 0;
                    if(tmp)
                        free(tmp);
                }
            }

            if(m_buffer)
            {
                memcpy(m_buffer + m_buffer_at, b->data, b->samples * sizeof(float));
                m_buffer_at += b->samples;
                b->samples = 0;
            }
        }
        else if(m_buffer_at > 0)
            m_state = PROCESSING;
        break;
    case PROCESSING:
        if (m_buffer_at > 0)
        {
            double volume = (double)m_buffer_at/m_buffer_size;
            size_t samples = qMin(m_buffer_at, b->samples);
            mix(b->data, m_buffer, samples, volume);
            m_buffer_at -= samples;
            memmove(m_buffer, m_buffer + samples, m_buffer_at * sizeof(float));
        }
        else
            m_state = WAITING;
        break;
    default:
        ;
    }
    return;
}

void CrossfadePlugin::configure(quint32 freq, ChannelMap map)
{
    Effect::configure(freq, map);
}

void CrossfadePlugin::mix(float *cur_buf, float *prev_buf, uint samples, double volume)
{
    for (uint i = 0; i < samples; i++)
    {
        cur_buf[i] = cur_buf[i]*(1.0 - volume) + prev_buf[i]*volume;
        cur_buf[i] = qBound(-1.0f, cur_buf[i], 1.0f);
    }
}
