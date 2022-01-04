/***************************************************************************
 *   Copyright (C) 2006-2021 by Ilya Kotov                                 *
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

#ifndef PALYLISTFORMAT_H
#define PALYLISTFORMAT_H

#include <QStringList>
#include "qmmpui_export.h"

class PlayListTrack;

/*! @brief Helper structure to store playlist format properies.
 * @author Ilya Kotov <forkotov02@ya.ru>
 */
struct PlayListFormatProperties
{
    QString shortName;        /*!< Unique name of the playlist format for internal usage */
    QStringList filters;      /*!< File filters (example: "*.m3u") */
    QStringList contentTypes; /*!< Supported content types */
};

/*! @brief Abstract interface for playlist formats.
 * @author Vladimir Kuznetsov <vovanec@gmail.com>
 */
class QMMPUI_EXPORT PlayListFormat
{
public:
    /*!
     * Object destructor
     */
    virtual ~PlayListFormat() {}
    /*!
     * Returns playlist format properties.
     */
    virtual PlayListFormatProperties properties() const = 0;
    /*!
     * Takes raw contents of playlist file, should return string list of
     * ready file pathes to fill the playlist.
     */
    virtual QList<PlayListTrack*> decode(const QByteArray &contents) = 0;
    /*!
     * Takes the list of \b PlayListTrack objects, should return content of
     * encoded playlist file.
     * @param path Playlist file path (May be used to adjust playlist content).
     * @param contents A list of \b PlayListTrack pointers.
     */
    virtual QByteArray encode(const QList<PlayListTrack*> &contents, const QString &path) = 0;
};

Q_DECLARE_INTERFACE(PlayListFormat,"PlayListFormat/1.0")

#endif
