/***************************************************************************
 *   Copyright (C) 2007-2021 by Ilya Kotov                                 *
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
#include <qmmp/qmmp.h>
#include "outputpulseaudio.h"
#include "outputpulseaudiofactory.h"


OutputProperties OutputPulseAudioFactory::properties() const
{
    OutputProperties properties;
    properties.name = tr("PulseAudio Plugin");
    properties.hasAbout = true;
    properties.hasSettings = false;
    properties.shortName = "pulse";
    return properties;
}

Output* OutputPulseAudioFactory::create()
{
    return new OutputPulseAudio();
}

Volume *OutputPulseAudioFactory::createVolume()
{
    return new VolumePulseAudio;
}

void OutputPulseAudioFactory::showSettings(QWidget* parent)
{
   Q_UNUSED(parent);
}

void OutputPulseAudioFactory::showAbout(QWidget *parent)
{
   QMessageBox::about (parent, tr("About PulseAudio Output Plugin"),
                        tr("Qmmp PulseAudio Output Plugin")+"\n"+
                       tr("Written by: Ilya Kotov <forkotov02@ya.ru>"));
}

QString OutputPulseAudioFactory::translation() const
{
    return QLatin1String(":/pulseaudio_plugin_");
}
