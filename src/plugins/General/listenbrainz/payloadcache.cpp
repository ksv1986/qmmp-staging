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

#include <QFile>
#include "payloadcache.h"

TrackMetaData::TrackMetaData()
{}

TrackMetaData::TrackMetaData(const TrackInfo &info) : TrackInfo(info)
{}

TrackMetaData::TrackMetaData(const TrackMetaData &other) : TrackInfo(other),
    m_start_ts(other.timeStamp())
{}

TrackMetaData::~TrackMetaData()
{}

TrackMetaData & TrackMetaData::operator=(const TrackMetaData &info)
{
    TrackInfo::operator=(info);
    m_start_ts = info.timeStamp();
    return *this;
}

bool TrackMetaData::operator==(const TrackMetaData &info)
{
    return (TrackInfo::operator==(info)) && (m_start_ts == info.timeStamp());
}

bool TrackMetaData::operator!=(const TrackMetaData &info)
{
    return !operator==(info);
}

void TrackMetaData::setTimeStamp(uint ts)
{
    m_start_ts = ts;
}

uint TrackMetaData::timeStamp() const
{
    return m_start_ts;
}

PayloadCache::PayloadCache(const QString &filePath) : m_filePath(filePath)
{}

QList<TrackMetaData> PayloadCache::load()
{
    QList<TrackMetaData> songs;
    QFile file(m_filePath);

    if(!file.open(QIODevice::ReadOnly))
        return QList<TrackMetaData>();

    while (!file.atEnd())
    {
        int s = 0;
        QString line = QString::fromUtf8(file.readLine()).trimmed();
        if ((s = line.indexOf("=")) < 0)
            continue;

        QString param = line.left(s);
        QString value = line.right(line.size() - s - 1);

        if (param == "title")
        {
            songs << TrackMetaData();
            songs.last().setValue(Qmmp::TITLE, value);
        }
        else if (songs.isEmpty())
            continue;
        else if (param == "artist")
            songs.last().setValue(Qmmp::ARTIST, value);
        else if (param == "album")
            songs.last().setValue(Qmmp::ALBUM, value);
        else if (param == "comment")
            songs.last().setValue(Qmmp::COMMENT, value);
        else if (param == "genre")
            songs.last().setValue(Qmmp::GENRE, value);
        else if (param == "year")
            songs.last().setValue(Qmmp::YEAR, value);
        else if (param == "track")
            songs.last().setValue(Qmmp::TRACK, value);
        else if (param == "length") //1.3.x config support
            songs.last().setDuration(value.toInt() * 1000);
        else if (param == "duration")
            songs.last().setDuration(value.toLongLong());
        else if (param == "time")
            songs.last().setTimeStamp(value.toUInt());
    }
    file.close();
    return songs;
}

void PayloadCache::save(const QList<TrackMetaData> &songs)
{
    QFile file(m_filePath);
    if (songs.isEmpty())
    {
        file.remove();
        return;
    }
    if(!file.open(QIODevice::WriteOnly))
    {
        qWarning("PayloadCache: unable to save file %s", qPrintable(m_filePath));
        qWarning("PayloadCache: error %d: %s", file.error(), qPrintable(file.errorString()));
        return;
    }
    for(const TrackMetaData &m : qAsConst(songs))
    {
        file.write(QString("title=%1").arg(m.value(Qmmp::TITLE)).toUtf8() +"\n");
        file.write(QString("artist=%1").arg(m.value(Qmmp::ARTIST)).toUtf8() +"\n");
        file.write(QString("album=%1").arg(m.value(Qmmp::ALBUM)).toUtf8() +"\n");
        file.write(QString("comment=%1").arg(m.value(Qmmp::COMMENT)).toUtf8() +"\n");
        file.write(QString("genre=%1").arg(m.value(Qmmp::GENRE)).toUtf8() +"\n");
        file.write(QString("year=%1").arg(m.value(Qmmp::YEAR)).toUtf8() +"\n");
        file.write(QString("track=%1").arg(m.value(Qmmp::TRACK)).toUtf8() +"\n");
        file.write(QString("duration=%1").arg(m.duration()).toLatin1() +"\n");
        file.write(QString("time=%1").arg(m.timeStamp()).toLatin1() +"\n");
    }
    file.close();
}
