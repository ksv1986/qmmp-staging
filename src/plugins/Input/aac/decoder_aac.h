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

#ifndef DECODER_AAC_H
#define DECODER_AAC_H

#include <neaacdec.h>
#include <qmmp/decoder.h>

struct aac_data
{
    NeAACDecHandle handle;
};

class DecoderAAC : public Decoder
{
public:
    explicit DecoderAAC(QIODevice *);
    virtual ~DecoderAAC();


    struct aac_data *data()
    {
        return m_data;
    }
      // Standard Decoder API
    bool initialize() override;
    qint64 totalTime() const override;
    int bitrate() const override;
    qint64 read(unsigned char *audio, qint64 maxSize) override;
    void seek(qint64 time) override;

private:
    struct aac_data *m_data = nullptr;
    char *m_input_buf = nullptr;
    void *m_sample_buf = nullptr;
    int m_sample_buf_at = 0;
    qint64 m_sample_buf_size = 0;
    int m_bitrate = 0;
    ulong  m_input_at = 0;
    qint64 m_totalTime = 0;
};


#endif // __decoder_aac_h
