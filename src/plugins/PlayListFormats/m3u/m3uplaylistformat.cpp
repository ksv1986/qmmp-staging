/***************************************************************************
 *   Copyright (C) 2008-2017 by Ilya Kotov                                 *
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

#include <QFileInfo>
#include <QtPlugin>
#include <qmmpui/metadataformatter.h>
#include "m3uplaylistformat.h"

const PlayListFormatProperties M3UPlaylistFormat::properties() const
{
    PlayListFormatProperties p;
    p.filters << "*.m3u" << "*.m3u8";
    p.shortName = "m3u";
    p.contentTypes << "audio/x-mpegurl";
    return p;
}

QList<PlayListTrack *> M3UPlaylistFormat::decode(const QByteArray &contents)
{
    QList<PlayListTrack*> out;
    QStringList splitted = QString::fromUtf8(contents).split("\n");
    if(splitted.isEmpty())
        return out;

    QRegExp extInfRegExp("#EXTINF:(-{0,1}\\d+),(.*) - (.*)");
    int length = 0;
    QString artist, title;
    bool hasExtInf = false;

    foreach(QString str, splitted)
    {
        str = str.trimmed();
        if(str.startsWith("#EXTM3U") || str.isEmpty())
            continue;

        if(extInfRegExp.indexIn(str) > -1)
        {
            length = extInfRegExp.cap(1).toInt();
            artist = extInfRegExp.cap(2);
            title =  extInfRegExp.cap(3);
            hasExtInf = true;
        }

        if(str.startsWith("#"))
            continue;

        out << new PlayListTrack();
        out.last()->insert(Qmmp::URL, str);

        if(hasExtInf)
        {
            out.last()->setLength(length);
            out.last()->insert(Qmmp::ARTIST, artist);
            out.last()->insert(Qmmp::TITLE, title);
            hasExtInf = false;
        }
    }
    return out;
}

QByteArray M3UPlaylistFormat::encode(const QList<PlayListTrack*> &contents, const QString &path)
{
    QStringList out;
    out << QString("#EXTM3U");
    MetaDataFormatter formatter("%if(%p,%p - %t,%t)%if(%p|%t,,%f)");
    QString m3uDir = QFileInfo(path).canonicalPath();

    foreach(PlayListTrack* f, contents)
    {
        QString info = "#EXTINF:" + QString::number(f->length()) + "," + formatter.format(f);
        out.append(info);

        if(!f->url().contains("://") && f->url().startsWith(m3uDir))
        {
            QString p = f->url();
            p.remove(0, m3uDir.size());
            if(p.startsWith("/"))
                p.remove(0, 1);
            out.append(p);
        }
        else
            out.append(f->url());
    }
    return out.join("\n").toUtf8();
}

Q_EXPORT_PLUGIN2(m3uplaylistformat,M3UPlaylistFormat)
