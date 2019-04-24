/***************************************************************************
 *   Copyright (C) 2008-2019 by Ilya Kotov                                 *
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
#ifndef CUEPARSER_H
#define CUEPARSER_H

#include <QByteArray>
#include <QString>
#include <QList>
#include <QStringList>
#include "trackinfo.h"
#include "qmmp_export.h"

class QMMP_EXPORT CueParser
{
public:
    CueParser();
    CueParser(const QByteArray &data, const QByteArray &codecName = QByteArray());
    ~CueParser();
    void loadData(const QByteArray &data, const QByteArray &codecName = QByteArray());
    QList<TrackInfo *> createPlayList() const;
    const QStringList &files() const;
    qint64 offset(int track) const;
    qint64 duration(int track) const;
    QString file(int track) const;
    QString url(int track) const;
    int count() const;
    const TrackInfo *info(int track) const;
    void setDuration(const QString &file, qint64 duration);
    void setDuration(qint64 duration);
    void setProperties(const QString &file, const QMap<Qmmp::TrackProperty, QString> &properties);
    void setProperties(const QMap<Qmmp::TrackProperty, QString> &properties);
    void setUrl(const QString &scheme, const QString &path);

private:
    struct CUETrack
    {
        TrackInfo info;
        QString file;
        qint64 offset = 0;
    };
    QList<CUETrack *> m_tracks;
    QStringList m_files;
    QStringList splitLine(const QString &line);
    qint64 getLength(const QString &str);
};

#endif // CUEPARSER_H
