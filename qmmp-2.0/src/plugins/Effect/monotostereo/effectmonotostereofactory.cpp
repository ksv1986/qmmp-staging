/***************************************************************************
 *   Copyright (C) 2018-2021 by Ilya Kotov                                 *
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
#include "effectmonotostereofactory.h"
#include "monotostereoplugin.h"

const EffectProperties EffectMonoToStereoFactory::properties() const
{
    EffectProperties properties;
    properties.name = tr("Mono to Stereo Converter Plugin");
    properties.shortName = "monotostereo";
    properties.hasSettings = false;
    properties.hasAbout = true;
    return properties;
}

Effect *EffectMonoToStereoFactory::create()
{
    return new MonoToStereoPlugin();
}

void EffectMonoToStereoFactory::showSettings(QWidget *parent)
{
   Q_UNUSED(parent);
}

void EffectMonoToStereoFactory::showAbout(QWidget *parent)
{
    QMessageBox::about(parent, tr("About Mono to Stereo Converter Plugin"),
                       tr("Qmmp Mono to Stereo Converter Plugin")+"\n"+
                       tr("Written by: Ilya Kotov <forkotov02@ya.ru>"));
}

QString EffectMonoToStereoFactory::translation() const
{
    return QLatin1String(":/monotostereo_plugin_");
}
