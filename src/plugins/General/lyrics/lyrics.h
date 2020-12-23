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
#ifndef LYRICS_H
#define LYRICS_H

#include <QPointer>
#include <qmmpui/general.h>
#include <qmmp/qmmp.h>

class QAction;
class SoundCore;
class LyricsWidget;

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/

class Lyrics : public QObject
{
Q_OBJECT
public:
    explicit Lyrics(QPointer<LyricsWidget> *lyricsWidget, QObject *parent = nullptr);

    ~Lyrics();

private slots:
    void showLyrics();
    void onTrackInfoChanged();

private:
    QAction *m_action;
    QPointer<LyricsWidget> *m_lyricsWidget;

};

#endif
