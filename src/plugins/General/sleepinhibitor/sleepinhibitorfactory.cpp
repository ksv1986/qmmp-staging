/***************************************************************************
 *   Copyright (C) 2019 by Ilya Kotov                                      *
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
#include "sleepinhibitor.h"
#include "sleepinhibitorfactory.h"

GeneralProperties SleepInhibitFactory::properties() const
{
    GeneralProperties properties;
    properties.name = tr("Sleep Mode Inhibition Plugin");
    properties.shortName = "sleepinhibitor";
    properties.hasAbout = true;
    properties.hasSettings = false;
    properties.visibilityControl = false;
    return properties;
}

QObject *SleepInhibitFactory::create(QObject *parent)
{
    return new SleepInhibitor(parent);
}

QDialog *SleepInhibitFactory::createConfigDialog(QWidget *parent)
{
    Q_UNUSED(parent);
    return nullptr;
}

void SleepInhibitFactory::showAbout(QWidget *parent)
{
    QMessageBox::about(parent, tr("About Sleep Mode Inhibit Plugin"),
                       tr("Qmmp Sleep Mode Inhibit Plugin")+"\n"+
                       tr("This plugin inhibits sleep mode while audio playback") + "\n" +
                       tr("Written by: Ilya Kotov <forkotov02@ya.ru>"));
}

QString SleepInhibitFactory::translation() const
{
    return QLatin1String(":/sleepinhibitor_plugin_");
}
