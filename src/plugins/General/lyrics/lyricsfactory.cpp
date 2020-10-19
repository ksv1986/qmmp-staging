/***************************************************************************
 *   Copyright (C) 2009-2020 by Ilya Kotov                                 *
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
#include "settingsdialog.h"
#include "lyrics.h"
#include "lyricswindow.h"
#include "lyricsfactory.h"

GeneralProperties LyricsFactory::properties() const
{
    GeneralProperties properties;
    properties.name = tr("Lyrics Plugin");
    properties.shortName = "lyrics";
    properties.hasAbout = true;
    properties.hasSettings = true;
    properties.visibilityControl = false;
    properties.widgets = { { 0, tr("Lyrics"), Qt::AllDockWidgetAreas } };
    return properties;
}

QObject *LyricsFactory::create(QObject *parent)
{
    return new Lyrics(parent);
}

QWidget *LyricsFactory::createWidget(int id, QWidget *parent)
{
    if(id == 0)
        return new LyricsWindow(nullptr, parent);
    return nullptr;
}

QDialog *LyricsFactory::createConfigDialog(QWidget *parent)
{
    return new SettingsDialog(parent);
}

void LyricsFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About Lyrics Plugin"),
                        tr("Qmmp Lyrics Plugin")+"\n"+
                        tr("This plugin retrieves lyrics from LyricWiki")+"\n"+
                        tr("Written by: Ilya Kotov <forkotov02@ya.ru>")+"\n"+
                        tr("Based on Ultimate Lyrics script by Vladimir Brkic <vladimir_brkic@yahoo.com>")
                        );
}

QString LyricsFactory::translation() const
{
    return QLatin1String(":/lyrics_plugin_");
}
