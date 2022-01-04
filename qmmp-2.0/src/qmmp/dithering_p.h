/***************************************************************************
 *  Based on madplay project                                               *
 *                                                                         *
 * Copyright (C) 2000-2004 Robert Leslie <rob@mars.org>                    *
 * Copyright (C) 2016-2020 Ilya Kotov forkotov02@ya.ru                     *
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


#ifndef DITHERING_P_H
#define DITHERING_P_H

#include "effect.h"

/** @internal
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class Dithering : public Effect
{
public:
    Dithering();

    void configure(quint32 srate, ChannelMap map) override;
    void setFormats(Qmmp::AudioFormat in, Qmmp::AudioFormat out);
    void applyEffect(Buffer *b) override;
    void setEnabled(bool enabled);

private:
    void clearHistory();
    int m_chan = 2;

    typedef struct
    {
        float error[3];
        quint32 random;

    } AudioDither;

    AudioDither m_dither[9];
    float m_lsb = 0.0f;
    bool m_required = false, m_enabled;


    quint32 prng(quint32 state);
    float audioLinearDither(float sample, AudioDither *dither);
};

#endif // DITHERING_P_H
