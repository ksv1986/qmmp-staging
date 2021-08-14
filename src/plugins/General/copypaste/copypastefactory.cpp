/***************************************************************************
 *   Copyright (C) 2013 by Ilya Kotov                                      *
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
#include "copypaste.h"
#include "copypastefactory.h"

GeneralProperties CopyPasteFactory::properties() const
{
    GeneralProperties properties;
    properties.name = tr("Copy/Paste Plugin");
    properties.shortName = "copypaste";
    properties.hasAbout = true;
    properties.hasSettings = false;
    properties.visibilityControl = false;
    return properties;
}

QObject *CopyPasteFactory::create(QObject *parent)
{
    return new CopyPaste(parent);
}

QDialog *CopyPasteFactory::createConfigDialog(QWidget *)
{
    return nullptr;
}

void CopyPasteFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About Copy/Paste Plugin"),
                        tr("Qmmp Copy/Paste Plugin")+"\n"+
                        tr("This plugin allows one to copy selected tracks from one playlist to another")+"\n"+
                        tr("Written by: Ilya Kotov <forkotov02@ya.ru>"));
}

QString CopyPasteFactory::translation() const
{
    return QLatin1String(":/copypaste_plugin_");
}
