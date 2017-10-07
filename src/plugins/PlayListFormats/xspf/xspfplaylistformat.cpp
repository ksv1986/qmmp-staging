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

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFileInfo>
#include <QUrl>
#include <QtPlugin>
#include <qmmp/qmmp.h>
#include "xspfplaylistformat.h"

// Needs more work - it's better use libSpiff there and put it as plugin.

const PlayListFormatProperties XSPFPlaylistFormat::XSPFPlaylistFormat::properties() const
{
    PlayListFormatProperties p;
    p.filters << "*.xspf";
    p.shortName = "xspf";
    p.contentTypes << "application/xspf+xml";
    return p;
}

QList<PlayListTrack*> XSPFPlaylistFormat::decode(const QByteArray &contents)
{
    QList<PlayListTrack*> out;
    QString currentTag;
    QString contents_copy = QString::fromUtf8(contents);

    //remove control symbols to avoid xml errors
    for(int i = 0; i < contents_copy.size(); ++i)
    {
        if(contents_copy[i] <= 0x1F)
        {
            contents_copy.replace(i, 1, "");
            i--;
        }
    }

    QXmlStreamReader xml(contents_copy);
    while(!xml.atEnd() || !xml.hasError())
    {
        xml.readNext();
        if (xml.isStartElement())
        {
            currentTag = xml.name().toString();
            if(currentTag == "track")
                out << new PlayListTrack();
        }
        else if (xml.isCharacters() && !xml.isWhitespace())
        {
            if(out.isEmpty())
                continue;

            if(currentTag == "location")
            {
                QUrl url(xml.text().toString());
                if (url.scheme() == "file")  //remove scheme for local files only
                    out.last()->insert(Qmmp::URL, QUrl::fromPercentEncoding(url.toString().toLatin1()).remove("file://"));
                else
                    out.last()->insert(Qmmp::URL, QUrl::fromPercentEncoding(url.toString().toLatin1()));
            }
            else if(currentTag == "title")
            {
                out.last()->insert(Qmmp::TITLE, xml.text().toString());
            }
            else if(currentTag == "creator")
            {
                out.last()->insert(Qmmp::ARTIST, xml.text().toString());
            }
            else if(currentTag == "annotation")
            {
                out.last()->insert(Qmmp::COMMENT, xml.text().toString());
            }
            else if(currentTag == "album")
            {
                out.last()->insert(Qmmp::ALBUM, xml.text().toString());
            }
            else if(currentTag == "meta" && xml.attributes().value("rel") == "year")
            {
                out.last()->insert(Qmmp::YEAR, xml.text().toString());
            }
            else
                xml.skipCurrentElement();
        }
    }

    if(xml.hasError())
    {
        qDebug("XSPFPlaylistFormat: parse error: %s (row:%lld, col:%lld",
               qPrintable(xml.errorString()), xml.lineNumber(), xml.columnNumber());
    }
    return out;
}

// Needs more work - it's better use libSpiff there and put it as plugin.

QByteArray XSPFPlaylistFormat::encode(const QList<PlayListTrack*> &files, const QString &path)
{
    QString xspfDir = QFileInfo(path).canonicalPath();
    QByteArray out;
    QXmlStreamWriter xml(&out);
    xml.setCodec("UTF-8");
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeStartElement("playlist");
    xml.writeAttribute("version", "1");
    xml.writeAttribute("xmlns", "http://xspf.org/ns/0/");
    xml.writeTextElement("creator", "qmmp-" + Qmmp::strVersion());
    xml.writeStartElement("trackList");

    int counter = 1;
    foreach(PlayListTrack* f, files)
    {
        xml.writeStartElement("track");

        QString url;
        if (f->url().contains("://"))
        {
            url = QUrl::toPercentEncoding(f->url(), ":/");
        }
        else if(f->url().startsWith(xspfDir)) //relative path
        {
            QString p = f->url();
            p.remove(0, xspfDir.size());
            if(p.startsWith("/"))
                p.remove(0, 1);
            url = QUrl::toPercentEncoding(p, ":/");
        }
        else  //absolute path
        {
            url = QUrl::toPercentEncoding(QLatin1String("file://") + f->url(), ":/");
        }

        xml.writeTextElement("location", url);
        xml.writeTextElement("title", f->value(Qmmp::TITLE));
        xml.writeTextElement("creator", f->value(Qmmp::ARTIST));
        xml.writeTextElement("annotation", f->value(Qmmp::COMMENT));
        xml.writeTextElement("album", f->value(Qmmp::ALBUM));
        xml.writeTextElement("trackNum", QString::number(counter));

        xml.writeStartElement("meta");
        xml.writeAttribute("rel", "year");
        xml.writeCharacters(f->value(Qmmp::YEAR));
        xml.writeEndElement(); // meta
        xml.writeEndElement(); // track

        counter ++;
    }
    xml.writeEndElement(); //trackList
    xml.writeEndElement(); //playlist
    xml.writeEndDocument();
    return out;
}

Q_EXPORT_PLUGIN2(xspfplaylistformat,XSPFPlaylistFormat)
