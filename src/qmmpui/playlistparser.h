/***************************************************************************
 *   Copyright (C) 2008-2021 by Ilya Kotov                                 *
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
#ifndef PLAYLISTPARSER_H
#define PLAYLISTPARSER_H

#include <QStringList>
#include <QUrl>
#include <QHash>
#include "playlisttrack.h"
#include "playlistformat.h"
#include "qmmpui_export.h"

/*! @brief The PlaylistParser class provides a simple api to access playlist format plugins.
 * @author Ilya Kotov <forkotov02@ya.ru>
 */
class QMMPUI_EXPORT PlayListParser
{
public:
    /*!
     * Returns a list of the installed playlist formats.
     */
    static QList<PlayListFormat*> formats();
    /*!
     * Returns a list of the supported files name filters, i.e. "*.m3u *.pls"
     */
    static QStringList nameFilters();
    /*!
     * Returns a list of file name filters with description, i.e. "M3U (*.m3u)"
     */
    static QStringList filters();
    /*!
     * Returns \b true if file \b url is playlist. Otherwise returns \b false.
     * \param url Local file path or URL.
     */
    static bool isPlayList(const QString &url);
    /*!
     * Returns PlayListFormat pointer which supports mime type \b mime
     * or \b 0 if mime type \b mime is unsupported
     */
    static PlayListFormat *findByMime(const QString &mime);
    /*!
     * Finds playlist format by file path \b filePath
     * Returns \b 0 if file \b filePath is unsupported.
     */
    static PlayListFormat *findByPath(const QString &filePath);
    /*!
     * Finds playlist format by url path \b url
     * Returns \b 0 if file \b filePath is unsupported.
     */
    static PlayListFormat *findByUrl(const QUrl &url);
    /*!
     * Saves a list of tracks to the file.
     * @param tracks A list of tracks.
     * @param f_name File name the  playlist.
     */
    static void savePlayList(QList<PlayListTrack *> tracks, const QString &f_name);
    /*!
     * Loads playlist from file \b f_name.
     * @param f_name File name.
     * @return A list of tracks.
     */
    static QList<PlayListTrack *> loadPlaylist(const QString &f_name);
    /*!
     * Loads playlist from content \b content.
     * @param fmt Playlist format (short name).
     * @param content Playlist content.
     * @return A list of tracks.
     */
    static QList<PlayListTrack *> loadPlaylist(const QString &fmt, const QByteArray &content);
    /*!
     * Loads all playlist plugins. Should be called before usage from another thread.
     */
    static void loadFormats();

    static QByteArray serialize(const QList<PlayListTrack *> &tracks);
    static QList<PlayListTrack *> deserialize(const QByteArray &json);

private:
    PlayListParser(){}

    static QList<PlayListFormat*> *m_formats;
    static const QHash<QString, Qmmp::MetaData> m_metaKeys;
    static const QHash<QString, Qmmp::TrackProperty> m_propKeys;


};

#endif
