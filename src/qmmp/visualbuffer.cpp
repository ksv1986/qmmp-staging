/***************************************************************************
 *   Copyright (C) 2017 by Ilya Kotov                                      *
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

#include <string.h>
#include "visualbuffer_p.h"

static inline void stereo_from_multichannel(float *l,
                                              float *r,
                                              float *s,
                                              long cnt, int chan)
{
    if(chan == 1)
    {
        memcpy(l, s, cnt * sizeof(float));
        memcpy(r, s, cnt * sizeof(float));
        return;
    }
    while (cnt > 0)
    {
        l[0] = s[0];
        r[0] = s[1];
        s += chan;
        l++;
        r++;
        cnt--;
    }
}

VisualBuffer::VisualBuffer()
{
    consumer_pos = 0;
    insertion_pos = 0;
    m_elapsed = 0;
}

void VisualBuffer::add(float *pcm, int samples, int channels, qint64 ts, qint64 delay)
{
    insertion_pos++;
    if (insertion_pos == VISUAL_BUFFER_SIZE )
    {
        insertion_pos = 0;
    }

    VisualNode *b = &m_buffer[insertion_pos];
    stereo_from_multichannel(b->data[0], b->data[1], pcm, qMin(512, samples / channels), channels);
    b->ts = ts;
    m_elapsed = qMax(0LL, ts - delay);
    m_time.restart();
}

VisualNode *VisualBuffer::take()
{
    int steps = 0;
    int t = m_elapsed + m_time.elapsed();
    while(!m_buffer[consumer_pos].ts || ((m_buffer[consumer_pos].ts < t) && (steps++ < VISUAL_BUFFER_SIZE)))
    {
        consumer_pos++;
        if(consumer_pos == VISUAL_BUFFER_SIZE)
        {
            consumer_pos = 0;
        }
    }
    return &m_buffer[consumer_pos];
}

QMutex *VisualBuffer::mutex()
{
    return &m_mutex;
}
