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
#include "history.h"
#include "historysettingsdialog.h"
#include "historyfactory.h"

GeneralProperties HistoryFactory::properties() const
{
    GeneralProperties properties;
    properties.name = tr("Listening History Plugin");
    properties.shortName = "history";
    properties.hasAbout = true;
    properties.hasSettings = true;
    properties.visibilityControl = false;
    return properties;
}

QObject *HistoryFactory::create(QObject *parent)
{
    return new History(parent);
}

QDialog *HistoryFactory::createConfigDialog(QWidget *parent)
{
    return new HistorySettingsDialog(parent) ;
}

void HistoryFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About Listening History Plugin"),
                        tr("Qmmp Listening History Plugin")+"\n"+
                        tr("This plugin collects information about listened tracks")+"\n"+
                        tr("Written by: Ilya Kotov <forkotov02@ya.ru>"));
}

QString HistoryFactory::translation() const
{
    return QLatin1String(":/history_plugin_");
}
