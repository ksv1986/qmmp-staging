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

#include <QPluginLoader>
#include <QList>
#include <QDir>
#include <QApplication>
#include <algorithm>
#include <qmmp/qmmp.h>
#include "playlistformat.h"
#include "playlistparser.h"

QList<PlayListFormat*> *PlayListParser::m_formats = nullptr;

QList<PlayListFormat *> PlayListParser::formats()
{
    loadFormats();
    return *m_formats;
}

QStringList PlayListParser::nameFilters()
{
    loadFormats();
    QStringList filters;
    for(const PlayListFormat *format : qAsConst(*m_formats))
    {
        filters << format->properties().filters;
    }
    return filters;
}

QStringList PlayListParser::filters()
{
    loadFormats();
    QStringList filters;
    for(const PlayListFormat *format : qAsConst(*m_formats))
    {
        if (!format->properties().filters.isEmpty())
            filters << format->properties().shortName.toUpper() + " (" + format->properties().filters.join(" ") + ")";
    }
    return filters;
}

bool PlayListParser::isPlayList(const QString &url)
{
    return QDir::match(nameFilters(), url.section(QChar('/'), -1));
}

PlayListFormat *PlayListParser::findByMime(const QString &mime)
{
    loadFormats();
    auto it = std::find_if(m_formats->cbegin(), m_formats->cend(),
                           [mime](PlayListFormat *format) { return format->properties().contentTypes.contains(mime); } );
    return it == m_formats->cend() ? nullptr : *it;
}

PlayListFormat *PlayListParser::findByPath(const QString &filePath)
{
    loadFormats();
    for(PlayListFormat *format : qAsConst(*m_formats))
    {
        if(QDir::match(format->properties().filters, filePath.section(QChar('/'), -1)))
            return format;
    }
    return nullptr;
}

PlayListFormat *PlayListParser::findByUrl(const QUrl &url)
{
    QString path = url.path(QUrl::FullyEncoded);
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

    const QList<PlayListTrack*> tracks = prs->decode(file.readAll());

    if(tracks.isEmpty())
    {
        qWarning("PlayListParser: error opening %s",qPrintable(f_name));
        return tracks;
    }

    QString path;
    for(PlayListTrack *t : qAsConst(tracks))
    {
        path = t->path();

        if(path.contains("://"))
            continue;

        if(QFileInfo(path).isRelative())
            path.prepend(QFileInfo(f_name).canonicalPath () + "/");

        path.replace("\\","/");
        path.replace("//","/");
        t->setPath(path);
    }
    return tracks;
}

QList<PlayListTrack *> PlayListParser::loadPlaylist(const QString &fmt, const QByteArray &content)
{
    auto it = std::find_if(m_formats->cbegin(), m_formats->cend(),
                           [fmt](PlayListFormat *format) { return format->properties().shortName == fmt; } );
    return it == m_formats->cend() ? QList<PlayListTrack *>() : (*it)->decode(content);
}

void PlayListParser::loadFormats()
{
    if (m_formats)
        return;

    m_formats = new QList<PlayListFormat*>();
    for(const QString &filePath : Qmmp::findPlugins("PlayListFormats"))
    {
        QPluginLoader loader(filePath);
        QObject *plugin = loader.instance();
        if (loader.isLoaded())
            qDebug("PlayListParser: loaded plugin %s", qPrintable(QFileInfo(filePath).filePath()));
        else
            qWarning("PlayListParser: %s", qPrintable(loader.errorString ()));

        PlayListFormat *fmt = nullptr;
        if (plugin)
            fmt = qobject_cast<PlayListFormat *>(plugin);

        if (fmt)
            m_formats->append(fmt);
    }
}
