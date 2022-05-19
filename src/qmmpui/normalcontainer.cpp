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

#include <QRandomGenerator>
#include "normalcontainer_p.h"

NormalContainer::NormalContainer()
{}

NormalContainer::~NormalContainer()
{
}

void NormalContainer::addTracks(const QList<PlayListTrack *> &tracks)
{
    for(PlayListTrack *track : qAsConst(tracks))
    {
        m_items.append(track);
        track->m_queued_index = -1;
        track->m_track_index = m_items.count() - 1;
    }
}

int NormalContainer::insertTrack(int index, PlayListTrack *track)
{
    track->m_queued_index = -1;
    if(index >= 0 && index < m_items.count())
    {
        m_items.insert(index, track);
        track->m_track_index = index;
        //update indexes
        for(int i = index; i < m_items.count(); ++i)
            static_cast<PlayListTrack *>(m_items[i])->m_track_index = i;
        return index;
    }
    else
    {
        m_items.append(track);
        track->m_track_index = m_items.count() - 1;
        return m_items.count() - 1;
    }
}

void NormalContainer::replaceTracks(const QList<PlayListTrack *> &tracks)
{
    clearQueue();
    m_items.clear();
    addTracks(tracks);
}

QList<PlayListGroup *> NormalContainer::groups() const
{
    return QList<PlayListGroup *>();
}

QList<PlayListTrack *> NormalContainer::tracks() const
{
    QList<PlayListTrack *> trackList;
    for(int i = 0; i < m_items.count(); ++i)
        trackList.append(static_cast<PlayListTrack *>(m_items[i]));
    return trackList;
}

const QList<PlayListItem *> &NormalContainer::items() const
{
    return m_items;
}

int NormalContainer::count() const
{
    return m_items.count();
}

int NormalContainer::trackCount() const
{
    return m_items.count();
}

QList<PlayListItem *> NormalContainer::mid(int pos, int count) const
{
    return m_items.mid(pos, count);
}

bool NormalContainer::isEmpty() const
{
    return m_items.isEmpty();
}

bool NormalContainer::isSelected(int index) const
{
    if (0 <= index && index < m_items.count())
        return m_items.at(index)->isSelected();
    return false;
}

void NormalContainer::setSelected(int index, bool selected)
{
    if (0 <= index && index < m_items.count())
        m_items.at(index)->setSelected(selected);
}

void NormalContainer::clearSelection()
{
    for(PlayListItem *item : qAsConst(m_items))
    {
        item->setSelected(false);
    }
}

int NormalContainer::indexOf(PlayListItem *item) const
{
    return m_items.indexOf(item);
}

PlayListItem *NormalContainer::item(int index) const
{
    if(index >= count() || index < 0)
    {
        qWarning("NormalContainer: index is out of range");
        return nullptr;
    }
    return m_items.at(index);
}

PlayListTrack *NormalContainer::track(int index) const
{
    PlayListItem *i = item(index);
    return static_cast<PlayListTrack *> (i);
}

PlayListGroup *NormalContainer::group(int index) const
{
    Q_UNUSED(index);
    return nullptr;
}

bool NormalContainer::contains(PlayListItem *item) const
{
    return m_items.contains(item);
}

int NormalContainer::indexOfTrack(int index) const
{
    return index;
}

PlayListTrack *NormalContainer::findTrack(int number) const
{
    if(number >= count() || number < 0)
    {
        return nullptr;
    }
    return static_cast<PlayListTrack *> (m_items.at(number));
}

void NormalContainer::removeTrack(PlayListTrack *track)
{
    removeTracks(QList<PlayListTrack *> () << track);
}

void NormalContainer::removeTracks(QList<PlayListTrack *> tracks)
{
    for(PlayListTrack *t : qAsConst(tracks))
    {
        m_items.removeAll(t);
        removeFromQueue(t);
    }

    for(int i = 0; i < m_items.count(); ++i)
        static_cast<PlayListTrack *>(m_items[i])->m_track_index = i;
}

bool NormalContainer::move(const QList<int> &indexes, int from, int to)
{
    if (from > to)
    {
        for(const int &i : qAsConst(indexes))
        {
            if (i + to - from < 0)
                break;

            else
            {
                m_items.move(i,i + to - from);
                swapTrackNumbers(&m_items,i,i + to - from);
            }
        }
    }
    else
    {
        for (int i = indexes.count() - 1; i >= 0; i--)
        {
            if (indexes[i] + to - from >= m_items.count())
                break;
            else
            {
                m_items.move(indexes[i], indexes[i] + to - from);
                swapTrackNumbers(&m_items,indexes[i], indexes[i] + to - from);
            }
        }
    }
    return true;
}

QList<PlayListTrack *> NormalContainer::takeAllTracks()
{
    clearQueue();
    QList<PlayListTrack *> tracks;
    while(!m_items.isEmpty())
        tracks.append(static_cast<PlayListTrack *>(m_items.takeFirst()));
    return tracks;
}

void NormalContainer::clear()
{
    clearQueue();
    qDeleteAll(m_items);
    m_items.clear();
}

void NormalContainer::reverseList()
{
    for (int i = 0; i < m_items.size()/2; i++)
    {
        m_items.swapItemsAt(i, m_items.size() - i - 1);
        swapTrackNumbers(&m_items, i, m_items.size() - i - 1);
    }
}

void NormalContainer::randomizeList()
{
    QRandomGenerator *rg = QRandomGenerator::global();

    for (int i = 0; i < m_items.size(); i++)
        m_items.swapItemsAt(rg->generate() % m_items.size(), rg->generate() % m_items.size());

    for(int i = 0; i < m_items.count(); ++i)
        static_cast<PlayListTrack *>(m_items[i])->m_track_index = i;
}
