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

#ifndef DECODER_FLAC_H
#define DECODER_FLAC_H

#include <qmmp/decoder.h>

#include <FLAC/all.h>

#define MAX_SUPPORTED_CHANNELS 2
#define SAMPLES_PER_WRITE 512
#define SAMPLE_BUFFER_SIZE ((FLAC__MAX_BLOCK_SIZE + SAMPLES_PER_WRITE) * MAX_SUPPORTED_CHANNELS * (32/8))

class CueParser;

struct flac_data
{
    //FLAC__SeekableStreamDecoder *decoder;
    FLAC__StreamDecoder *decoder = nullptr;
    int bitrate = 0;
    int abort = 0; /* abort playing (due to an error) */

    qint64 length = 0;
    qint64 total_samples = 0;

    FLAC__byte sample_buffer[SAMPLE_BUFFER_SIZE];
    unsigned sample_buffer_fill = 0;

    /* sound parameters */
    unsigned bits_per_sample = 0;
    unsigned sample_rate = 0;
    unsigned channels = 0;

    FLAC__uint64 last_decode_position = 0;

    //counters
    qint64 read_bytes = 0;
    qint64 frame_counter = 0;

    QIODevice *input = nullptr;
};

class DecoderFLAC : public Decoder
{
public:
    explicit DecoderFLAC(const QString &path, QIODevice *i);
    virtual ~DecoderFLAC();

    // Standard Decoder API
    bool initialize() override;
    qint64 totalTime() const override;
    int bitrate() const override;
    qint64 read(unsigned char *data, qint64 maxSize) override;
    void seek(qint64 time) override;
    const QString nextURL() const override;
    void next() override;

private:

    // helper functions
    void deinit();
    uint findID3v2(char *data, ulong size); //retuns ID3v2 tag size
    ChannelMap findChannelMap(int channels);

    struct flac_data *m_data = nullptr;
    qint64 length_in_bytes = 0;
    qint64 m_totalBytes = 0;
    qint64 m_offset = 0;
    qint64 m_length = 0;
    QString m_path;
    CueParser *m_parser = nullptr;
    int m_track = 0;
    char *m_buf = nullptr; //buffer for remainig data
    qint64 m_buf_size = 0;
    qint64 m_sz = 0; //sample size
};


#endif // __decoder_flac_h
