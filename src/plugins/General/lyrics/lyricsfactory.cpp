/***************************************************************************
 *   Copyright (C) 2009-2021 by Ilya Kotov                                 *
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
#include <qmmp/soundcore.h>
#include "settingsdialog.h"
#include "lyrics.h"
#include "lyricswidget.h"
#include "lyricsfactory.h"

GeneralProperties LyricsFactory::properties() const
{
    GeneralProperties properties;
    properties.name = tr("Lyrics Plugin");
    properties.shortName = "lyrics";
    properties.hasAbout = true;
    properties.hasSettings = true;
    properties.visibilityControl = false;
    properties.widgets = { { LYRICS_WIDGET, tr("Lyrics"), Qt::LeftDockWidgetArea, Qt::AllDockWidgetAreas } };
    return properties;
}

QObject *LyricsFactory::create(QObject *parent)
{
    return new Lyrics(&m_lyricsWidget, parent);
}

QWidget *LyricsFactory::createWidget(int id, QWidget *parent)
{
    if(id == LYRICS_WIDGET)
    {
        m_lyricsWidget = new LyricsWidget(false, parent);
        if(SoundCore::instance()->state() == Qmmp::Playing || SoundCore::instance()->state() == Qmmp::Paused)
        {
            TrackInfo info = SoundCore::instance()->trackInfo();
            if (!info.value(Qmmp::ARTIST).isEmpty() && !info.value(Qmmp::TITLE).isEmpty())
                m_lyricsWidget->fetch(&info);
        }
        return m_lyricsWidget;
    }
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
