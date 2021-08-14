/***************************************************************************
 *   Copyright (C) 2010-2021 by Ilya Kotov                                 *
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

#ifndef DECODER_GME_H
#define DECODER_GME_H

#include <gme/gme.h>
#include <qmmp/decoder.h>

class GmeHelper;

/**
   @author Ilya Kotov <forkotov02@ya.ru>
*/
class DecoderGme : public Decoder
{
public:
    explicit DecoderGme(const QString &path);
    virtual ~DecoderGme();

    // Standard Decoder API
    bool initialize() override;
    qint64 totalTime() const override;
    int bitrate() const override;
    qint64 read(unsigned char *data, qint64 size) override;
    void seek(qint64) override;

private:
    GmeHelper m_helper;
    Music_Emu *m_emu = nullptr;
    qint64 m_totalTime;
    QString m_path;
};

#endif // DECODER_GME_H
