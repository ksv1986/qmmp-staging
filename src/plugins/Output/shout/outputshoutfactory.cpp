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
#include <QTranslator>
#include <QtPlugin>
#include <QApplication>
#include <qmmp/qmmp.h>
#include <shout/shout.h>
#include "shoutclient.h"
#include "shoutoutput.h"
#include "settingsdialog.h"
#include "outputshoutfactory.h"

OutputShoutFactory::OutputShoutFactory()
{
    m_connection = new ShoutClient(qApp);
}

const OutputProperties OutputShoutFactory::properties() const
{
    OutputProperties properties;
    properties.name = tr("Icecast Plugin");
    properties.hasAbout = true;
    properties.hasSettings = true;
    properties.shortName = "shout";
    return properties;
}

Output* OutputShoutFactory::create()
{
    return new ShoutOutput(m_connection);
}

Volume *OutputShoutFactory::createVolume()
{
    return 0;
}

void OutputShoutFactory::showSettings(QWidget* parent)
{
    SettingsDialog *s = new SettingsDialog(parent);
    s->show();
}

void OutputShoutFactory::showAbout(QWidget *parent)
{
   QMessageBox::about (parent, tr("About Icecast Output Plugin"),
                       tr("Qmmp Icecast Output Plugin")+"\n"+
                       tr("Compiled against libshout-%1").arg(shout_version(0,0,0))+"\n"+
                       tr("Written by: Ilya Kotov <forkotov02@ya.ru>"));
}

QTranslator *OutputShoutFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/shout_plugin_") + locale);
    return translator;
}
