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
#include "replaygainreader.h"
#include "decoder_ffmpeg.h"
extern "C"{
#include <libavutil/channel_layout.h>
#include <libavutil/frame.h>
}

// callbacks

static int ffmpeg_read(void *data, uint8_t *buf, int size)
{
    DecoderFFmpeg *d = (DecoderFFmpeg*)data;
#if (LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(57,84,101))
    if(d->input()->atEnd())
        return AVERROR_EOF;
#endif
    return static_cast<int>(d->input()->read((char*)buf, size));
}

static int64_t ffmpeg_seek(void *data, int64_t offset, int whence)
{
    DecoderFFmpeg *d = static_cast<DecoderFFmpeg*>(data);
    int64_t absolute_pos = 0;
    /*if(d->input()->isSequential())
        return -1;*/
    switch( whence )
    {
    case AVSEEK_SIZE:
        return d->input()->size();
    case SEEK_SET:
        absolute_pos = offset;
        break;
    case SEEK_CUR:
        absolute_pos = d->input()->pos() + offset;
        break;
    case SEEK_END:
        absolute_pos = d->input()->size() - offset;
        break;
    default:
        return -1;
    }
    if(absolute_pos < 0 || absolute_pos > d->input()->size())
        return -1;
    return d->input()->seek(absolute_pos);
}

// Decoder class
DecoderFFmpeg::DecoderFFmpeg(const QString &path, QIODevice *i) : Decoder(i),
    m_path(path)

{
    m_pkt = av_packet_alloc();
}

DecoderFFmpeg::~DecoderFFmpeg()
{
    m_bitrate = 0;
    if(m_codecContext)
        avcodec_free_context(&m_codecContext);
    if (m_formatContext)
        avformat_free_context(m_formatContext);

    av_packet_free(&m_pkt);

    if(m_stream)
        avio_context_free(&m_stream);

    if(m_frame)
        av_frame_free(&m_frame);
}

bool DecoderFFmpeg::initialize()
{
    m_bitrate = 0;
    m_totalTime = 0;
    m_seekTime = -1;
    m_eof = false;

    m_formatContext = avformat_alloc_context();

    AVProbeData pd;
    memset(&pd, 0, sizeof(pd));
    uint8_t buf[PROBE_BUFFER_SIZE + AVPROBE_PADDING_SIZE];
    pd.filename = m_path.toLocal8Bit().constData();
    pd.buf_size = input()->peek((char*)buf, sizeof(buf) - AVPROBE_PADDING_SIZE);
    pd.buf = buf;
    if(pd.buf_size < PROBE_BUFFER_SIZE)
    {
        qWarning("DecoderFFmpeg: too small buffer size: %d bytes", pd.buf_size);
        return false;
    }
    AVInputFormat *fmt = av_probe_input_format(&pd, 1);
    if(!fmt)
    {
        qWarning("DecoderFFmpeg: usupported format");
        return false;
    }
    qDebug("DecoderFFmpeg: detected format: %s", fmt->long_name);
    qDebug("=%s=", fmt->name);

#if (LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(58,10,100)) //ffmpeg-3.5
    m_input_buf = (uchar*)av_malloc(INPUT_BUFFER_SIZE + AV_INPUT_BUFFER_PADDING_SIZE);
#else
    m_input_buf = (uchar*)av_malloc(INPUT_BUFFER_SIZE + FF_INPUT_BUFFER_PADDING_SIZE);
#endif

    m_stream = avio_alloc_context(m_input_buf, INPUT_BUFFER_SIZE, 0, this, ffmpeg_read, nullptr, ffmpeg_seek);
    if(!m_stream)
    {
        av_free(m_input_buf);
        m_input_buf = nullptr;
        qWarning("DecoderFFmpeg: unable to initialize I/O callbacks");
        return false;
    }
    m_stream->seekable = !input()->isSequential();
    m_stream->max_packet_size = INPUT_BUFFER_SIZE;
    m_formatContext->pb = m_stream;

    if(avformat_open_input(&m_formatContext, nullptr, fmt, nullptr) != 0)
    {
        qDebug("DecoderFFmpeg: avformat_open_input() failed");
        return false;
    }
    avformat_find_stream_info(m_formatContext, nullptr);
    if(m_formatContext->pb)
        m_formatContext->pb->eof_reached = 0;

    if (input()->isSequential())
    {
        QMap<Qmmp::MetaData, QString> metaData;
        AVDictionaryEntry *album = av_dict_get(m_formatContext->metadata,"album",nullptr,0);
        AVDictionaryEntry *album_artist = av_dict_get(m_formatContext->metadata,"album_artist",nullptr,0);
        AVDictionaryEntry *artist = av_dict_get(m_formatContext->metadata,"artist",nullptr,0);
        AVDictionaryEntry *composer = av_dict_get(m_formatContext->metadata,"composer",nullptr,0);
        AVDictionaryEntry *comment = av_dict_get(m_formatContext->metadata,"comment",nullptr,0);
        AVDictionaryEntry *genre = av_dict_get(m_formatContext->metadata,"genre",nullptr,0);
        AVDictionaryEntry *title = av_dict_get(m_formatContext->metadata,"title",nullptr,0);
        AVDictionaryEntry *year = av_dict_get(m_formatContext->metadata,"date",nullptr,0);
        AVDictionaryEntry *track = av_dict_get(m_formatContext->metadata,"track",nullptr,0);

        if(!album)
            album = av_dict_get(m_formatContext->metadata,"WM/AlbumTitle",nullptr,0);

        if(!artist)
            artist = av_dict_get(m_formatContext->metadata,"author",nullptr,0);

        if(!year)
            year = av_dict_get(m_formatContext->metadata,"WM/Year",nullptr,0);
        if(!year)
            year = av_dict_get(m_formatContext->metadata,"year",nullptr,0);

        if(!track)
            track = av_dict_get(m_formatContext->metadata,"WM/Track",nullptr,0);
        if(!track)
            track = av_dict_get(m_formatContext->metadata,"WM/TrackNumber",nullptr,0);

        if(album)
            metaData.insert(Qmmp::ALBUM, QString::fromUtf8(album->value).trimmed());
        if(album_artist)
            metaData.insert(Qmmp::ALBUMARTIST, QString::fromUtf8(album_artist->value).trimmed());
        if(artist)
            metaData.insert(Qmmp::ARTIST, QString::fromUtf8(artist->value).trimmed());
        if(composer)
            metaData.insert(Qmmp::COMPOSER, QString::fromUtf8(composer->value).trimmed());
        if(comment)
            metaData.insert(Qmmp::COMMENT, QString::fromUtf8(comment->value).trimmed());
        if(genre)
            metaData.insert(Qmmp::GENRE, QString::fromUtf8(genre->value).trimmed());
        if(title)
            metaData.insert(Qmmp::TITLE, QString::fromUtf8(title->value).trimmed());
        if(year)
            metaData.insert(Qmmp::YEAR, year->value);
        if(track)
            metaData.insert(Qmmp::TRACK, track->value);
        addMetaData(metaData);
    }

    //replay gain
    ReplayGainReader rg(m_formatContext);
    setReplayGainInfo(rg.replayGainInfo());

    m_codecContext = avcodec_alloc_context3(nullptr);
    m_formatContext->flags |= AVFMT_FLAG_GENPTS;
    av_read_play(m_formatContext);
    m_audioIndex = av_find_best_stream(m_formatContext, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if(m_audioIndex < 0)
    {
        qWarning("DecoderFFmpeg: unable to find audio stream");
        return false;
    }

    avcodec_parameters_to_context(m_codecContext, m_formatContext->streams[m_audioIndex]->codecpar);

    if (m_codecContext->channels == 1)
    {
        m_codecContext->request_channel_layout = AV_CH_LAYOUT_MONO;
        m_channels = m_codecContext->channels;
    }
    else
    {
        m_codecContext->request_channel_layout = AV_CH_LAYOUT_STEREO;
        m_channels = 2;
    }

    av_dump_format(m_formatContext,0,nullptr,0);

    AVCodec *codec = avcodec_find_decoder(m_codecContext->codec_id);

    if (!codec)
    {
        qWarning("DecoderFFmpeg: unsupported codec for output stream");
        return false;
    }

    if (avcodec_open2(m_codecContext, codec, nullptr) < 0)
    {
        qWarning("DecoderFFmpeg: error while opening codec for output stream");
        return false;
    }

    m_frame = av_frame_alloc();
    m_totalTime = input()->isSequential() ? 0 : m_formatContext->duration * 1000 / AV_TIME_BASE;

    if(m_codecContext->codec_id == AV_CODEC_ID_SHORTEN) //ffmpeg bug workaround
        m_totalTime = 0;

    Qmmp::AudioFormat format = Qmmp::PCM_UNKNOWN;

    switch(m_codecContext->sample_fmt)
    {
    case AV_SAMPLE_FMT_U8:
    case AV_SAMPLE_FMT_U8P:
        format = Qmmp::PCM_U8;
        break;
    case AV_SAMPLE_FMT_S16:
    case AV_SAMPLE_FMT_S16P:
        format = Qmmp::PCM_S16LE;
        break;
    case AV_SAMPLE_FMT_S32:
    case AV_SAMPLE_FMT_S32P:
        format = Qmmp::PCM_S32LE;
        break;
    case AV_SAMPLE_FMT_FLT:
    case AV_SAMPLE_FMT_FLTP:
        format = Qmmp::PCM_FLOAT;
        break;
    default:
        qWarning("DecoderFFmpeg: unsupported audio format");
        return false;
    }

    setProperty(Qmmp::FORMAT_NAME, QString::fromLatin1(m_codecContext->codec->name));
    configure(m_codecContext->sample_rate, m_channels, format);

    if(m_formatContext->bit_rate)
        m_bitrate = m_formatContext->bit_rate/1000;
    if(m_codecContext->bit_rate)
        m_bitrate = m_codecContext->bit_rate/1000;
    qDebug("DecoderFFmpeg: initialize succes");
    qDebug() << "DecoderFFmpeg: total time =" << m_totalTime;

    return true;
}

qint64 DecoderFFmpeg::totalTime() const
{
    return m_totalTime;
}

int DecoderFFmpeg::bitrate() const
{
    return m_bitrate;
}

qint64 DecoderFFmpeg::read(unsigned char *audio, qint64 maxSize)
{
    m_skipBytes = 0;

    if(!m_output_size)
        fillBuffer();

    if(!m_output_size)
        return 0;

    qint64 len = qMin(m_output_size, maxSize);

    if(av_sample_fmt_is_planar(m_codecContext->sample_fmt) && m_channels > 1)
    {
        int bps = av_get_bytes_per_sample(m_codecContext->sample_fmt);

        for(int i = 0; i < len / bps; i++)
        {
            memcpy(audio + i * bps, m_frame->extended_data[i % m_channels] + i / m_channels * bps, bps);
        }

        m_output_size -= len;
        for(int i = 0; i < m_channels; i++)
        {
            memmove(m_frame->extended_data[i],
                    m_frame->extended_data[i] + len / m_channels, m_output_size / m_channels);
        }
    }
    else
    {
        memcpy(audio, m_frame->extended_data[0], len);
        m_output_size -= len;
        memmove(m_frame->extended_data[0], m_frame->extended_data[0] + len, m_output_size);
    }

    if(!m_output_size)
        av_frame_unref(m_frame);

    return len;
}

void DecoderFFmpeg::seek(qint64 pos)
{
    int64_t timestamp = int64_t(pos) * AV_TIME_BASE / 1000;
    if (m_formatContext->start_time != (qint64)AV_NOPTS_VALUE)
        timestamp += m_formatContext->start_time;
    m_seekTime = timestamp;
    av_seek_frame(m_formatContext, -1, timestamp, AVSEEK_FLAG_BACKWARD);
    avcodec_flush_buffers(m_codecContext);
    av_packet_unref(m_pkt);
    av_frame_unref(m_frame);
    m_output_size = 0;
    m_eof = false;
}

void DecoderFFmpeg::fillBuffer()
{
    while(!m_output_size || m_skipBytes > 0)
    {
        if(!m_pkt->size && !m_eof)
        {
            int read_error = av_read_frame(m_formatContext, m_pkt);
            if(!read_error)
            {
                if(m_pkt->stream_index != m_audioIndex)
                {
                    av_packet_unref(m_pkt);
                    m_pkt->size = 0;
                    continue;
                }

                if(m_seekTime > 0 && m_codecContext->codec_id == AV_CODEC_ID_APE)
                {
                    int64_t rescaledPts = av_rescale(m_pkt->pts,
                                                     AV_TIME_BASE * (int64_t) m_formatContext->streams[m_audioIndex]->time_base.num,
                                                     m_formatContext->streams[m_pkt->stream_index]->time_base.den);
                    m_skipBytes = (m_seekTime - rescaledPts) * m_codecContext->sample_rate / AV_TIME_BASE * audioParameters().frameSize();
                }
                m_seekTime = -1;
            }
            else
            {
                if(read_error != AVERROR_EOF)
                {
                    char errbuf[AV_ERROR_MAX_STRING_SIZE] = { 0 };
                    av_strerror(read_error, errbuf, sizeof(errbuf));
                    qWarning("DecoderFFmpeg: av_read_frame error: %s", errbuf);
                }
                m_eof = true;
            }
        }

        int send_error = 0;

        if(m_pkt->size > 0)
        {
            if(m_pkt->duration > 0 && m_codecContext->codec_id == AV_CODEC_ID_OPUS)
            {
                AVStream *st = m_formatContext->streams[m_audioIndex];
                m_bitrate = double(m_pkt->size) / (av_q2d(st->time_base) * m_pkt->duration) * 8.0 / 1000;
            }

            if(!(send_error = avcodec_send_packet(m_codecContext, m_pkt)))
            {
                av_packet_unref(m_pkt);
                m_pkt->size = 0;
            }
        }

        int recv_error = avcodec_receive_frame(m_codecContext, m_frame);

        if((m_eof || send_error < 0) && recv_error < 0)
        {
            if(!m_eof)
            {
                char errbuf[AV_ERROR_MAX_STRING_SIZE] = { 0 };
                av_strerror(send_error, errbuf, sizeof(errbuf));
                qWarning("DecoderFFmpeg: avcodec_send_packet error: %s", errbuf);
                av_strerror(recv_error, errbuf, sizeof(errbuf));
                qWarning("DecoderFFmpeg: avcodec_receive_frame error: %s", errbuf);
            }
            else
            {
                qDebug("DecoderFFmpeg: finished");
            }
            return;
        }

        if(!recv_error)
        {
            m_output_size = av_samples_get_buffer_size(nullptr,
                                                     m_codecContext->channels,
                                                     m_frame->nb_samples,
                                                     m_codecContext->sample_fmt, 1);
            if(m_codecContext->bit_rate)
                m_bitrate = m_codecContext->bit_rate / 1000;

            //hack for APE
            if(m_skipBytes > 0 && m_output_size > 0)
            {
                qint64 len = qMin(m_output_size, m_skipBytes);
                m_skipBytes -= len;
                m_output_size -= len;

                if(!m_output_size)
                {
                    av_frame_unref(m_frame);
                }
                else if(m_output_size > 0)
                {
                    if(av_sample_fmt_is_planar(m_codecContext->sample_fmt) && m_channels > 1)
                    {
                        for(int i = 0; i < m_channels; i++)
                        {
                            memmove(m_frame->extended_data[i],
                                    m_frame->extended_data[i] + len / m_channels, m_output_size / m_channels);
                        }
                    }
                    else
                    {
                        memmove(m_frame->extended_data[0], m_frame->extended_data[0] + len, m_output_size);
                    }
                }
            }
        }
    }
}
