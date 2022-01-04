/***************************************************************************
 *   Copyright (C) 2010-2016 by Ilya Kotov                                 *
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

#include <QtGlobal>
#include "eqsettings.h"

EqSettings::EqSettings(const EqSettings &other)
{
    m_preamp = other.m_preamp;
    m_is_enabled = other.m_is_enabled;
    m_bands = other.m_bands;
    m_two_passes = other.m_two_passes;
    for(int i = 0; i < 31; ++i)
        m_gains[i] = other.m_gains[i];
}

EqSettings::EqSettings(Bands bands)
{
    m_bands = bands;
}

bool EqSettings::isEnabled() const
{
    return m_is_enabled;
}

double EqSettings::gain(int chan) const
{
    return m_gains[chan];
}

double EqSettings::preamp() const
{
    return m_preamp;
}

int EqSettings::bands() const
{
    return static_cast<int>(m_bands);
}

bool EqSettings::twoPasses() const
{
    return m_two_passes;
}

void EqSettings::setEnabled(bool enabled)
{
    m_is_enabled = enabled;
}

void EqSettings::setGain(int band, double gain)
{
    m_gains[band] = gain;
}

void EqSettings::setPreamp(double preamp)
{
    m_preamp = preamp;
}

void EqSettings::setTwoPasses(bool enabled)
{
    m_two_passes = enabled;
}

EqSettings &EqSettings::operator=(const EqSettings &s)
{
    for(int i = 0; i < m_bands; ++i)
        m_gains[i] = s.m_gains[i];
    m_preamp = s.m_preamp;
    m_is_enabled = s.m_is_enabled;
    m_bands = s.m_bands;
    m_two_passes = s.m_two_passes;
    return *this;
}

bool EqSettings::operator==(const EqSettings &s) const
{
    for(int i = 0; i < m_bands; ++i)
    {
        if(m_gains[i] != s.m_gains[i])
            return false;
    }
    return (m_preamp == s.m_preamp) && (m_is_enabled == s.m_is_enabled) && (m_bands == s.m_bands) && (m_two_passes == s.m_two_passes);
}

bool EqSettings::operator!=(const EqSettings &s) const
{
    return !operator==(s);
}
