/***************************************************************************
 *   Copyright (C) 2013-2021 by Ilya Kotov                                 *
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

#ifndef PLAYLISTGROUP_H
#define PLAYLISTGROUP_H

#include "playlisttrack.h"
#include "playlistitem.h"
#include "qmmpui_export.h"

class GroupedContainer;

/** @brief The PlayListTrack class provides a group for use with the PlayListModel class.
 * @author Ilya Kotov <forkotov02@ya.ru>
 */
class QMMPUI_EXPORT PlayListGroup : public PlayListItem
{
public:
    /*!
     * Constructor.
     * @param formattedTitle Title of the group.
     */
    explicit PlayListGroup(const QString &formattedTitle);
    /*!
     * Object destructor.
     */
    virtual ~PlayListGroup();
    /*!
     * Returns formatted title of the  group.
     * @param column Number of column (unused).
     */
    QString formattedTitle(int column = 0) override;
    QStringList formattedTitles() override;
    /*!
     * Returns \b true if the group contains track \b track.
     * Otherwise returns \b false.
     */
    bool contains(PlayListTrack *track) const;
    /*!
     * Returns \b true if the group is empty.
     * Otherwise returns \b false.
     */
    bool isEmpty() const;
    /*!
     * Returns a list of tracks if the group.
     */
    QList<PlayListTrack *> tracks() const;
    /*!
     * Returns number of tracks if the group.
     */
    int count() const;
    /*!
     *  Returns formatted length of the item.
     */
    QString formattedLength() override;
    /*!
     * Returns \b true.
     */
    bool isGroup() const override;

private:
    QList<PlayListTrack *> trackList; //A list of tracks
    friend class GroupedContainer;

    QString m_name;
};

#endif // PLAYLISTGROUP_H
