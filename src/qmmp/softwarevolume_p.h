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

#ifndef SOFTWAREVOLUME_P_H
#define SOFTWAREVOLUME_P_H

#include "volume.h"
#include "buffer.h"

/*! @internal
 * @brief The SoftwareVolume class provides access to the software volume control.
 * @author Ilya Kotov <forkotov02@ya.ru>
 */
class SoftwareVolume : public Volume
{
public:
    SoftwareVolume();
    ~SoftwareVolume();

    void setVolume(const VolumeSettings &v) override;
    VolumeSettings volume() const override;
    void changeVolume(Buffer *b, int chan);

private:
    int m_left, m_right;
    double m_scaleLeft, m_scaleRight;
};

#endif // SOFTWAREVOLUME_P_H
