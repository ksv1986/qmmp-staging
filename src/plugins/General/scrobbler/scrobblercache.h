/***************************************************************************
 *   Copyright (C) 2013-2020 by Ilya Kotov                                 *
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

#ifndef SCROBBLERCACHE_H
#define SCROBBLERCACHE_H

#include <qmmp/qmmp.h>
#include <qmmp/trackinfo.h>

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class SongInfo : public TrackInfo
{
public:
    SongInfo();
    SongInfo(const TrackInfo &info);
    SongInfo(const SongInfo &other);

    ~SongInfo();

    SongInfo & operator=(const SongInfo &info);
    bool operator==(const SongInfo &info);
    bool operator!=(const SongInfo &info);
    void setTimeStamp(uint ts);
    uint timeStamp() const;

private:
    uint m_start_ts = 0;
};

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class ListenCache
{
public:
    explicit ListenCache(const QString &filePath);

    QList<SongInfo> load();
    void save(const QList<SongInfo> &songs);

private:
    QString m_filePath;

};

#endif // SCROBBLERCACHE_H
