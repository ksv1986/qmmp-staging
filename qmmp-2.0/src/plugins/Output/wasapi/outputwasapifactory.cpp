/***************************************************************************
 *   Copyright (C) 2016-2021 by Ilya Kotov                                 *
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
#include "settingsdialog.h"
#include "outputwasapi.h"
#include "outputwasapifactory.h"

OutputProperties OutputWASAPIFactory::properties() const
{
    OutputProperties properties;
    properties.name = tr("WASAPI Plugin");
    properties.hasAbout = true;
    properties.hasSettings = true;
    properties.shortName = "wasapi";
    return properties;
}

Output* OutputWASAPIFactory::create()
{
    return new OutputWASAPI();
}

Volume *OutputWASAPIFactory::createVolume()
{
    return new VolumeWASAPI;
}

void OutputWASAPIFactory::showSettings(QWidget* parent)
{
    SettingsDialog *s = new SettingsDialog(parent);
    s->show();
}

void OutputWASAPIFactory::showAbout(QWidget *parent)
{
   QMessageBox::about (parent, tr("About WASAPI Output Plugin"),
                        tr("Qmmp WASAPI Output Plugin")+"\n"+
                       tr("Written by: Ilya Kotov <forkotov02@ya.ru>"));
}

QString OutputWASAPIFactory::translation() const
{
    return QLatin1String(":/wasapi_plugin_");
}
