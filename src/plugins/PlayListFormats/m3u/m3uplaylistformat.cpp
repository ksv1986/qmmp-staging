/***************************************************************************
 *   Copyright (C) 2008-2017 by Ilya Kotov                                 *
 *   forkotov02@hotmail.ru                                                 *
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

QStringList M3UPlaylistFormat::decode(const QString & contents)
{
    QStringList out;
    QStringList splitted = contents.split("\n");
    if(splitted.isEmpty())
        return QStringList();

    foreach(QString str, splitted)
    {
        str = str.trimmed ();
        if (str.startsWith("#EXTM3U") || str.startsWith("#EXTINF:") || str.isEmpty())
            continue;//TODO: Let's skip it for now..
        else if (str.startsWith("#") || str.isEmpty())
            continue;
        else
            out << str;
    }
    return out;
}

QString M3UPlaylistFormat::encode(const QList<PlayListTrack*> & contents, const QString &path)
{
    QStringList out;
    out << QString("#EXTM3U");
    MetaDataFormatter formatter("%if(%p,%p - %t,%t)%if(%p|%t,,%f)");
    QString m3uDir = QFileInfo(path).canonicalPath();

    foreach(PlayListTrack* f,contents)
    {
        QString info = "#EXTINF:" + QString::number(f->length()) + "," + formatter.format(f);
        out.append(info);

        if(!f->url().contains("://") && f->url().startsWith(m3uDir))
        {
            QString p = f->url();
            p.remove(0, m3uDir.size() + 1);
            out.append(p);
        }
        else
            out.append(f->url());
    }
    return out.join("\n");
}

Q_EXPORT_PLUGIN2(m3uplaylistformat,M3UPlaylistFormat)
