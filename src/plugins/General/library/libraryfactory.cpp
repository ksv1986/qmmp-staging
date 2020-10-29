/***************************************************************************
 *   Copyright (C) 2017 by Ilya Kotov                                      *
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
#include <QtPlugin>
#include <qmmp/qmmp.h>
//#include "history.h"
//#include "historysettingsdialog.h"
#include "libraryfactory.h"

GeneralProperties LibraryFactory::properties() const
{
    GeneralProperties properties;
    properties.name = tr("Media Library Plugin");
    properties.shortName = "library";
    properties.hasAbout = true;
    properties.hasSettings = false;
    properties.visibilityControl = false;
    return properties;
}

QObject *LibraryFactory::create(QObject *parent)
{
    return nullptr;
}

QDialog *LibraryFactory::createConfigDialog(QWidget *parent)
{
    return nullptr;
}

void LibraryFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About Media Library Plugin"),
                        tr("Qmmp Media Library Plugin")+"\n"+
                        tr("This plugin represents a database to store music files tags for a fast access")+"\n"+
                        tr("Written by: Ilya Kotov <forkotov02@ya.ru>"));
}

QString LibraryFactory::translation() const
{
    return QLatin1String(":/library_plugin_");
}
