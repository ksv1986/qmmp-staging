/***************************************************************************
 *   Copyright (C) 2017-2022 by Ilya Kotov                                 *
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

#include <QFile>
#include <QSettings>
#include <QRandomGenerator>
#include <stdlib.h>
#include <qmmp/qmmp.h>
#include "shoutoutput.h"

ShoutOutput::ShoutOutput(ShoutClient *m) : m_client(m)
{}

ShoutOutput::~ShoutOutput()
{
    m_client->closeLater();
    ogg_stream_clear(&m_os);
    vorbis_block_clear(&m_vb);
    vorbis_dsp_clear(&m_vd);
    vorbis_comment_clear(&m_vc);
    vorbis_info_clear(&m_vi);
    if(m_soxr)
    {
        soxr_delete(m_soxr);
        m_soxr = nullptr;
    }
    if(m_soxr_buf)
    {
        free(m_soxr_buf);
        m_soxr_buf = nullptr;
    }
}

bool ShoutOutput::initialize(quint32 freq, ChannelMap map, Qmmp::AudioFormat)
{
    QSettings settings;
    float quality = settings.value("Shout/vorbis_quality", 0.8).toFloat();
    quint32 outFreq = settings.value("Shout/sample_rate", 44100).toInt();

    if(freq != outFreq)
    {
        m_soxr = soxr_create(freq, outFreq, map.size(), nullptr, nullptr, nullptr, nullptr);
        m_ratio = (double)outFreq/freq;
    }

    vorbis_info_init(&m_vi);
    vorbis_encode_init_vbr(&m_vi,2,outFreq, quality);

    vorbis_comment_init(&m_vc);

    vorbis_analysis_init(&m_vd, &m_vi);
    vorbis_block_init(&m_vd,&m_vb);

    ogg_stream_init(&m_os, QRandomGenerator::system()->generate());

    configure(freq, map, Qmmp::PCM_FLOAT);
    return m_client->open();
}

qint64 ShoutOutput::latency()
{
    return m_client->latency();
}

qint64 ShoutOutput::writeAudio(unsigned char *data, qint64 maxSize)
{
    int chan = channels();
    int frames = maxSize / chan / sizeof(float);
    float *input_data = nullptr;

    if(m_soxr)
    {
        size_t required_frames = double(frames) * m_ratio * 2 + 2;
        if(required_frames > m_soxr_buf_frames)
        {
            m_soxr_buf_frames = required_frames;
            float *tmp = m_soxr_buf;
            m_soxr_buf = (float *)realloc(m_soxr_buf, m_soxr_buf_frames * sizeof(float) * chan);
            if(!m_soxr_buf)
            {
                qWarning("ShoutOutput: unable to allocate %zu bytes", m_soxr_buf_frames * sizeof(float) * chan);
                m_soxr_buf_frames = 0;
                if(tmp)
                    free(tmp);
                return -1;
            }
        }

        size_t done = 0;
        soxr_process(m_soxr, data, frames, nullptr,  m_soxr_buf, m_soxr_buf_frames, &done);
        input_data = m_soxr_buf;
        if(done == 0) //soxr requires more data
            return maxSize;
        frames = done;
    }
    else
    {
        input_data = (float *)data;
    }

    float **buffer = vorbis_analysis_buffer(&m_vd, frames);

    if(chan == 1)
    {
        memcpy(buffer[0], input_data, frames * sizeof(float));
        memcpy(buffer[1], input_data, frames * sizeof(float));
    }
    else
    {
        for(int i = 0; i < frames; i++)
        {
            buffer[0][i] = input_data[i * chan];
            buffer[1][i] = input_data[i * chan + 1];
        }
    }

    vorbis_analysis_wrote(&m_vd, frames);

    bool ok = true;

    while(ok && vorbis_analysis_blockout(&m_vd, &m_vb) == 1)
    {
        // analysis, assume we want to use bitrate management
        vorbis_analysis(&m_vb, nullptr);
        vorbis_bitrate_addblock(&m_vb);

        while(ok && vorbis_bitrate_flushpacket(&m_vd, &m_op))
        {
            // weld the packet into the bitstream
            ogg_stream_packetin(&m_os, &m_op);
            // write out pages (if any)
            bool eos = false;
            while(!eos)
            {
                if(ogg_stream_pageout(&m_os, &m_og))
                {

                    if(!m_client->send(m_og.header, m_og.header_len))
                    {
                        ok = false;
                        break;
                    }

                    if(!m_client->send(m_og.body, m_og.body_len))
                    {
                        ok = false;
                        break;
                    }

                    eos = (ogg_page_eos(&m_og) != 0);
                }
                else
                {
                    eos = true;
                }
            }
        }
    }

    if(!ok)
    {
        qWarning("ShoutOutput: trying to reconnect...");
        m_client->close();
        if(!m_client->open())
            return -1;

        ogg_stream_reset(&m_os);
        ogg_stream_init(&m_os, QRandomGenerator::system()->generate());

        sendHeader();
    }

    return maxSize;
}

void ShoutOutput::drain()
{
    vorbis_analysis_wrote(&m_vd,0);
}

void ShoutOutput::reset()
{}

void ShoutOutput::setTrackInfo(const TrackInfo &info)
{
    vorbis_comment_clear(&m_vc);

    static const struct
    {
        Qmmp::MetaData key;
        const char *tag;
    } tag_map[]  = {
        { Qmmp::TITLE, "title"},
        { Qmmp::ARTIST, "artist"},
        { Qmmp::ALBUM, "album"},
        { Qmmp::COMMENT, "comment"},
        { Qmmp::GENRE, "genre"},
        { Qmmp::TRACK, "tracknumber"},
        { Qmmp::YEAR, "date"},
        { Qmmp::COMPOSER, "composer"},
        { Qmmp::DISCNUMBER, "discnumber"},
        { Qmmp::UNKNOWN, nullptr}
    };

    int i = 0;
    while(tag_map[i].key != Qmmp::UNKNOWN)
    {
        if(!info.value(tag_map[i].key).isEmpty())
            vorbis_comment_add_tag(&m_vc, tag_map[i].tag, info.value(tag_map[i].key).toUtf8().constData());
        i++;
    }
    sendHeader();
}

void ShoutOutput::sendHeader()
{
    ogg_packet header;
    ogg_packet header_comm;
    ogg_packet header_code;

    vorbis_analysis_headerout(&m_vd, &m_vc, &header, &header_comm, &header_code);
    ogg_stream_packetin(&m_os, &header); // automatically placed in its own page
    ogg_stream_packetin(&m_os, &header_comm);
    ogg_stream_packetin(&m_os, &header_code);
}
