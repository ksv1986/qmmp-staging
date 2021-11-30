/***************************************************************************
 *   Copyright (C) 2014-2021 by Ilya Kotov                                 *
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
#include <QDateTime>
#include <QTime>
#include <algorithm>
#include <qmmp/metadatamanager.h>
#include "qmmpuisettings.h"
#include "metadatahelper_p.h"
#include "playlisttrack.h"
#include "playlisttask_p.h"

struct TrackField
{
    PlayListTrack *track;
    QString value;
    QString groupName;
};

struct GroupdField
{
    QList <TrackField *> fields;
    QString groupName;
};

////===============THE BEGINNING OF SORT IMPLEMENTATION =======================////

// First we'll implement bundle of static compare procedures
// to sort items in different ways

//by string
static bool _stringLessComparator(TrackField* s1, TrackField* s2)
{
    return QString::localeAwareCompare (s1->value, s2->value) < 0;
}

static bool _stringGreaterComparator(TrackField* s1, TrackField* s2)
{
    return QString::localeAwareCompare (s1->value, s2->value) > 0;
}
//by number
static bool _numberLessComparator(TrackField* s1, TrackField* s2)
{
    return s1->value.toInt() < s2->value.toInt();
}

static bool _numberGreaterComparator(TrackField* s1, TrackField* s2)
{
    return s1->value.toInt() > s2->value.toInt();
}
//by file creation date
static bool _fileCreationDateLessComparator(TrackField* s1, TrackField* s2)
{
    return QFileInfo(s1->value).birthTime() < QFileInfo(s2->value).birthTime();
}

static bool _fileCreationDateGreaterComparator(TrackField* s1, TrackField* s2)
{
    return QFileInfo(s1->value).birthTime() > QFileInfo(s2->value).birthTime();
}
//by file modification date
static bool _fileModificationDateLessComparator(TrackField* s1, TrackField* s2)
{
    return QFileInfo(s1->value).lastModified() < QFileInfo(s2->value).lastModified();
}

static bool _fileModificationDateGreaterComparator(TrackField* s1, TrackField* s2)
{
    return QFileInfo(s1->value).lastModified() > QFileInfo(s2->value).lastModified();
}
//by file name
static bool _filenameLessComparator(TrackField* s1, TrackField* s2)
{
    QFileInfo i_s1(s1->value);
    QFileInfo i_s2(s2->value);
    return QString::localeAwareCompare (i_s1.baseName(), i_s2.baseName()) < 0;
}

static bool _filenameGreaterComparator(TrackField* s1, TrackField* s2)
{
    QFileInfo i_s1(s1->value);
    QFileInfo i_s2(s2->value);
    return QString::localeAwareCompare (i_s1.baseName(), i_s2.baseName()) > 0;
}
////=============== THE END OF SORT IMPLEMENTATION =======================////

PlayListTask::PlayListTask(QObject *parent) : QThread(parent)
{
    m_sort_keys = {
        { PlayListModel::TITLE, Qmmp::TITLE },
        { PlayListModel::DISCNUMBER, Qmmp::DISCNUMBER },
        { PlayListModel::ALBUM, Qmmp::ALBUM },
        { PlayListModel::ARTIST, Qmmp::ARTIST },
        { PlayListModel::ALBUMARTIST, Qmmp::ALBUMARTIST },
        { PlayListModel::FILENAME, Qmmp::UNKNOWN },
        { PlayListModel::PATH_AND_FILENAME, Qmmp::UNKNOWN },
        { PlayListModel::DATE, Qmmp::YEAR },
        { PlayListModel::TRACK, Qmmp::TRACK },
        { PlayListModel::FILE_CREATION_DATE, Qmmp::UNKNOWN },
        { PlayListModel::FILE_MODIFICATION_DATE, Qmmp::UNKNOWN }
    };
}

PlayListTask::~PlayListTask()
{
    clear();
}

void PlayListTask::sort(QList<PlayListTrack *> tracks, PlayListModel::SortMode mode)
{
    if(isRunning())
        return;
    clear();
    m_reverted = !m_reverted;
    m_sort_mode = mode;
    m_task = SORT;
    m_input_tracks = tracks;
    Qmmp::MetaData key = m_sort_keys.value(mode);

    m_align_groups = QmmpUiSettings::instance()->isGroupsEnabled() && (mode != PlayListModel::GROUP);

    for(PlayListTrack *t : qAsConst(tracks))
    {
        TrackField *f = new TrackField;
        f->track = t;
        if(mode == PlayListModel::GROUP)
            f->value = t->groupName();
        else if(key == Qmmp::UNKNOWN)
            f->value = t->path();
        else
            f->value = t->value(key);
        if(m_align_groups)
            f->groupName = t->groupName();
        m_fields.append(f);
    }

    start();
}

void PlayListTask::sortSelection(QList<PlayListTrack *> tracks, PlayListModel::SortMode mode)
{
    if(isRunning())
        return;
    clear();
    m_reverted = !m_reverted;
    m_sort_mode = mode;
    m_task = SORT_SELECTION;
    m_tracks = tracks;
    m_input_tracks = tracks;
    Qmmp::MetaData key = m_sort_keys.value(mode);

    for(int i = 0; i < tracks.count(); ++i)
    {
        if(!tracks[i]->isSelected())
            continue;

        TrackField *f = new TrackField;
        f->track = tracks[i];
        if(mode == PlayListModel::GROUP)
            f->value = f->track->groupName();
        else if(key == Qmmp::UNKNOWN)
            f->value = f->track->path();
        else
            f->value = f->track->value(key);
        m_fields.append(f);
        m_indexes.append(i);
    }

    start();
}

void PlayListTask::sortByColumn(QList<PlayListTrack *> tracks, int column)
{
    if(isRunning())
        return;
    clear();
    m_reverted = !m_reverted;
    m_task = SORT_BY_COLUMN;
    m_input_tracks = tracks;
    m_column = column;
    if(MetaDataHelper::instance()->titleFormatter(column)->pattern() == "%n")
        m_sort_mode = PlayListModel::TRACK;
    else
        m_sort_mode = PlayListModel::TITLE;

    for(int i = 0; i < tracks.count(); ++i)
    {
        TrackField *f = new TrackField;
        f->track = tracks[i];
        f->value = f->track->formattedTitle(column);
        m_fields.append(f);
    }
    MetaDataManager::instance()->prepareForAnotherThread();
    start();
}

void PlayListTask::removeInvalidTracks(QList<PlayListTrack *> tracks, PlayListTrack *current_track)
{
    if(isRunning())
        return;
    clear();
    m_task = REMOVE_INVALID;
    m_input_tracks = tracks;
    m_tracks = tracks;
    m_current_track = current_track;

    for(int i = 0; i < tracks.count(); ++i)
    {
        TrackField *f = new TrackField;
        f->track = tracks[i];
        f->value = f->track->path();
        m_fields.append(f);
    }
    MetaDataManager::instance()->prepareForAnotherThread();
    start();
}

void PlayListTask::removeDuplicates(QList<PlayListTrack *> tracks, PlayListTrack *current_track)
{
    if(isRunning())
        return;
    clear();
    m_task = REMOVE_DUPLICATES;
    m_input_tracks = tracks;
    m_tracks = tracks;
    m_current_track = current_track;

    for(int i = 0; i < tracks.count(); ++i)
    {
        TrackField *f = new TrackField;
        f->track = tracks[i];
        f->value = f->track->path();
        m_fields.append(f);
    }
    MetaDataManager::instance()->prepareForAnotherThread();
    start();
}

void PlayListTask::refresh(QList<PlayListTrack *> tracks, PlayListTrack *current_track)
{
    if(isRunning())
        return;
    clear();
    m_task = REFRESH;
    m_input_tracks = tracks;
    m_tracks = tracks;
    m_current_track = current_track;

    for(int i = 0; i < tracks.count(); ++i)
    {
        TrackField *f = new TrackField;
        f->track = tracks[i];
        f->value = f->track->path();
        m_fields.append(f);
    }
    MetaDataManager::instance()->prepareForAnotherThread();
    start();
}

void PlayListTask::run()
{
    qDebug("PlayListTask: started");

    if(m_task == SORT || m_task == SORT_SELECTION || m_task == SORT_BY_COLUMN)
    {
        bool(*compareLessFunc)(TrackField*, TrackField*) = nullptr;
        bool(*compareGreaterFunc)(TrackField*, TrackField*) = nullptr;

        QList<TrackField*>::iterator begin = m_fields.begin();
        QList<TrackField*>::iterator end = m_fields.end();

        if(m_sort_mode == PlayListModel::FILE_CREATION_DATE)
        {
            compareLessFunc = _fileCreationDateLessComparator;
            compareGreaterFunc = _fileCreationDateGreaterComparator;
        }
        else if(m_sort_mode == PlayListModel::FILE_MODIFICATION_DATE)
        {
            compareLessFunc = _fileModificationDateLessComparator;
            compareGreaterFunc = _fileModificationDateGreaterComparator;
        }
        else if(m_sort_mode == PlayListModel::TRACK || m_sort_mode == PlayListModel::DATE)
        {
            compareLessFunc = _numberLessComparator;
            compareGreaterFunc = _numberGreaterComparator;
        }
        else if(m_sort_mode == PlayListModel::FILENAME)
        {
            compareLessFunc = _filenameLessComparator;
            compareGreaterFunc = _filenameGreaterComparator;
        }
        else
        {
            compareLessFunc = _stringLessComparator;
            compareGreaterFunc = _stringGreaterComparator;
        }

        if(m_reverted)
            std::stable_sort(begin,end,compareGreaterFunc);
        else
            std::stable_sort(begin,end,compareLessFunc);

        //align track list by group name (optimization)
        if(m_align_groups)
        {
            QList<GroupdField *> groups;
            for(int i = 0; i < m_fields.count(); ++i)
            {
                bool found = false;
                for(int j = groups.count() - 1; j >= 0; j--)
                {
                    if(groups[j]->groupName == m_fields[i]->groupName)
                    {
                        groups[j]->fields.append(m_fields[i]);
                        found = true;
                        break;
                    }
                }

                if(!found)
                {
                    groups << new GroupdField;
                    groups.last()->fields.append(m_fields[i]);
                    groups.last()->groupName = m_fields[i]->groupName;
                }
            }

            m_fields.clear();
            for(int j = 0; j < groups.count(); ++j)
            {
                m_fields.append(groups[j]->fields);
            }
            qDeleteAll(groups);
            groups.clear();
        }
    }
    else if(m_task == REMOVE_INVALID)
    {
        TrackField *f = nullptr;
        bool ok = false;
        for(int i = 0; i < m_fields.count(); ++i)
        {
            f = m_fields.at(i);

            if(f->value.contains("://"))
                ok = MetaDataManager::instance()->protocols().contains(f->value.section("://",0,0)); //url
            else
                ok = MetaDataManager::instance()->supports(f->value); //local file

            if(!ok)
                m_indexes << i;
        }
    }
    else if(m_task == REMOVE_DUPLICATES)
    {
        QStringList urls;
        TrackField *f = nullptr;
        for(int i = 0; i < m_fields.count(); ++i)
        {
            f = m_fields.at(i);

            if(urls.contains(f->value))
            {
                m_indexes.append(i);
            }
            else
            {
                urls.append(f->value);
            }
        }
    }
    else if(m_task == REFRESH)
    {
        TrackField *f = nullptr;
        MetaDataManager *mm = MetaDataManager::instance();
        QStringList protocols = mm->protocols();
        QList<QRegularExpression> regExps = mm->regExps();
        bool ok = false;
        //find invalid files
        for(int i = 0; i < m_fields.count(); ++i)
        {
            f = m_fields.at(i);

            if(f->value.contains("://"))
                ok = protocols.contains(f->value.section("://",0,0)) || MetaDataManager::hasMatch(regExps, f->value) ; //url
            else
                ok = mm->supports(f->value); //local file

            if(!ok)
                m_indexes << i;
        }
        //find all directories
        QStringList dirs; QString path;
        for(int i = 0; i < m_fields.count(); ++i)
        {
            f = m_fields.at(i);

            if(f->value.contains("://")) //skip urls
                continue;

            path = QFileInfo(f->value).canonicalPath();

            if(!dirs.contains(path))
                dirs << path;
        }
        //find all files
        QFileInfoList l;
        for(const QString &p : qAsConst(dirs))
        {
            QDir dir(p);
            dir.setFilter(QDir::Files | QDir::Hidden);
            dir.setSorting(QDir::Name);
            l << dir.entryInfoList(mm->nameFilters());
        }
        //generate URLs for the current playlist
        QStringList urls;
        for(const TrackField *t : qAsConst(m_fields))
            urls.append(t->value);

        //find files that have already been added
        QList<int> indexes;
        for(int i = 0; i < l.count(); ++i)
        {
            QFileInfo info = l[i];
            if(urls.contains(info.canonicalFilePath()))
                indexes.append(i);
            else
                urls.append(info.canonicalFilePath());
        }
        //remove existing URLs
        for(int i = indexes.count() - 1; i >= 0; i--)
            l.takeAt(indexes[i]);

        //create new playlist tracks
        QStringList ignoredFiles;
        TrackInfo::Parts parts = QmmpUiSettings::instance()->useMetaData() ? TrackInfo::AllParts : TrackInfo::Parts();
        for(const QFileInfo &i : qAsConst(l))
        {
            QStringList ignored;
            for(TrackInfo *info : mm->createPlayList(i.canonicalFilePath(), parts, &ignored))
            {
                m_new_tracks << new PlayListTrack(info);
                delete info;
            }
            ignoredFiles.append(ignored);
        }
        //remove dublicate URLs and ignored files
        //this code prevents re-addition of cue tracks
        QList<PlayListTrack *>::iterator it = m_new_tracks.begin();
        while(it != m_new_tracks.end())
        {
            if(((*it)->path().contains("://") && urls.contains((*it)->path())) ||
                    ignoredFiles.contains((*it)->path()))
            {
                delete (*it);
                it = m_new_tracks.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
    qDebug("PlayListTask: finished");
}

PlayListTask::TaskType PlayListTask::type() const
{
    return m_task;
}

bool PlayListTask::isChanged(PlayListContainer *container)
{
    if(m_input_tracks.count() != container->trackCount())
        return true;

    return m_input_tracks != container->tracks();
}

QList<PlayListTrack *> PlayListTask::takeResults(PlayListTrack **current_track)
{
    if(m_task == SORT || m_task == SORT_BY_COLUMN)
    {
        for(const TrackField *f : qAsConst(m_fields))
            m_tracks.append(f->track);
    }
    else if(m_task == SORT_SELECTION)
    {
        for (int i = 0; i < m_indexes.count(); i++)
            m_tracks.replace(m_indexes[i], m_fields[i]->track);
    }
    else if(m_task == REMOVE_INVALID || m_task == REMOVE_DUPLICATES || m_task == REFRESH)
    {
        for (int i = m_indexes.count() - 1; i >= 0; i--)
        {
            int index = m_indexes.at(i);
            PlayListTrack *t = m_tracks.takeAt(index);
            if(t == m_current_track)
            {
                if(m_tracks.isEmpty())
                    m_current_track = nullptr;
                else if(index > 0 && index <= m_tracks.count())
                    m_current_track = m_tracks[index - 1];
                else
                    m_current_track = m_tracks[0];
                *current_track = m_current_track;
            }
            if(t->isUsed())
                t->deleteLater();
            else
                delete t;
        }

        if(m_task == REFRESH)
        {
            m_tracks.append(m_new_tracks);
            m_new_tracks.clear();
        }
    }
    return m_tracks;
}

int PlayListTask::column() const
{
    return m_column;
}

bool PlayListTask::isReverted() const
{
    return m_reverted;
}

void PlayListTask::clear()
{
    qDeleteAll(m_fields);
    m_fields.clear();
    qDeleteAll(m_new_tracks);
    m_new_tracks.clear();
    m_align_groups = false;
    m_indexes.clear();
    m_input_tracks.clear();
    m_tracks.clear();
    m_current_track = nullptr;
}
