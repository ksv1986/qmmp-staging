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
#include "settingsdialog.h"
#include "ladspahelper.h"
#include "effectladspafactory.h"

const EffectProperties EffectLADSPAFactory::properties() const
{
    EffectProperties properties;
    properties.name = tr("LADSPA Plugin");
    properties.shortName = "ladspa";
    properties.hasSettings = true;
    properties.hasAbout = true;
    return properties;
}

Effect *EffectLADSPAFactory::create()
{
    return new LADSPAHelper();
}

void EffectLADSPAFactory::showSettings(QWidget *parent)
{
    SettingsDialog *s = new SettingsDialog(parent);
    s->show();
}

void EffectLADSPAFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About LADSPA Host for Qmmp"),
                        tr("LADSPA Host for Qmmp")+"\n"+
                        tr("Written by: Ilya Kotov <forkotov02@ya.ru>")+"\n"+
                        tr("Based on the LADSPA Host for BMP")+"\n"+
                        tr("BMP-ladspa developers:")+"\n"+
                        tr("Nick Lamb <njl195@zepler.org.uk>")+"\n"+
                        tr("Giacomo Lozito <city_hunter@users.sf.net>"));
}

QString EffectLADSPAFactory::translation() const
{
    return QLatin1String(":/ladspa_plugin_");
}
