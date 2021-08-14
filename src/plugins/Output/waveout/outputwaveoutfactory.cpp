/***************************************************************************
 *   Copyright (C) 2009-2021 by Ilya Kotov                                 *
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

#include <QMessageBox>
#include "outputwaveout.h"
#include "outputwaveoutfactory.h"


OutputProperties OutputWaveOutFactory::properties() const
{
    OutputProperties properties;
    properties.name = tr("WaveOut Plugin");
    properties.hasAbout = true;
    properties.hasSettings = false;
    properties.shortName = "waveout";
    return properties;
}

Output* OutputWaveOutFactory::create()
{
    return new OutputWaveOut();
}

Volume *OutputWaveOutFactory::createVolume()
{
    VolumeWaveOut *vol = new VolumeWaveOut();
    if(!vol->isSupported())
    {
        qDebug("OutputWaveOutFactory: device doesn't support volume control");
        delete vol;
        return nullptr;
    }
    return vol;
}

void OutputWaveOutFactory::showSettings(QWidget* parent)
{
   Q_UNUSED(parent);
}

void OutputWaveOutFactory::showAbout(QWidget *parent)
{
   QMessageBox::about (parent, tr("About WaveOut Output Plugin"),
                        tr("Qmmp WaveOut Output Plugin")+"\n"+
                        tr("Written by: Ilya Kotov <forkotov02@ya.ru>") + "\n" +
                        tr("Based on aacDECdrop player"));
}

QString OutputWaveOutFactory::translation() const
{
    return QLatin1String(":/waveout_plugin_");
}


