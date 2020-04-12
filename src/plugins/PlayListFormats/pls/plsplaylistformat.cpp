/***************************************************************************
 *   Copyright (C) 2008-2020 by Ilya Kotov                                 *
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

#include <QtPlugin>
#include <QRegExp>
#include <qmmpui/metadataformatter.h>
#include "plsplaylistformat.h"

PlayListFormatProperties PLSPlaylistFormat::properties() const
{
    PlayListFormatProperties p;
    p.filters << "*.pls";
    p.contentTypes << "audio/x-scpls";
    p.shortName = "pls";
    return p;
}

QList<PlayListTrack *> PLSPlaylistFormat::decode(const QByteArray &contents)
{
    QList<PlayListTrack *> out;
    QStringList splitted = QString::fromUtf8(contents).split("\n");

    if(splitted.isEmpty())
    {
        qWarning("PLSPlaylistFormat: error parsing PLS format");
        return out;
    }

    if(!splitted.takeFirst().toLower().startsWith("[playlist]"))
    {
        qWarning("PLSPlaylistFormat: unknown playlist format");
        return out;
    }

    QRegExp fileRegExp("^File(\\d+)=(.+)");
    QRegExp fullTitleRegExp("^Title(\\d+)=(.+) - (.+)");
    QRegExp titleRegExp("^Title(\\d+)=(.+)");
    QRegExp lengthRegExp("^Length(\\d+)=(-{0,1}\\d+)");

    int number = 0;
    bool error = false;

    for(const QString &line : qAsConst(splitted))
    {
        if(fileRegExp.indexIn(line) > -1)
        {
            if((number = fileRegExp.cap(1).toInt()) > 0)
            {
                while(number > out.count())
                    out << new PlayListTrack();
                out[number - 1]->setPath(fileRegExp.cap(2));
            }
            else
                error = true;
        }
        else if(fullTitleRegExp.indexIn(line) > -1)
        {
            if((number = fullTitleRegExp.cap(1).toInt()) > 0)
            {
                while(number > out.count())
                    out << new PlayListTrack();
                out[number - 1]->setValue(Qmmp::ARTIST, fullTitleRegExp.cap(2));
                out[number - 1]->setValue(Qmmp::TITLE, fullTitleRegExp.cap(3));
            }
            else
                error = true;
        }
        else if(titleRegExp.indexIn(line) > -1)
        {
            if((number = titleRegExp.cap(1).toInt()) > 0)
            {
                while(number > out.count())
                    out << new PlayListTrack();
                out[number - 1]->setValue(Qmmp::TITLE, titleRegExp.cap(2));
            }
            else
                error = true;
        }
        else if(lengthRegExp.indexIn(line) > -1)
        {
            if((number = lengthRegExp.cap(1).toInt()) > 0)
            {
                while(number > out.count())
                    out << new PlayListTrack();
                out[number - 1]->setDuration(lengthRegExp.cap(2).toInt() * 1000);
            }
            else
                error = true;
        }

        if(error)
        {
            qWarning("PLSPlaylistFormat: error while parsing line: '%s'", qPrintable(line));
            qDeleteAll(out);
            out.clear();
            break;
        }
    }

    return out;
}

QByteArray PLSPlaylistFormat::encode(const QList<PlayListTrack *> &contents, const QString &path)
{
    Q_UNUSED(path);
    MetaDataFormatter formatter("%if(%p,%p - %t,%t)%if(%p|%t,,%f)");
    QStringList out;
    out << QString("[playlist]");
    int counter = 1;
    for(const PlayListTrack *f : qAsConst(contents))
    {
        out.append(QString("File%1=%2").arg(counter).arg(f->path()));
        out.append(QString("Title%1=%2").arg(counter).arg(formatter.format(f)));
        out.append(QString("Length%1=%2").arg(counter).arg(f->duration() / 1000));
        counter++;
    }
    out << QString("NumberOfEntries=%1").arg(contents.count());
    out << "Version=2";
    return out.join("\n").toUtf8();
}
