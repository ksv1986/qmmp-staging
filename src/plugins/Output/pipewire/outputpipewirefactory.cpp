/***************************************************************************
 *   Copyright (C) 2007-2022 by Ilya Kotov                                 *
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
#include "outputpipewire.h"
#include "outputpipewirefactory.h"


OutputProperties OutputPipeWireFactory::properties() const
{
    OutputProperties properties;
    properties.name = tr("PipeWire Plugin");
    properties.hasAbout = true;
    properties.hasSettings = false;
    properties.shortName = "pipewire";
    return properties;
}

Output* OutputPipeWireFactory::create()
{
    return new OutputPipeWire();
}

Volume *OutputPipeWireFactory::createVolume()
{
    return new VolumePipeWire;
}

void OutputPipeWireFactory::showSettings(QWidget *parent)
{
   Q_UNUSED(parent);
}

void OutputPipeWireFactory::showAbout(QWidget *parent)
{
    QMessageBox::about(parent, tr("About PipeWire Output Plugin"),
                       tr("Qmmp PipeWire Output Plugin")+"\n"+
                       tr("Written by: Ilya Kotov <forkotov02@ya.ru>"));
}

QString OutputPipeWireFactory::translation() const
{
    return QLatin1String(":/pipewire_plugin_");
}
