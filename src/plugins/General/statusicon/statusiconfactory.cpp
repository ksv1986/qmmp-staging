/***************************************************************************
 *   Copyright (C) 2008-2015 by Ilya Kotov                                 *
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
#include "statusicon.h"
#include "settingsdialog.h"
#include "statusiconfactory.h"

GeneralProperties StatusIconFactory::properties() const
{
    GeneralProperties properties;
    properties.name = tr("Status Icon Plugin");
    properties.shortName = "status_icon";
    properties.hasAbout = true;
    properties.hasSettings = true;
    properties.visibilityControl = true;
    return properties;
}

QObject *StatusIconFactory::create(QObject *parent)
{
    return new StatusIcon(parent);
}

QDialog *StatusIconFactory::createConfigDialog(QWidget *parent)
{
    return new SettingsDialog(parent);
}

void StatusIconFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About Status Icon Plugin"),
                        tr("Qmmp Status Icon Plugin")+"\n"+
                        tr("Written by:") + "\n"+
                        tr("Ilya Kotov <forkotov02@ya.ru>")+"\n"+
                        tr("Artur Guzik <a.guzik88@gmail.com>"));
}

QString StatusIconFactory::translation() const
{
    return QLatin1String(":/statusicon_plugin_");
}
