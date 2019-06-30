/***************************************************************************
 *   Copyright (C) 2017-2019 by Ilya Kotov                                 *
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

#ifndef HISTORY_H
#define HISTORY_H

#include <QObject>
#include <QElapsedTimer>
#include <QPointer>
#include <qmmp/trackinfo.h>
#include <qmmp/qmmp.h>

#define CONNECTION_NAME "qmmp_history"

class SoundCore;
class HistoryWindow;

class History : public QObject
{
    Q_OBJECT
public:
    explicit History(QObject *parent = nullptr);
    ~History();

private slots:
    void onTrackInfoChanged();
    void onStateChanged(Qmmp::State state);
    void showHistoryWindow();

private:
    bool createTables();
    void saveTrack();

    SoundCore *m_core;
    TrackInfo m_trackInfo;
    QElapsedTimer m_time;
    Qmmp::State m_previousState;
    quint64 m_elapsed;
    QPointer<HistoryWindow> m_historyWindow;

};

#endif // HISTORY_H
