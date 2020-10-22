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

#include <QAction>
#include <QApplication>
#include <qmmp/soundcore.h>
#include <qmmpui/uihelper.h>
#include <qmmpui/playlistmanager.h>
#include <qmmpui/playlistitem.h>
#include <qmmpui/mediaplayer.h>
#include "lyricswidget.h"
#include "lyrics.h"

Lyrics::Lyrics(QPointer<LyricsWidget> *lyricsWidget, QObject *parent) : QObject(parent)
{
    m_lyricsWidget = lyricsWidget;
    m_action = new QAction(tr("View Lyrics"), this);
    m_action->setShortcut(tr("Ctrl+L"));
    UiHelper::instance()->addAction(m_action, UiHelper::PLAYLIST_MENU);
    connect(m_action, SIGNAL(triggered ()), SLOT(showLyrics()));
    connect(SoundCore::instance(), SIGNAL(trackInfoChanged()), SLOT(onTrackInfoChanged()));
}

Lyrics::~Lyrics()
{}

void Lyrics::showLyrics()
{
    PlayListManager *pl_manager = MediaPlayer::instance()->playListManager();
    QList <PlayListTrack *> tracks = pl_manager->selectedPlayList()->selectedTracks();
    if (!tracks.isEmpty())
    {
        if (tracks.at(0)->value(Qmmp::ARTIST).isEmpty() || tracks.at(0)->value(Qmmp::TITLE).isEmpty())
            return;

        if(!m_lyricsWidget->isNull() && m_lyricsWidget->data()->isVisible())
        {
            m_lyricsWidget->data()->fetch(tracks.first());
        }
        else
        {
            LyricsWidget *w = new LyricsWidget(true, qApp->activeWindow());
            w->fetch(tracks.first());
            w->show();
        }
    }
}

void Lyrics::onTrackInfoChanged()
{
    if(!m_lyricsWidget->isNull())
    {
        TrackInfo info = SoundCore::instance()->trackInfo();
        if (!info.value(Qmmp::ARTIST).isEmpty() && !info.value(Qmmp::TITLE).isEmpty())
        {
            m_lyricsWidget->data()->fetch(&info);
        }
    }
}
