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

#include <QPluginLoader>
#include <QRegExp>
#include <QList>
#include <QDir>
#include <QApplication>
#include <qmmp/qmmp.h>
#include "playlistformat.h"
#include "playlistparser.h"

QList<PlayListFormat*> *PlayListParser::m_formats = 0;

QList<PlayListFormat *> PlayListParser::formats()
{
    loadFormats();
    return *m_formats;
}

QStringList PlayListParser::nameFilters()
{
    loadFormats();
    QStringList filters;
    foreach(PlayListFormat* format, *m_formats)
    {
        filters << format->properties().filters;
    }
    return filters;
}

bool PlayListParser::isPlayList(const QString &url)
{
    foreach (QString filter, nameFilters())
    {
        QRegExp r(filter, Qt::CaseInsensitive, QRegExp::Wildcard);
        if(r.exactMatch(url))
            return true;
    }
    return false;
}

PlayListFormat *PlayListParser::findByMime(const QString &mime)
{
    loadFormats();
    foreach(PlayListFormat* format, *m_formats)
    {
        if(format->properties().contentTypes.contains(mime))
            return format;
    }
    return 0;
}

PlayListFormat *PlayListParser::findByPath(const QString &filePath)
{
    loadFormats();
    foreach(PlayListFormat* format, *m_formats)
    {
        foreach(QString filter, format->properties().filters)
        {
            QRegExp r(filter, Qt::CaseInsensitive, QRegExp::Wildcard);
            if(r.exactMatch(filePath))
                return format;
        }
    }
    return 0;
}

PlayListFormat *PlayListParser::findByUrl(const QUrl &url)
{
    QString path = url.encodedPath();
    return findByPath(path);
}

void PlayListParser::savePlayList(QList<PlayListTrack *> tracks, const QString &f_name)
{
    if(tracks.isEmpty())
        return;
    PlayListFormat* prs = PlayListParser::findByPath(f_name);
    if (!prs)
        return;
    QFile file(f_name);
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(prs->encode(tracks, QFileInfo(f_name).canonicalFilePath()));
        file.close();
    }
    else
        qWarning("PlayListParser: unable to save playlist, error: %s", qPrintable(file.errorString()));
}

QList<PlayListTrack *> PlayListParser::loadPlaylist(const QString &f_name)
{
    if(!QFile::exists(f_name))
        return QList<PlayListTrack *>();
    PlayListFormat* prs = PlayListParser::findByPath(f_name);
    if(!prs)
        return QList<PlayListTrack *>();

    QFile file(f_name);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning("PlayListParser: unable to open playlist, error: %s", qPrintable(file.errorString()));
        return QList<PlayListTrack *>();
    }

    QList <PlayListTrack*> tracks = prs->decode(file.readAll());

    if(tracks.isEmpty())
    {
        qWarning("PlayListParser: error opening %s",qPrintable(f_name));
        return tracks;
    }

    QString url;
    foreach (PlayListTrack *t, tracks)
    {
        url = t->value(Qmmp::URL);

        if(url.contains("://"))
            continue;

        if(QFileInfo(url).isRelative())
            url.prepend(QFileInfo(f_name).canonicalPath () + "/");

        url.replace("\\","/");
        url.replace("//","/");
        t->insert(Qmmp::URL, url);
    }
    return tracks;
}

QList<PlayListTrack *> PlayListParser::loadPlaylist(const QString &fmt, const QByteArray &contents)
{
    foreach (PlayListFormat *p, *m_formats)
    {
        if(p->properties().shortName == fmt)
            return p->decode(contents);
    }
    return QList<PlayListTrack *>();
}

void PlayListParser::loadFormats()
{
    if (m_formats)
        return;

    m_formats = new QList<PlayListFormat*>();
    QDir pluginsDir (Qmmp::pluginsPath());
    pluginsDir.cd("PlayListFormats");
    QStringList filters;
    filters << "*.dll" << "*.so";
    foreach (QString fileName, pluginsDir.entryList(filters, QDir::Files))
    {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (loader.isLoaded())
            qDebug("PlayListParser: loaded plugin %s", qPrintable(fileName));
        else
            qWarning("PlayListParser: %s", qPrintable(loader.errorString ()));

        PlayListFormat *fmt = 0;
        if (plugin)
            fmt = qobject_cast<PlayListFormat *>(plugin);

        if (fmt)
            m_formats->append(fmt);
    }
}
