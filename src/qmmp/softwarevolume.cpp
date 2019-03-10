/***************************************************************************
 *   Copyright (C) 2008-2019 by Ilya Kotov                                 *
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

#include <QSettings>
#include "buffer.h"
#include "qmmp.h"
#include "softwarevolume_p.h"

SoftwareVolume::SoftwareVolume()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_left = settings.value("Volume/left", 80).toInt();
    m_right = settings.value("Volume/right", 80).toInt();
    m_scaleLeft = (double)m_left/100.0;
    m_scaleRight = (double)m_right/100.0;
}

SoftwareVolume::~SoftwareVolume()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("Volume/left", m_left);
    settings.setValue("Volume/right", m_right);
}

void SoftwareVolume::setVolume(const VolumeSettings &v)
{
    m_left = v.left;
    m_right = v.right;
    m_scaleLeft = (double)m_left/100.0;
    m_scaleRight = (double)m_right/100.0;
}

VolumeSettings SoftwareVolume::volume() const
{
    VolumeSettings v;
    v.left = m_left;
    v.right = m_right;
    return v;
}

void SoftwareVolume::changeVolume(Buffer *b, int chan)
{
    if(chan == 1)
    {
        for(size_t i = 0; i < b->samples; ++i)
        {
            b->data[i] *= qMax(m_scaleLeft, m_scaleRight);
        }
    }
    else
    {
        for(size_t i = 0; i < b->samples; i+=2)
        {
            b->data[i] *= m_scaleLeft;
            b->data[i+1] *= m_scaleRight;
        }
    }
}
