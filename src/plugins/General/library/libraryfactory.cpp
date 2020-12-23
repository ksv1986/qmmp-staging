/***************************************************************************
 *   Copyright (C) 2020-2021 by Ilya Kotov                                 *
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
#include "library.h"
#include "settingsdialog.h"
#include "librarywidget.h"
#include "libraryfactory.h"

GeneralProperties LibraryFactory::properties() const
{
    GeneralProperties properties;
    properties.name = tr("Media Library Plugin");
    properties.shortName = "library";
    properties.hasAbout = true;
    properties.hasSettings = true;
    properties.visibilityControl = false;
    properties.widgets = { { LIBRARY_WIDGET, tr("Media Library"), Qt::RightDockWidgetArea,
                             Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea } };
    return properties;
}

QObject *LibraryFactory::create(QObject *parent)
{
    return new Library(parent);
}

QWidget *LibraryFactory::createWidget(int id, QWidget *parent)
{
    if(id == LIBRARY_WIDGET)
    {
        m_libraryWidget = new LibraryWidget(false, parent);
        return m_libraryWidget;
    }
    return nullptr;
}

QDialog *LibraryFactory::createConfigDialog(QWidget *parent)
{
    return new SettingsDialog(parent);
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
