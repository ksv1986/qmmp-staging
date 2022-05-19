/***************************************************************************
 *   Copyright (C) 2006-2022 by Ilya Kotov                                 *
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
#include "eqpreset.h"

EQPreset::EQPreset()
 : QListWidgetItem()
{}

EQPreset::~EQPreset()
{}

void EQPreset::setGain(int n, double value)
{
    if(n > 9 || n < 0)
        return;
    m_bands[n] = value;
}

void EQPreset::setPreamp(double preamp)
{
    m_preamp = preamp;
}

double EQPreset::gain(int n)
{
    if(n > 9 || n < 0)
        return 0;
    return m_bands[n];
}

double EQPreset::preamp()
{
    return m_preamp;
}
