/***************************************************************************
 *  Based on mq3 and madplay projects                                      *
 *                                                                         *
 * Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>             *
 * Copyright (C) 2000-2004 Robert Leslie <rob@mars.org>                    *
 * Copyright (C) 2009-2020 Ilya Kotov forkotov02@ya.ru                     *
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

#include <taglib/id3v2header.h>
#include <taglib/tbytevector.h>
#include <math.h>
#include <stdio.h>
#include "tagextractor.h"
#include "decoder_mad.h"

#define XING_MAGIC (('X' << 24) | ('i' << 16) | ('n' << 8) | 'g')
#define XING_MAGIC2 (('I' << 24) | ('n' << 16) | ('f' << 8) | 'o')
#define LAME_MAGIC (('L' << 24) | ('A' << 16) | ('M' << 8) | 'E')
#define INPUT_BUFFER_SIZE (32*1024)

DecoderMAD::DecoderMAD(QIODevice *i) : Decoder(i)
{}

DecoderMAD::~DecoderMAD()
{
    deinit();
    if (m_input_buf)
    {
        qDebug("DecoderMAD: deleting input_buf");
        delete [] m_input_buf;
        m_input_buf = nullptr;
    }
}

bool DecoderMAD::initialize()
{
    m_inited = false;
    m_totalTime = 0;
    m_channels = 0;
    m_bitrate = 0;
    m_freq = 0;
    m_len = 0;
    m_input_bytes = 0;

    if (!input())
    {
        qWarning("DecoderMAD: cannot initialize.  No input.");
        return false;
    }

    if (!m_input_buf)
        m_input_buf = new char[INPUT_BUFFER_SIZE];

    if (input()->isSequential ()) //for streams only
    {
        TagExtractor extractor(input());
        if(!extractor.id3v2tag().isEmpty())
            addMetaData(extractor.id3v2tag());
    }

    mad_stream_init(&m_stream);
    mad_stream_options(&m_stream, MAD_OPTION_IGNORECRC);
    mad_frame_init(&m_frame);
    mad_synth_init(&m_synth);

    if (!findHeader())
    {
        qDebug("DecoderMAD: Can't find a valid MPEG header.");
        return false;
    }
    mad_stream_buffer(&m_stream, (unsigned char *) m_input_buf, m_input_bytes);
    m_stream.error = MAD_ERROR_BUFLEN;
    mad_frame_mute (&m_frame);
    m_stream.next_frame = nullptr;
    m_stream.sync = 0;
    ChannelMap map;
    if(m_channels == 1)
        map << Qmmp::CHAN_FRONT_LEFT;
    else
        map << Qmmp::CHAN_FRONT_LEFT << Qmmp::CHAN_FRONT_RIGHT;
    configure(m_freq, map, Qmmp::PCM_FLOAT);
    m_inited = true;
    return true;
}

void DecoderMAD::deinit()
{
    if (!m_inited)
        return;

    mad_synth_finish(&m_synth);
    mad_frame_finish(&m_frame);
    mad_stream_finish(&m_stream);

    m_inited = false;
    m_totalTime = 0;
    m_channels = 0;
    m_bitrate = 0;
    m_freq = 0;
    m_len = 0;
    m_input_bytes = 0;
    m_skip_frames = 0;
    m_eof = false;
    if(m_xing.lame)
    {
        delete m_xing.lame;
        m_xing.lame = nullptr;
    }
}

bool DecoderMAD::findXingHeader(struct mad_bitptr ptr, unsigned int bitlen)
{
    if (bitlen < 64)
        return false;

    struct mad_bitptr start = ptr;
    quint32 xing_magic = mad_bit_read(&ptr, 32);
    bitlen -= 32;

    if(xing_magic != XING_MAGIC && xing_magic != XING_MAGIC2)
    {
        /*
         * Due to an unfortunate historical accident, a Xing VBR tag may be
         * misplaced in a stream with CRC protection. We check for this by
         * assuming the tag began two octets prior and the high bits of the
         * following flags field are always zero.
         */

        if(xing_magic != ((quint64(XING_MAGIC) << 16) & 0xffffffffL) && xing_magic != ((quint64(XING_MAGIC2) << 16) & 0xffffffffL))
            return false;

        ptr = start;
        mad_bit_skip(&ptr, 16);
        bitlen += 16;
    }

    m_xing.flags = mad_bit_read(&ptr, 32);
    bitlen -= 32;

    if (m_xing.flags & XING_FRAMES)
    {
        if (bitlen < 32)
            return false;

        m_xing.frames = mad_bit_read(&ptr, 32);
        bitlen -= 32;

        if(!m_xing.frames)
        {
            qDebug("DecoderMAD: invalid xing header (zero number of frames)");
            return false;
        }
    }

    if (m_xing.flags & XING_BYTES)
    {
        if (bitlen < 32)
            return false;

        m_xing.bytes = mad_bit_read(&ptr, 32);
        bitlen -= 32;

        if(!m_xing.bytes)
        {
            qDebug("DecoderMAD: invalid xing header (zero number of bytes)");
            return false;
        }
    }

    if (m_xing.flags & XING_TOC)
    {
        if (bitlen < 800)
           return false;

        for (int i = 0; i < 100; ++i)
            m_xing.toc[i] = mad_bit_read(&ptr, 8);

        bitlen -= 800;
    }

    if (m_xing.flags & XING_SCALE)
    {
        if (bitlen < 32)
            return false;

        m_xing.scale = mad_bit_read(&ptr, 32);
        bitlen -= 32;
    }

    m_xing.lame = findLameHeader(ptr, bitlen);
    return true;
}

DecoderMAD::LameHeader* DecoderMAD::findLameHeader(mad_bitptr ptr, unsigned int bitlen)
{
    if(bitlen < 272)
        return nullptr;

    if(mad_bit_read (&ptr, 32) != LAME_MAGIC)
        return nullptr;

    LameHeader header;
    mad_bit_skip (&ptr, 40); //version

    header.revision = mad_bit_read (&ptr, 4);
    if (header.revision == 15)
        return nullptr;

    mad_bit_skip(&ptr, 12); //VBR,Lowpass filter value
    header.peak = mad_bit_read(&ptr, 32) << 5; //Peak amplitude
    mad_bit_skip(&ptr, 32); //Replay Gain
    mad_bit_skip(&ptr, 16); //Encoding flags, ATH Type, bitrate
    header.start_delay = mad_bit_read (&ptr, 12); //Start delay
    header.end_padding = mad_bit_read (&ptr, 12); //End padding
    mad_bit_skip (&ptr, 8); //Misc
    header.gain = mad_bit_read (&ptr, 8); //MP3 Gain
    mad_bit_skip (&ptr, 64); //Preset and surroud info, MusicLength, Music CRC
    return new LameHeader(header);
}

bool DecoderMAD::findHeader()
{
    bool result = false;
    int count = 0;
    bool has_xing = false;
    bool is_vbr = false;
    mad_timer_t duration = mad_timer_zero;
    struct mad_header header;
    mad_header_init (&header);
    uint id3v2Size = 0;

    forever
    {
        m_input_bytes = 0;
        if (m_stream.error == MAD_ERROR_BUFLEN || !m_stream.buffer)
        {
            size_t remaining = 0;

            if (m_stream.next_frame)
            {
                remaining = m_stream.bufend - m_stream.next_frame;
                memmove (m_input_buf, m_stream.next_frame, remaining);
            }

            m_input_bytes = input()->read(m_input_buf + remaining, INPUT_BUFFER_SIZE - remaining);

            if (m_input_bytes <= 0)
                break;

            mad_stream_buffer(&m_stream, (unsigned char *) m_input_buf + remaining, m_input_bytes);
            m_stream.error = MAD_ERROR_NONE;
        }

        if (mad_header_decode(&header, &m_stream) < 0)
        {
            if(m_stream.error == MAD_ERROR_LOSTSYNC)
            {
                uint tagSize = findID3v2((uchar *)m_stream.this_frame,
                                         (ulong) (m_stream.bufend - m_stream.this_frame));
                if (tagSize > 0)
                {
                    mad_stream_skip(&m_stream, tagSize);
                    id3v2Size = tagSize;
                }
                continue;
            }
            else if (m_stream.error == MAD_ERROR_BUFLEN || MAD_RECOVERABLE(m_stream.error))
                continue;
            else
            {
                qDebug ("DecoderMAD: Can't decode header: %s", mad_stream_errorstr(&m_stream));
                break;
            }
        }
        result = true;

        if (input()->isSequential())
            break;

        count ++;
        //try to detect xing header
        if (count == 1)
        {
            m_frame.header = header;
            if (mad_frame_decode(&m_frame, &m_stream) != -1 &&
                    findXingHeader(m_stream.anc_ptr, m_stream.anc_bitlen))
            {
                is_vbr = true;

                qDebug("DecoderMAD: Xing header found");

                if (m_xing.flags & XING_FRAMES)
                {
                    has_xing = true;
                    count = m_xing.frames;

                    if(m_xing.lame)
                    {
                        qDebug("DecoderMAD: LAME header found");
                        m_skip_bytes = m_xing.lame->start_delay * sizeof(float) * MAD_NCHANNELS(&header);
                        m_play_bytes = (m_xing.frames * 1152 - m_xing.lame->start_delay - m_xing.lame->end_padding) *
                                sizeof(float) * MAD_NCHANNELS(&header);
                        qDebug("DecoderMAD: samples to skip: %d, padding: %d",
                               m_xing.lame->start_delay, m_xing.lame->end_padding);
                    }
                    break;
                }
            }
        }
        //try to detect VBR
        if (!is_vbr && !(count > 15))
        {
            if (m_bitrate && header.bitrate != m_bitrate)
            {
                qDebug ("DecoderMAD: VBR detected");
                is_vbr = true;
            }
            else
                m_bitrate = header.bitrate;
        }
        else if (!is_vbr)
        {
            qDebug ("DecoderMAD: Fixed rate detected");
            break;
        }
        mad_timer_add (&duration, header.duration);
    }

    if (!result)
        return false;

    if (!is_vbr && !input()->isSequential())
    {
        double time = ((input()->size() - id3v2Size) * 8.0) / (header.bitrate);
        double timefrac = (double)time - ((long)(time));
        mad_timer_set(&duration, (long)time, (long)(timefrac*100), 100);
    }
    else if (has_xing)
    {
        mad_timer_multiply (&header.duration, count);
        duration = header.duration;
    }

    m_totalTime = mad_timer_count(duration, MAD_UNITS_MILLISECONDS);
    qDebug ("DecoderMAD: Total time: %ld", long(m_totalTime));
    m_freq = header.samplerate;
    m_channels = MAD_NCHANNELS(&header);
    m_bitrate = header.bitrate / 1000;

    if((header.flags & MAD_FLAG_LSF_EXT) && (header.flags & MAD_FLAG_MPEG_2_5_EXT))
        setProperty(Qmmp::FORMAT_NAME, QString("MPEG-2.5 layer %1").arg(header.layer));
    else if(header.flags & MAD_FLAG_LSF_EXT)
        setProperty(Qmmp::FORMAT_NAME, QString("MPEG-2 layer %1").arg(header.layer));
    else
        setProperty(Qmmp::FORMAT_NAME, QString("MPEG-1 layer %1").arg(header.layer));

    mad_header_finish(&header);
    input()->seek(0);
    m_input_bytes = 0;
    return true;
}

qint64 DecoderMAD::totalTime() const
{
    if (!m_inited)
        return 0;
    return m_totalTime;
}

int DecoderMAD::bitrate() const
{
    return int(m_bitrate);
}

qint64 DecoderMAD::read(unsigned char *data, qint64 size)
{
    while(m_skip_bytes > 0)
    {
        if(!decodeFrame())
            return 0;

        qint64 l = madOutputFloat((float*)data, size / sizeof(float)) * sizeof(float);

        if(m_skip_bytes > l)
        {
            m_skip_bytes -= l;
            continue;
        }
        else if(m_skip_bytes < l)
        {
            l -= m_skip_bytes;
            memmove(data, data + m_skip_bytes, l);
            m_skip_bytes = 0;
            m_play_bytes -= l;
            return l;
        }
        else
        {
            m_skip_bytes = 0;
            break;
        }
    }

    if(!decodeFrame())
        return 0;

    qint64 l = madOutputFloat((float*)data, size / sizeof(float)) * sizeof(float);

    if(m_play_bytes > 0)
    {
        if(m_play_bytes > l)
            m_play_bytes -= l;
        else
        {
            l -= m_play_bytes;
            m_play_bytes = 0;
        }

    }
    return l;
}

void DecoderMAD::seek(qint64 pos)
{
    if(m_totalTime > 0)
    {
        qint64 seek_pos = qint64(pos * input()->size() / m_totalTime);
        input()->seek(seek_pos);
        mad_frame_mute(&m_frame);
        mad_synth_mute(&m_synth);
        m_stream.error = MAD_ERROR_BUFLEN;
        m_stream.sync = 0;
        m_input_bytes = 0;
        m_stream.next_frame = nullptr;
        m_skip_frames = 2;
        m_skip_bytes = 0;
        m_play_bytes = -1;
    }
}

bool DecoderMAD::fillBuffer()
{
    if (m_stream.next_frame)
    {
        m_input_bytes = &m_input_buf[m_input_bytes] - (char *) m_stream.next_frame;
        memmove(m_input_buf, m_stream.next_frame, m_input_bytes);
    }
    int len = input()->read((char *) m_input_buf + m_input_bytes, INPUT_BUFFER_SIZE - m_input_bytes);
    if (!len)
    {
        qDebug("DecoderMAD: end of file");
        return false;
    }
    else if(len < 0)
    {
        qWarning("DecoderMAD: error");
        return false;
    }
    m_input_bytes += len;
    mad_stream_buffer(&m_stream, (unsigned char *) m_input_buf, m_input_bytes);
    return true;
}

uint DecoderMAD::findID3v2(uchar *data, ulong size) //retuns ID3v2 tag size
{
    if (size < 10)
        return 0;

    if (((data[0] == 'I' && data[1] == 'D' && data[2] == '3') || //ID3v2 tag
         (data[0] == '3' && data[1] == 'D' && data[2] == 'I')) && //ID3v2 footer
            data[3] < 0xff && data[4] < 0xff && data[6] < 0x80 &&
            data[7] < 0x80 && data[8] < 0x80 && data[9] < 0x80)
    {
        TagLib::ByteVector byteVector((char *)data, size);
        TagLib::ID3v2::Header header(byteVector);
        return header.tagSize();
    }
    return 0;
}

bool DecoderMAD::decodeFrame()
{
    forever
    {
        if(((m_stream.error == MAD_ERROR_BUFLEN) || !m_stream.buffer) && !m_eof)
        {
            m_eof = !fillBuffer();
        }
        if(mad_frame_decode(&m_frame, &m_stream) < 0)
        {
            switch((int) m_stream.error)
            {
            case MAD_ERROR_LOSTSYNC:
            {
                //skip ID3v2 tag
                uint tagSize = findID3v2((uchar *)m_stream.this_frame,
                                         (ulong) (m_stream.bufend - m_stream.this_frame));
                if (tagSize > 0)
                {
                    mad_stream_skip(&m_stream, tagSize);
                    qDebug("DecoderMAD: %d bytes skipped", tagSize);
                }
                continue;
            }
            case MAD_ERROR_BUFLEN:
                if(m_eof)
                    return false;
                continue;
            case MAD_ERROR_BADCRC:
                qDebug("DecoderMAD: CRC check error");
                continue;
            default:
                if (!MAD_RECOVERABLE(m_stream.error))
                    return false;
                else
                    continue;
            }
        }
        if(m_skip_frames)
        {
            m_skip_frames--;
            continue;
        }
        mad_synth_frame(&m_synth, &m_frame);
        break;
    }
    return true;
}

qint64 DecoderMAD::madOutputFloat(float *data, qint64 samples)
{
    float *data_it = data;
    unsigned int samples_per_channel, channels;
    mad_fixed_t const *left, *right;

    samples_per_channel = m_synth.pcm.length;
    channels = m_synth.pcm.channels;
    left = m_synth.pcm.samples[0];
    right = m_synth.pcm.samples[1];
    m_bitrate = m_frame.header.bitrate / 1000;
    qint64 output_samples = 0;

    if(samples_per_channel * channels > samples)
    {
        qWarning("DecoderMad: input buffer is too small");
        samples_per_channel = samples / channels;
    }

    while (samples_per_channel--)
    {
        *data_it++ = mad_f_todouble(*left++);
        output_samples++;
        if (channels == 2)
        {
            *data_it++ = mad_f_todouble(*right++);
            output_samples++;
        }
    }
    return output_samples;
}
