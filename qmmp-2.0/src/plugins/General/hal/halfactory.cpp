/***************************************************************************
 *   Copyright (C) 2009-2012 by Ilya Kotov                                 *
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
#include "halplugin.h"
#include "halfactory.h"

GeneralProperties HalFactory::properties() const
{
    GeneralProperties properties;
    properties.name = tr("HAL Plugin");
    properties.shortName = "hal";
    properties.hasAbout = true;
    properties.hasSettings = true;
    properties.visibilityControl = false;
    return properties;
}

QObject *HalFactory::create(QObject *parent)
{
    return new HalPlugin(parent);
}

QDialog *HalFactory::createConfigDialog(QWidget *parent)
{
    return new SettingsDialog(parent);
}

void HalFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About HAL Plugin"),
                        tr("Qmmp HAL Plugin") + "\n" +
                        tr("This plugin provides removable devices detection using HAL") + "\n" +
                        tr("Written by: Ilya Kotov <forkotov02@ya.ru>") + "\n" +
                        tr("Based on Solid (KDE hardware library)"));
}

QString HalFactory::translation() const
{
    return QLatin1String(":/hal_plugin_");
}
