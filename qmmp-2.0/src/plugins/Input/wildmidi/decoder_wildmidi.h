/***************************************************************************
 *   Copyright (C) 2008-2021 by Ilya Kotov                                 *
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

#ifndef DECODER_WILDMIDI_H
#define DECODER_WILDMIDI_H

extern "C"{
#include <wildmidi_lib.h>
}
#include <qmmp/decoder.h>

class DecoderWildMidi : public Decoder
{
public:
    explicit DecoderWildMidi(const QString &path);
    virtual ~DecoderWildMidi();

    // Standard Decoder API
    bool initialize() override;
    qint64 totalTime() const override;
    int bitrate() const override;
    qint64 read(unsigned char *data, qint64 size) override;
    void seek(qint64) override;

private:
    void *midi_ptr = nullptr;
    qint64 m_totalTime = 0;
    quint32 m_sample_rate = 0;
    QString m_path;
};

#endif // DECODER_WILDMIDI_H
