/***************************************************************************
 *   Copyright (C) 2009 by Ilya Kotov                                      *
 *   forkotov02@hotmail.ru                                                 *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef REPLAYGAIN_H
#define REPLAYGAIN_H

#include <QtGlobal>
#include <QMap>
#include "qmmp.h"
#include "replaygainsettings.h"

/*!
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class ReplayGain
{
public:
    ReplayGain();

    void setSampleSize(int size);
    void setReplayGainSettings(const ReplayGainSettings &settings);
    void setReplayGainInfo(const QMap<Qmmp::ReplayGainKey, double> &info);
    void applyReplayGain(char *data, qint64 size);

private:
    void updateScale();
    int m_sampleSize;
    QMap<Qmmp::ReplayGainKey, double> m_info;
    ReplayGainSettings m_settings;
    double m_scale;
};

#endif // REPLAYGAIN_H
