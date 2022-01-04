/***************************************************************************
 *   Copyright (C) 2009-2015 by Ilya Kotov                                 *
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
#include "visualprojectmfactory.h"
#include "projectmplugin.h"

VisualProperties VisualProjectMFactory::properties() const
{
    VisualProperties properties;
    properties.name = tr("ProjectM");
    properties.shortName = "projectm";
    properties.hasSettings = false;
    properties.hasAbout = true;
    return properties;
}

Visual *VisualProjectMFactory::create(QWidget *parent)
{
    return new ProjectMPlugin(parent);
}

QDialog *VisualProjectMFactory::createConfigDialog(QWidget *parent)
{
    Q_UNUSED(parent);
    return nullptr;
}

void VisualProjectMFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About ProjectM Visual Plugin"),
                        tr("Qmmp ProjectM Visual Plugin")+"\n"+
                        tr("This plugin adds projectM visualization")+"\n"+
                        tr("Written by: Ilya Kotov <forkotov02@ya.ru>")+"\n"+
                        tr("Based on libprojectM-qt library"));
}

QString VisualProjectMFactory::translation() const
{
    return QLatin1String(":/projectm_plugin_");
}
