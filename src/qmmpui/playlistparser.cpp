/***************************************************************************
 *   Copyright (C) 2008-2021 by Ilya Kotov                                 *
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
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <algorithm>
#include <qmmp/qmmp.h>
#include "playlistformat.h"
#include "playlistparser.h"

QList<PlayListFormat*> *PlayListParser::m_formats = nullptr;

//key names
const QHash<QString, Qmmp::MetaData>  PlayListParser::m_metaKeys = {
    { "title", Qmmp::TITLE },
    { "artist", Qmmp::ARTIST },
    { "albumArtist", Qmmp::ALBUMARTIST },
    { "album", Qmmp::ALBUM },
    { "comment", Qmmp::COMMENT },
    { "genre", Qmmp::GENRE },
    { "composer", Qmmp::COMPOSER },
    { "year", Qmmp::YEAR },
    { "track", Qmmp::TRACK },
    { "disk", Qmmp::DISCNUMBER }
};

const QHash<QString, Qmmp::TrackProperty>  PlayListParser::m_propKeys = {
    { "bitrate", Qmmp::BITRATE },
    { "samplerate", Qmmp::SAMPLERATE },
    { "channels", Qmmp::CHANNELS },
    { "bitsPerSample", Qmmp::BITS_PER_SAMPLE },
    { "formatName", Qmmp::FORMAT_NAME },
    { "decoder", Qmmp::DECODER },
    { "fileSize", Qmmp::FILE_SIZE }
};

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

QByteArray PlayListParser::serialize(const QList<PlayListTrack *> &tracks)
{
    QJsonArray array;
    for(const PlayListTrack *t : qAsConst(tracks))
    {
        QJsonObject obj;
        QString value;
        for(QHash<QString, Qmmp::MetaData>::const_iterator it = m_metaKeys.constBegin(); it != m_metaKeys.constEnd(); ++it)
        {
            if(!(value = t->value(it.value())).isEmpty())
                obj.insert(it.key(), value);
        }

        for(QHash<QString, Qmmp::TrackProperty>::const_iterator it = m_propKeys.constBegin(); it != m_propKeys.constEnd(); ++it)
        {
            if(!(value = t->value(it.value())).isEmpty())
                obj.insert(it.key(), value);
        }

        obj.insert("path", t->path());
        obj.insert("duration", t->duration());
        array.append(obj);
    }

    return QJsonDocument(array).toJson(QJsonDocument::Compact);
}

QList<PlayListTrack *> PlayListParser::deserialize(const QByteArray &json)
{
    QList<PlayListTrack *> out;

    QJsonDocument document = QJsonDocument::fromJson(json);
    if(!document.isArray())
    {
        qWarning("PlayListParser: invalid JSON array");
        return out;
    }

    QJsonArray array = document.array();
    for(QJsonArray::const_iterator it = array.cbegin(); it != array.cend(); ++it)
    {
        if(!(*it).isObject())
            continue;

        QJsonObject obj = (*it).toObject();

        if(obj.value("path").isNull())
            continue;

        PlayListTrack *t = new PlayListTrack();
        t->setPath(obj.value("path").toString());
        t->setDuration(obj.value("duration").toDouble());

        Qmmp::MetaData metaKey;
        Qmmp::TrackProperty propKey;

        for(QJsonObject::const_iterator i = obj.constBegin(); i != obj.constEnd(); ++i)
        {
            if((metaKey = m_metaKeys.value(i.key(), Qmmp::UNKNOWN)) != Qmmp::UNKNOWN)
                t->setValue(metaKey, i.value().toString());
            else if((propKey = m_propKeys.value(i.key(), Qmmp::UNKNOWN_PROPERTY)) != Qmmp::UNKNOWN_PROPERTY)
                t->setValue(propKey, i.value().toString());
        }

        out << t;
    }

    return out;
}
