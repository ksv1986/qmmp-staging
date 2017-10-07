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
#include <qmmp/qmmp.h>
#include "filewriterplugin.h"
#include "settingsdialog.h"
#include "effectfilewriterfactory.h"

const EffectProperties EffectFileWriterFactory::properties() const
{
    EffectProperties properties;
    properties.name = tr("File Writer Plugin");
    properties.shortName = "filewriter";
    properties.hasSettings = true;
    properties.hasAbout = true;
    properties.priority = EffectProperties::EFFECT_PRIORITY_LOW;
    return properties;
}

Effect *EffectFileWriterFactory::create()
{
    return new FileWriterPlugin();
}

void EffectFileWriterFactory::showSettings(QWidget *parent)
{
    SettingsDialog *dialog = new SettingsDialog(parent);
    dialog->show();
}

void EffectFileWriterFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About File Writer Plugin"),
                       tr("Qmmp File Writer Plugin")+"\n"+
                       tr("Written by: Ilya Kotov <forkotov02@ya.ru>"));
}

QTranslator *EffectFileWriterFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/filewriter_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN2(filewriter,EffectFileWriterFactory)
