/***************************************************************************
 *   Copyright (C) 2019-2021 by Ilya Kotov                                 *
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

#ifndef PAYLOADCACHE_H
#define PAYLOADCACHE_H

#include <qmmp/qmmp.h>
#include <qmmp/trackinfo.h>

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class TrackMetaData : public TrackInfo
{
public:
    TrackMetaData();
    explicit TrackMetaData(const TrackInfo &info);
    TrackMetaData(const TrackMetaData &other);

    ~TrackMetaData();

    TrackMetaData & operator=(const TrackMetaData &info);
    bool operator==(const TrackMetaData &info);
    bool operator!=(const TrackMetaData &info);
    void setTimeStamp(uint ts);
    uint timeStamp() const;

private:
    uint m_start_ts = 0;
};

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class PayloadCache
{
public:
    explicit PayloadCache(const QString &filePath);

    QList<TrackMetaData> load();
    void save(const QList<TrackMetaData> &songs);

private:
    QString m_filePath;

};

#endif // PAYLOADCACHE_H
