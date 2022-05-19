/***************************************************************************
 *   Copyright (C) 2013-2022 by Ilya Kotov                                 *
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

#include "playlistmodel.h"
#include "playlistcontainer_p.h"

void PlayListContainer::addTrack(PlayListTrack *track)
{
    addTracks(QList<PlayListTrack *> () << track);
}

PlayListTrack *PlayListContainer::dequeue()
{
    PlayListTrack *t = m_queue.dequeue();
    t->m_queued_index = -1;
    updateQueueIndexes();
    return t;
}

void PlayListContainer::enqueue(PlayListTrack *track)
{
    m_queue.enqueue(track);
    updateQueueIndexes();
}

void PlayListContainer::removeFromQueue(PlayListTrack *track)
{
    if(track->isQueued())
    {
        m_queue.removeAll(track);
        track->m_queued_index = -1;
        updateQueueIndexes();
    }
}

void PlayListContainer::clearQueue()
{
    for(int i = 0; i < m_queue.size(); ++i)
        m_queue[i]->m_queued_index = -1;

    m_queue.clear();
}

void PlayListContainer::restoreQueue(const QList<PlayListTrack *> &tracks)
{
    m_queue.clear();
    for(PlayListTrack *t : qAsConst(tracks))
        m_queue.enqueue(t);
    updateQueueIndexes();
}

const QList<PlayListTrack *> &PlayListContainer::queuedTracks() const
{
    return m_queue;
}

void PlayListContainer::swapTrackNumbers(QList<PlayListItem *> *container, int index1, int index2)
{
    PlayListTrack *track1 = static_cast<PlayListTrack *>(container->at(index1));
    PlayListTrack *track2 = static_cast<PlayListTrack *>(container->at(index2));
    int number = track1->trackIndex();
    track1->m_track_index = track2->m_track_index;
    track2->m_track_index = number;
}

void PlayListContainer::updateQueueIndexes()
{
    for(int i = 0; i < m_queue.size(); ++i)
        m_queue[i]->m_queued_index = i;
}
