/***************************************************************************
 *   Copyright (C) 2009-2019 by Ilya Kotov                                 *
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

#include <QByteArray>
#include <QFile>
#include <QBuffer>
#include <QDir>
#include <QTimer>
#include <QSettings>
#include <QMap>
#include <QSaveFile>
#include <qmmp/trackinfo.h>
#include "qmmpuisettings.h"
#include "playlistmanager.h"

PlayListManager *PlayListManager::m_instance = nullptr;

PlayListManager::PlayListManager(QObject *parent) : QObject(parent)
{
    if(m_instance)
        qFatal("PlayListManager: only one instance is allowed");
    qRegisterMetaType<PlayListModel::SortMode>();
    m_instance = this;
    m_ui_settings = QmmpUiSettings::instance();
    m_header = new PlayListHeaderModel(this);
    m_current = nullptr;
    m_selected = nullptr;
    m_timer = new QTimer(this);
    m_timer->setInterval(5000);
    m_timer->setSingleShot(true);
    connect(m_timer, SIGNAL(timeout()), SLOT(writePlayLists()));
    //key names
    m_metaKeys.insert("title", Qmmp::TITLE);
    m_metaKeys.insert("artist", Qmmp::ARTIST);
    m_metaKeys.insert("albumartist", Qmmp::ALBUMARTIST);
    m_metaKeys.insert("album", Qmmp::ALBUM);
    m_metaKeys.insert("comment", Qmmp::COMMENT);
    m_metaKeys.insert("genre", Qmmp::GENRE);
    m_metaKeys.insert("composer", Qmmp::COMPOSER);
    m_metaKeys.insert("year", Qmmp::YEAR);
    m_metaKeys.insert("track", Qmmp::TRACK);
    m_metaKeys.insert("disk", Qmmp::DISCNUMBER);
    m_propKeys.insert("bitrate", Qmmp::BITRATE);
    m_propKeys.insert("samplerate", Qmmp::SAMPLERATE);
    m_propKeys.insert("channels", Qmmp::CHANNELS);
    m_propKeys.insert("bits_per_sample", Qmmp::BITS_PER_SAMPLE);
    m_propKeys.insert("format_name", Qmmp::FORMAT_NAME);
    m_propKeys.insert("decoder", Qmmp::DECODER);
    m_propKeys.insert("file_size", Qmmp::FILE_SIZE);
    readPlayLists(); //read playlists
}

PlayListManager::~PlayListManager()
{
    writePlayLists();
    m_instance = nullptr;
}

PlayListManager* PlayListManager::instance()
{
    return m_instance;
}

PlayListModel *PlayListManager::selectedPlayList() const
{
    return m_selected;
}

PlayListModel *PlayListManager::currentPlayList() const
{
    return m_current;
}

int PlayListManager::selectedPlayListIndex() const
{
    return indexOf(m_selected);
}

int PlayListManager::currentPlayListIndex() const
{
    return indexOf(m_current);
}

QList <PlayListModel *> PlayListManager::playLists() const
{
    return m_models;
}

QStringList PlayListManager::playListNames() const
{
    QStringList names;
    foreach(PlayListModel *model, m_models)
        names << model->name();
    return names;
}

void PlayListManager::selectPlayList(PlayListModel *model)
{
    if(model != m_selected && m_models.contains(model))
    {
        PlayListModel *prev = m_selected;
        m_selected = model;
        emit selectedPlayListChanged(model, prev);
        emit playListsChanged();
    }
}

void PlayListManager::selectPlayList(int i)
{
    if(i > m_models.count() - 1)
        return;
    selectPlayList(playListAt(i));
}

void PlayListManager::selectPlayList(const QString &name)
{
    int index = playListNames().indexOf(name);
    if(index >= 0)
        selectPlayList(playListAt(index));
}

void PlayListManager::selectNextPlayList()
{
    int i = m_models.indexOf(m_selected);
    i++;
    if( i >= 0 && i < m_models.size())
        selectPlayList(i);
}

void PlayListManager::selectPreviousPlayList()
{
    int i = m_models.indexOf(m_selected);
    i--;
    if( i >= 0 && i < m_models.size())
        selectPlayList(i);
}

void PlayListManager::activatePlayList(PlayListModel *model)
{
    if(model != m_current && m_models.contains(model))
    {
        PlayListModel *prev = m_current;
        m_current = model;
        emit currentPlayListChanged(model, prev);
        emit playListsChanged();
    }
}

void PlayListManager::activatePlayList(int index)
{
    activatePlayList(playListAt(index));
}

PlayListModel *PlayListManager::createPlayList(const QString &name)
{
    PlayListModel *model = new PlayListModel (name.isEmpty() ? tr("Playlist") : name, this);
    QString pl_name = model->name();
    if(playListNames().contains(pl_name))
    {
        int i = 0;
        forever
        {
            i++;
            if(!playListNames().contains(pl_name + QString(" (%1)").arg(i)))
            {
                pl_name = pl_name + QString(" (%1)").arg(i);
                break;
            }
        }
        model->setName(pl_name);
    }
    m_models.append(model);
    connect(model, SIGNAL(nameChanged(QString)), SIGNAL(playListsChanged()));
    connect(model, SIGNAL(listChanged(int)), SLOT(onListChanged(int)));
    emit playListAdded(m_models.indexOf(model));
    selectPlayList(model);
    return model;
}

void PlayListManager::removePlayList(PlayListModel *model)
{
     if(m_models.count() < 2 || !m_models.contains(model))
        return;

     int i = m_models.indexOf(model);

     if(m_current == model)
     {
         m_current = m_models.at((i > 0) ? (i - 1) : (i + 1));
         emit currentPlayListChanged(m_current, model);
     }
     if(m_selected == model)
     {
         m_selected = m_models.at((i > 0) ? (i - 1) : (i + 1));
         emit selectedPlayListChanged(m_selected, model);
     }
     m_models.removeAt(i);
     model->deleteLater();
     emit playListRemoved(i);
     emit playListsChanged();
}

void PlayListManager::removePlayList(int index)
{
    removePlayList(playListAt(index));
}

void PlayListManager::move(int i, int j)
{
    if(i < 0 || j < 0 || i == j)
        return;
    if(i < m_models.count() && j < m_models.count())
    {
        m_models.move(i,j);
        emit playListMoved(i,j);
        emit playListsChanged();
    }
}

int PlayListManager::count() const
{
    return m_models.count();
}

int PlayListManager::indexOf(PlayListModel *model) const
{
    return m_models.indexOf(model);
}

PlayListModel *PlayListManager::playListAt(int i) const
{
    if(i >= 0 && i < m_models.count())
        return m_models.at(i);
    return nullptr;
}

PlayListHeaderModel *PlayListManager::headerModel()
{
    return m_header;
}

void PlayListManager::readPlayLists()
{
    Qmmp::MetaData metaKey;
    Qmmp::TrackProperty propKey;
    QString line, key, value;
    int s = 0, current = 0, pl = 0;
    QList <PlayListTrack *> tracks;
    QFile file(Qmmp::configDir() + "/playlist.txt");
    file.open(QIODevice::ReadOnly);
    QByteArray array = file.readAll();
    file.close();
    QBuffer buffer(&array);
    buffer.open(QIODevice::ReadOnly);

    while (!buffer.atEnd())
    {
        line = QString::fromUtf8(buffer.readLine().constData()).trimmed();
        if ((s = line.indexOf("=")) < 0)
            continue;

        key = line.left(s);
        value = line.right(line.size() - s - 1);

        if(key == "current_playlist")
            pl = value.toInt();
        else if(key == "playlist")
        {
            if(!m_models.isEmpty() && !tracks.isEmpty())
            {
                m_models.last()->add(tracks);
                m_models.last()->setCurrent(tracks.at(qBound(0, current, tracks.count()-1)));
            }
            tracks.clear();
            current = 0;
            m_models << new PlayListModel(value, this);
        }
        else if (key == "current")
        {
            current = value.toInt();
        }
        else if (key == "file")
        {
            tracks << new PlayListTrack();
            tracks.last()->setPath(value);
        }
        else if (tracks.isEmpty())
            continue;
        else if (key == "duration")
            tracks.last()->setDuration(value.toInt());
        else if (key == "length")
            tracks.last()->setDuration(value.toInt() * 1000);
        else if((metaKey = m_metaKeys.value(key, Qmmp::UNKNOWN)) != Qmmp::UNKNOWN)
            tracks.last()->setValue(metaKey, value);
        else if((propKey = m_propKeys.value(key, Qmmp::UNKNOWN_PROPERTY)) != Qmmp::UNKNOWN_PROPERTY)
            tracks.last()->setValue(propKey, value);
    }
    buffer.close();
    if(m_models.isEmpty())
    {
        m_models << new PlayListModel(tr("Playlist"),this);
    }
    else if(!tracks.isEmpty())
    {
        m_models.last()->add(tracks);
        m_models.last()->setCurrent(tracks.at(qBound(0, current, tracks.count()-1)));
    }
    if(pl < 0 || pl >= m_models.count())
        pl = 0;
    m_selected = m_models.at(pl);
    m_current = m_models.at(pl);
    foreach(PlayListModel *model, m_models)
    {
        connect(model, SIGNAL(nameChanged(QString)), SIGNAL(playListsChanged()));
        connect(model, SIGNAL(listChanged(int)), SLOT(onListChanged(int)));
    }
}

void PlayListManager::writePlayLists()
{
    qDebug("PlayListManager: saving playlists...");
    QString value;
    QString plFilePath = Qmmp::configDir() + "/playlist.txt";
    QSaveFile plFile(plFilePath);
    if(!plFile.open(QIODevice::WriteOnly))
    {
        qDebug("PlayListManager: error: %s", qPrintable(plFile.errorString()));
        return;
    }
    plFile.write(QString("current_playlist=%1\n").arg(m_models.indexOf(m_current)).toUtf8());
    foreach(PlayListModel *model, m_models)
    {
        plFile.write(QString("playlist=%1\n").arg(model->name()).toUtf8());
        if(model->isEmpty())
            continue;
        QList<PlayListItem *> items = model->items();
        plFile.write(QString("current=%1\n").arg(model->indexOfTrack(model->currentIndex())).toUtf8());
        foreach(PlayListItem* m, items)
        {
            if(m->isGroup())
                continue;
            PlayListTrack *t = dynamic_cast<PlayListTrack *>(m);
            plFile.write(QString("file=%1\n").arg(t->path()).toUtf8());

            foreach(Qmmp::MetaData metaKey, m_metaKeys.values())
            {
                if(!(value = t->value(metaKey)).isEmpty())
                    plFile.write(QString("%1=%2\n").arg(m_metaKeys.key(metaKey)).arg(value).toUtf8());
            }

            foreach(Qmmp::TrackProperty propKey, m_propKeys.values())
            {
                if(!(value = t->value(propKey)).isEmpty())
                    plFile.write(QString("%1=%2\n").arg(m_propKeys.key(propKey)).arg(value).toLatin1());
            }

            if(t->duration() > 0)
                plFile.write(QString("duration=%1\n").arg(t->duration()).toLatin1());
        }
    }
    plFile.commit();
}

void PlayListManager::onListChanged(int flags)
{
    if((flags & PlayListModel::STRUCTURE) && m_ui_settings->autoSavePlayList())
        m_timer->start();
}

void PlayListManager::clear()
{
    m_selected->clear();
}

void PlayListManager::clearSelection()
{
    m_selected->clearSelection();
}

void PlayListManager::removeSelected()
{
    m_selected->removeSelected();
}

void PlayListManager::removeUnselected()
{
    m_selected->removeUnselected();
}

void PlayListManager::removeTrack (int i)
{
    m_selected->removeTrack(i);
}

void PlayListManager::removeTrack (PlayListTrack *track)
{
    m_selected->removeTrack(track);
}

void PlayListManager::invertSelection()
{
    m_selected->invertSelection();
}

void PlayListManager::selectAll()
{
    m_selected->selectAll();
}

void PlayListManager::showDetails()
{
    m_selected->showDetails();
}

void PlayListManager::add(const QString &path)
{
    m_selected->add(path);
}

void PlayListManager::add(const QStringList &paths)
{
    m_selected->add(paths);
}

void PlayListManager::randomizeList()
{
    m_selected->randomizeList();
}

void PlayListManager::reverseList()
{
    m_selected->reverseList();
}

void PlayListManager::sortSelection(PlayListModel::SortMode mode)
{
    m_selected->sortSelection(mode);
}

void PlayListManager::sort(PlayListModel::SortMode mode)
{
    m_selected->sort(mode);
}

void PlayListManager::addToQueue()
{
    m_selected->addToQueue();
}

void PlayListManager::removeInvalidTracks()
{
    m_selected->removeInvalidTracks();
}

void PlayListManager::removeDuplicates()
{
    m_selected->removeDuplicates();
}

void PlayListManager::refresh()
{
    m_selected->refresh();
}

void PlayListManager::clearQueue()
{
    m_selected->clearQueue();
}

void PlayListManager::stopAfterSelected()
{
    m_selected->stopAfterSelected();
}
