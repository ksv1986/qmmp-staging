/***************************************************************************
 *   Copyright (C) 2014-2016 by Ilya Kotov                                 *
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

#ifndef PLAYLISTTASK_P_H
#define PLAYLISTTASK_P_H

#include <QThread>
#include <QObject>
#include <QHash>
#include <QList>
#include "playlistmodel.h"
#include "playlistcontainer_p.h"

class PlayListTrack;
struct TrackField;

/** @internal
 * @author Ilya Kotov <forkotov02@ya.ru>
 */
class PlayListTask : public QThread
{
    Q_OBJECT
public:

    enum TaskType
    {
        EMPTY = -1,
        SORT = 0,
        SORT_SELECTION,
        REMOVE_INVALID,
        REMOVE_DUPLICATES,
        SORT_BY_COLUMN,
        REFRESH
    };
    explicit PlayListTask(QObject *parent);

    ~PlayListTask();

    void sort(QList<PlayListTrack *> tracks, PlayListModel::SortMode mode);
    void sortSelection(QList<PlayListTrack *> tracks, PlayListModel::SortMode mode);
    void sortByColumn(QList <PlayListTrack *> tracks, int column);
    void removeInvalidTracks(QList<PlayListTrack *> tracks, PlayListTrack *current_track);
    void removeDuplicates(QList<PlayListTrack *> tracks, PlayListTrack *current_track);
    void refresh(QList<PlayListTrack *> tracks, PlayListTrack *current_track);

    void run() override;

    TaskType type() const;
    bool isChanged(PlayListContainer *container);
    QList<PlayListTrack *> takeResults(PlayListTrack **current_track);
    PlayListTrack *currentTrack() const;
    int column() const;
    bool isReverted() const;
    void clear();


private:
    QList <TrackField *> m_fields;
    QList <PlayListTrack *> m_tracks;
    QList <PlayListTrack *> m_input_tracks;
    QList<int> m_indexes;
    QList <PlayListTrack *> m_new_tracks;
    PlayListTrack *m_current_track;
    int m_sort_mode = PlayListModel::TITLE;
    TaskType m_task = EMPTY;
    bool m_reverted = true;
    bool m_align_groups = false;
    int m_column = 0;
    QHash<int, Qmmp::MetaData> m_sort_keys;

};

#endif // PLAYLISTTASK_P_H
