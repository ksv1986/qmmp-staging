/***************************************************************************
 *   Copyright (C) 2007-2015 by Ilya Kotov                                 *
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
#include "settingsdialog.h"
#include "outputalsa.h"
#include "outputalsafactory.h"


OutputProperties OutputALSAFactory::properties() const
{
    OutputProperties properties;
    properties.name = tr("ALSA Plugin");
    properties.shortName = "alsa";
    properties.hasAbout = true;
    properties.hasSettings = true;
    return properties;
}

Output* OutputALSAFactory::create()
{
    return new OutputALSA();
}

Volume *OutputALSAFactory::createVolume()
{
    return new VolumeALSA();
}

void OutputALSAFactory::showSettings(QWidget* parent)
{
   SettingsDialog *s = new SettingsDialog(parent);
   s -> show();
}

void OutputALSAFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About ALSA Output Plugin"),
                        tr("Qmmp ALSA Output Plugin")+"\n"+
                        tr("Written by: Ilya Kotov <forkotov02@ya.ru>"));
}

QString OutputALSAFactory::translation() const
{
    return QLatin1String(":/alsa_plugin_");
}
