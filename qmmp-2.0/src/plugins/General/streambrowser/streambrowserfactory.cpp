/***************************************************************************
 *   Copyright (C) 2012 by Ilya Kotov                                      *
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
#include "streambrowser.h"
#include "streambrowserfactory.h"

GeneralProperties StreamBrowserFactory::properties() const
{
    GeneralProperties properties;
    properties.name = tr("Stream Browser Plugin");
    properties.shortName = "streambrowser";
    properties.hasAbout = true;
    properties.hasSettings = false;
    properties.visibilityControl = false;
    return properties;
}

QObject *StreamBrowserFactory::create(QObject *parent)
{
    return new StreamBrowser(parent);
}

QDialog *StreamBrowserFactory::createConfigDialog(QWidget *parent)
{
    Q_UNUSED(parent);
    return nullptr;
}

void StreamBrowserFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About Stream Browser Plugin"),
                        tr("Qmmp Stream Browser Plugin")+"\n"+
                        tr("This plugin allows one to add stream from IceCast stream directory")+"\n"+
                        tr("Written by: Ilya Kotov <forkotov02@ya.ru>"));
}

QString StreamBrowserFactory::translation() const
{
    return QLatin1String(":/streambrowser_plugin_");
}
