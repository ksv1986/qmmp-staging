/***************************************************************************
 *   Copyright (C) 2013 by Ilya Kotov                                      *
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

#ifndef NORMALCONTAINER_P_H
#define NORMALCONTAINER_P_H

#include "playlistcontainer_p.h"


/*! @internal
 * @brief Normal playlist container (i.e. without groups).
 * @author Ilya Kotov <forkotov02@ya.ru>
 */
class NormalContainer : public PlayListContainer
{
public:
    NormalContainer();
    virtual ~NormalContainer();

    void addTracks(const QList<PlayListTrack *> &tracks) override;
    void insertTrack(int index, PlayListTrack *track) override;
    void replaceTracks(const QList<PlayListTrack *> &tracks) override;
    QList<PlayListGroup *> groups() const override;
    QList<PlayListTrack *> tracks() const override;
    QList<PlayListItem *> items() const override;
    int count() const override;
    int trackCount() const override;
    QList<PlayListItem *> mid(int pos, int count) const override;
    bool isEmpty() const override;
    bool isSelected(int index) const override;
    void setSelected(int index, bool selected) override;
    void clearSelection() override;
    int indexOf(PlayListItem *item) const override;
    PlayListItem *item(int index) const override;
    PlayListTrack *track(int index) const override;
    PlayListGroup *group(int index) const override;
    bool contains(PlayListItem *item) const override;
    int indexOfTrack(int index) const override;
    PlayListTrack *findTrack(int number) const override;
    void removeTrack(PlayListTrack *track) override;
    void removeTracks(QList<PlayListTrack *> tracks) override;
    bool move(const QList<int> &indexes, int from, int to) override;
    QList<PlayListTrack *> takeAllTracks() override;
    void clear() override;

    void reverseList() override;
    void randomizeList() override;

private:
    QList<PlayListItem *> m_items;
};

#endif // NORMALCONTAINER_P_H
