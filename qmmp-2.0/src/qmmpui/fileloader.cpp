/***************************************************************************
 *   Copyright (C) 2006-2021 by Ilya Kotov                                 *
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

#include <QRegularExpression>
#include <QMetaType>
#include <QDir>
#include <QApplication>
#include <qmmp/metadatamanager.h>
#include "fileloader_p.h"
#include "qmmpuisettings.h"
#include "playlistitem.h"
#include "playlistparser.h"

FileLoader::FileLoader(QObject *parent) : QThread(parent)
{
    qRegisterMetaType<QList<PlayListTrack*> >("QList<PlayListTrack*>");
    m_settings = QmmpUiSettings::instance();
    connect(qApp, SIGNAL(aboutToQuit()), SLOT(finish()));
}

FileLoader::~FileLoader()
{}

QList<PlayListTrack *> FileLoader::processFile(const QString &path, QStringList *ignoredPaths)
{
    QList<PlayListTrack *> tracks;
    const QList<TrackInfo *> infoList = MetaDataManager::instance()->createPlayList(path, m_parts, ignoredPaths);

    for(const TrackInfo *info : qAsConst(infoList))
    {
        tracks.append(new PlayListTrack(info));
    }
    qDeleteAll(infoList);
    return tracks;
}

void FileLoader::insertPlayList(const QString &fmt, const QByteArray &contents, PlayListItem *before)
{
    QList<PlayListTrack *> tracks = PlayListParser::loadPlaylist(fmt, contents);

    while (!tracks.isEmpty() && !m_finished)
    {
        PlayListTrack *t = tracks.takeFirst();
        QList<TrackInfo *> infoList = MetaDataManager::instance()->createPlayList(t->path(), m_parts);
        if(infoList.count() != 1) //invalid or unsupported track
        {
            qDeleteAll(infoList);
            infoList.clear();
            delete t;
            continue;
        }

        TrackInfo *info = infoList.first();
        if(!info->value(Qmmp::ALBUM).isEmpty() && !info->value(Qmmp::ARTIST).isEmpty())
            t->updateMetaData(infoList.first());

        emit newTracksToInsert(before, QList<PlayListTrack *>() << t);
        delete info;
    }
    //clear remaining tracks
    qDeleteAll(tracks);
    tracks.clear();
}

void FileLoader::insertPlayList(const QString &path, PlayListItem *before)
{
    QList<PlayListTrack *> tracks = PlayListParser::loadPlaylist(path);

    if(m_readMetaDataForPlayLists)
    {
        QList<PlayListTrack *> tmp;
        while(!tracks.isEmpty() && !m_finished)
        {
            PlayListTrack *t = tracks.takeFirst();
            QList <TrackInfo *> infoList = MetaDataManager::instance()->createPlayList(t->path(), m_parts);
            if(infoList.count() != 1) //invalid or unsupported track
            {
                qDeleteAll(infoList);
                infoList.clear();
                delete t;
                continue;
            }

            if(!infoList.first()->value(Qmmp::ALBUM).isEmpty() && !infoList.first()->value(Qmmp::ARTIST).isEmpty())
                t->updateMetaData(infoList.first());
            delete infoList.takeFirst();
            tmp << t;
            if(tmp.count() > 30)
            {
                emit newTracksToInsert(before, tmp);
                tmp.clear();
            }
        }
        if(!tmp.isEmpty() && !m_finished)
        {
            emit newTracksToInsert(before, tmp);
            tmp.clear();
        }

        if(!tmp.isEmpty())
        {
            qDeleteAll(tmp);
            tmp.clear();
        }
    }
    else
    {
        QStringList protocols = MetaDataManager::instance()->protocols();
        QList<QRegularExpression> regExps =  MetaDataManager::instance()->regExps();
        QList<QRegularExpression> filters;
        for(const QString &pattern : MetaDataManager::instance()->nameFilters())
            filters << QRegularExpression(QRegularExpression::wildcardToRegularExpression(pattern), QRegularExpression::CaseInsensitiveOption);

        QList<PlayListTrack *>::iterator it = tracks.begin();
        while(it != tracks.end())
        {
            if((*it)->path().contains("://"))
            {
                if(!protocols.contains((*it)->path().section("://", 0, 0)) &&
                        !MetaDataManager::hasMatch(regExps, (*it)->path()))
                {
                    delete (*it);
                    it = tracks.erase(it);
                    continue;
                }
            }
            else if(!QFile::exists((*it)->path()) || !MetaDataManager::hasMatch(filters, (*it)->path().section(QChar('/'), -1)))
            {
                delete (*it);
                it = tracks.erase(it);
                continue;
            }

            ++it;
        }
        if(!m_finished && !tracks.isEmpty())
        {
            emit newTracksToInsert(before, tracks);
            tracks.clear();
        }
    }
    //clear remaining tracks
    qDeleteAll(tracks);
    tracks.clear();
}

void FileLoader::addDirectory(const QString& s, PlayListItem *before)
{
    QList<PlayListTrack *> tracks;
    QStringList ignoredPaths;
    QDir dir(s);
    dir.setFilter(QDir::Files | QDir::Hidden);
    dir.setSorting(QDir::Name);
    QFileInfoList l = dir.entryInfoList(m_filters);

    for(const QFileInfo &info : qAsConst(l))
    {
        if(checkRestrictFilters(info) && checkExcludeFilters(info))
        {
            QStringList paths;
            tracks.append(processFile(info.absoluteFilePath (), &paths));
            ignoredPaths.append(paths);
        }

        if (m_finished)
        {
            qDeleteAll(tracks);
            tracks.clear();
            return;
        }

        if(tracks.count() > 30) //do not send more than 30 tracks at once
        {
            removeIgnoredTracks(&tracks, ignoredPaths);
            emit newTracksToInsert(before, tracks);
            tracks.clear();
            ignoredPaths.clear();
        }
    }

    if(!tracks.isEmpty())
    {
        removeIgnoredTracks(&tracks, ignoredPaths);
        emit newTracksToInsert(before, tracks);
        ignoredPaths.clear();
    }

    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Name);
    l.clear();
    l = dir.entryInfoList();

    for (int i = 0; i < l.size(); ++i)
    {
        QFileInfo fileInfo = l.at(i);
        addDirectory(fileInfo.absoluteFilePath (), before);
        if (m_finished)
            return;
    }
}

void FileLoader::run()
{
    m_finished = false;
    m_mutex.lock();
    if(m_tasks.isEmpty())
    {
        m_mutex.unlock();
        return;
    }
    m_mutex.unlock();

    while(!m_finished)
    {
        m_mutex.lock();
        LoaderTask i = m_tasks.dequeue();
        m_mutex.unlock();
        PlayListItem *before = i.before;
        QString path = i.path;

        if(!path.isEmpty())
        {
            QFileInfo info(path);

            if(info.isDir())
            {
                addDirectory(path, before);

            }
            else if(info.isFile() && PlayListParser::isPlayList(path))
            {
                insertPlayList(path, before);
            }
            else if(info.isFile() || path.contains("://"))
            {
                QList<PlayListTrack *> tracks = processFile(path);
                if(!tracks.isEmpty())
                {
                    emit newTracksToInsert(before, tracks);
                }
            }
        }
        else if(!i.playListContent.isEmpty() && !i.playListFormat.isEmpty())
        {
            insertPlayList(i.playListFormat, i.playListContent, before);
        }

        m_mutex.lock();
        if(m_tasks.isEmpty())
        {
            m_mutex.unlock();
            break;
        }
        m_mutex.unlock();
    }
}

void FileLoader::add(const QString &path)
{
    insert(nullptr, QStringList() << path);
}

void FileLoader::add(const QStringList &paths)
{
    insert(nullptr, paths);
}

void FileLoader::addPlayList(const QString &fmt, const QByteArray &data)
{
    m_mutex.lock();
    LoaderTask task;
    task.before = nullptr;
    task.playListFormat = fmt;
    task.playListContent = data;
    m_tasks.append(task);
    m_mutex.unlock();
    if(!isRunning())
    {
        MetaDataManager::instance()->prepareForAnotherThread();
        PlayListParser::loadFormats();
        m_filters = MetaDataManager::instance()->nameFilters();
        m_parts = m_settings->useMetaData() ? TrackInfo::AllParts : TrackInfo::Parts();
        m_readMetaDataForPlayLists = m_settings->readMetaDataForPlayLists();
    }
    start(QThread::IdlePriority);
}

void FileLoader::insert(PlayListItem *before, const QString &path)
{
    insert(before, QStringList() << path);
}

void FileLoader::insert(PlayListItem *before, const QStringList &paths)
{
    m_mutex.lock();
    for(const QString &path : qAsConst(paths))
    {
        LoaderTask task;
        task.before = before;
        task.path = QDir::fromNativeSeparators(path);
        m_tasks.append(task);
    }
    m_mutex.unlock();

    if(!isRunning())
    {
        MetaDataManager::instance()->prepareForAnotherThread();
        m_filters = MetaDataManager::instance()->nameFilters();
        m_parts = m_settings->useMetaData() ? TrackInfo::AllParts : TrackInfo::Parts();
        m_readMetaDataForPlayLists = m_settings->readMetaDataForPlayLists();
    }
    start(QThread::IdlePriority);
}

void FileLoader::finish()
{
    m_finished = true;
    wait();
    m_tasks.clear();
}

bool FileLoader::checkRestrictFilters(const QFileInfo &info)
{
    if(m_settings->restrictFilters().isEmpty())
        return true;

    return QDir::match(m_settings->restrictFilters(), info.fileName());
}

bool FileLoader::checkExcludeFilters(const QFileInfo &info)
{
    if(m_settings->excludeFilters().isEmpty())
        return true;

    return !QDir::match(m_settings->excludeFilters() , info.fileName());
}

void FileLoader::removeIgnoredTracks(QList<PlayListTrack *> *tracks, const QStringList &ignoredPaths)
{
    if(ignoredPaths.isEmpty())
        return;

    QList<PlayListTrack *>::iterator it = tracks->begin();
    while(it != tracks->end())
    {
        if(ignoredPaths.contains((*it)->path()))
        {
            delete (*it);
            it = tracks->erase(it);
        }
        else
        {
            ++it;
        }
    }
}
