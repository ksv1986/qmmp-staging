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
#include "covermanager.h"
#include "covermanagerfactory.h"

GeneralProperties CoverManagerFactory::properties() const
{
    GeneralProperties properties;
    properties.name = tr("Cover Manager Plugin");
    properties.shortName = "cover_manager";
    properties.hasAbout = true;
    properties.hasSettings = false;
    properties.visibilityControl = false;
    return properties;
}

QObject *CoverManagerFactory::create(QObject *parent)
{
    return new CoverManager(parent);
}

QDialog *CoverManagerFactory::createConfigDialog(QWidget *)
{
    return nullptr;
}

void CoverManagerFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About Cover Manager Plugin"),
                        tr("Qmmp Cover Manager Plugin")+"\n"+
                        tr("Written by: Ilya Kotov <forkotov02@ya.ru>"));
}

QString CoverManagerFactory::translation() const
{
    return QLatin1String(":/covermanager_plugin_");
}
