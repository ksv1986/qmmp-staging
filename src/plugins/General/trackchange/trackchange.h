/***************************************************************************
 *   Copyright (C) 2013-2019 by Ilya Kotov                                 *
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
#ifndef TRACKCHANGE_H
#define TRACKCHANGE_H

#include <qmmpui/general.h>
#include <qmmp/qmmp.h>
#include <qmmp/trackinfo.h>

class QAction;
class SoundCore;
class PlayListTrack;
class PlayListManager;

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class TrackChange : public QObject
{
    Q_OBJECT
public:
    TrackChange(QObject *parent = nullptr);

    ~TrackChange();

private slots:
    void onStateChanged(Qmmp::State state);
    void onTrackInfoChanged();
    void onFinised();

private:
    bool executeCommand(const TrackInfo &info, const QString &format);
    QString m_newTrackCommand;
    QString m_endOfTrackCommand;
    QString m_endOfPlCommand;
    QString m_titleChangeCommand;
    SoundCore *m_core;
    PlayListManager *m_plManager;
    TrackInfo m_prevInfo;
};

#endif
