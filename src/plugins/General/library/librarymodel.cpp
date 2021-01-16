/***************************************************************************
 *   Copyright (C) 2020-2021 by Ilya Kotov                                 *
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

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QtDebug>
#include <QMimeData>
#include <QHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <qmmp/qmmp.h>
#include <qmmpui/playlistparser.h>
#include "librarymodel.h"

#define CONNECTION_NAME "qmmp_library_view"

class LibraryTreeItem
{
public:
    LibraryTreeItem() {}
    ~LibraryTreeItem()
    {
        clear();
    }

    void clear()
    {
        name.clear();
        type = Qmmp::UNKNOWN;
        parent = nullptr;
        qDeleteAll(children);
        children.clear();
    }

    QString name;
    Qmmp::MetaData type = Qmmp::UNKNOWN;
    QList<LibraryTreeItem *> children;
    LibraryTreeItem *parent = nullptr;
};

LibraryModel::LibraryModel(QObject *parent) : QAbstractItemModel(parent)
{
    m_rootItem = new LibraryTreeItem;
    refresh();
}

LibraryModel::~LibraryModel()
{
    delete m_rootItem;

    if(QSqlDatabase::contains(CONNECTION_NAME))
    {
        QSqlDatabase::database(CONNECTION_NAME).close();
        QSqlDatabase::removeDatabase(CONNECTION_NAME);
    }
}

Qt::ItemFlags LibraryModel::flags(const QModelIndex &index) const
{
    if(index.isValid())
        return QAbstractItemModel::flags(index) | Qt::ItemIsDragEnabled;
    else
        return QAbstractItemModel::flags(index);
}

QStringList LibraryModel::mimeTypes() const
{
    return QStringList("application/json");
}

QMimeData *LibraryModel::mimeData(const QModelIndexList &indexes) const
{
    QList<PlayListTrack *> tracks;

    for(const QModelIndex &index : indexes)
    {
        if(index.isValid())
        {
            tracks << getTracks(index);
        }
    }

    if(!tracks.isEmpty())
    {
        QMimeData *mimeData = new QMimeData;
        mimeData->setData("application/json", PlayListParser::serialize(tracks));
        qDeleteAll(tracks);
        return mimeData;
    }

    return nullptr;
}

bool LibraryModel::canFetchMore(const QModelIndex &parent) const
{
    if(!parent.isValid())
        return false;

    LibraryTreeItem *parentItem = static_cast<LibraryTreeItem *>(parent.internalPointer());
    if(parentItem == m_rootItem || parentItem->type == Qmmp::TITLE)
        return false;
    else
        return parentItem->children.isEmpty();
}

void LibraryModel::fetchMore(const QModelIndex &parent)
{
    if(!parent.isValid())
        return;

    LibraryTreeItem *parentItem = static_cast<LibraryTreeItem *>(parent.internalPointer());

    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    if(!db.isOpen())
        return;

    if(parentItem->type == Qmmp::ARTIST)
    {
        QSqlQuery query(db);
        if(m_filter.isEmpty())
        {
            query.prepare("SELECT DISTINCT Album from track_library WHERE Artist = :artist");
        }
        else
        {
            query.prepare("SELECT DISTINCT Album from track_library WHERE Artist = :artist AND SearchString LIKE :filter");
            query.bindValue(":filter", QString("%%1%").arg(m_filter.toLower()));
        }
        query.bindValue(":artist", parentItem->name);

        if(!query.exec())
        {
            qWarning("Library: exec error: %s", qPrintable(query.lastError().text()));
            return;
        }

        while(query.next())
        {
            LibraryTreeItem *item = new LibraryTreeItem;
            item->name = query.value("Album").toString();
            item->type = Qmmp::ALBUM;
            item->parent = parentItem;
            parentItem->children << item;
            qDebug() << parentItem->name << item->name;
        }
    }
    else if(parentItem->type == Qmmp::ALBUM)
    {
        QSqlQuery query(db);
        if(m_filter.isEmpty())
        {
            query.prepare("SELECT Title from track_library WHERE Artist = :artist AND Album = :album");
        }
        else
        {
            query.prepare("SELECT Title from track_library WHERE Artist = :artist AND Album = :album "
                          "AND SearchString LIKE :filter");
            query.bindValue(":filter", QString("%%1%").arg(m_filter.toLower()));
        }
        query.bindValue(":artist", parentItem->parent->name);
        query.bindValue(":album", parentItem->name);

        if(!query.exec())
        {
            qWarning("Library: exec error: %s", qPrintable(query.lastError().text()));
            return;
        }

        while(query.next())
        {
            LibraryTreeItem *item = new LibraryTreeItem;
            item->name = query.value("Title").toString();
            item->type = Qmmp::TITLE;
            item->parent = parentItem;
            parentItem->children << item;
        }
    }
}

QVariant LibraryModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    QString name = static_cast<LibraryTreeItem *>(index.internalPointer())->name;
    return name.isEmpty() ? tr("Unknown") : name;
}

QModelIndex LibraryModel::parent(const QModelIndex &child) const
{
    if(!child.isValid())
        return QModelIndex();

    LibraryTreeItem *childItem = static_cast<LibraryTreeItem *>(child.internalPointer());
    LibraryTreeItem *parentItem = childItem->parent;

    if(parentItem == m_rootItem || !parentItem || !parentItem->parent)
        return QModelIndex();

    return createIndex(parentItem->parent->children.indexOf(parentItem), 0, parentItem);
}

QModelIndex LibraryModel::index(int row, int column, const QModelIndex &parent) const
{
    if(parent.isValid() && parent.column() != 0)
        return QModelIndex();

    LibraryTreeItem *parentItem = parent.isValid() ? static_cast<LibraryTreeItem *>(parent.internalPointer()) :
                                                     m_rootItem;

    if(row >= 0 && row < parentItem->children.count())
        return createIndex(row, column, parentItem->children.at(row));
    else
        return QModelIndex();
}

int LibraryModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

int LibraryModel::rowCount(const QModelIndex &parent) const
{
    if(!parent.isValid())
        return m_rootItem->children.count();

    LibraryTreeItem *parentItem = static_cast<LibraryTreeItem *>(parent.internalPointer());
    if(parentItem->type == Qmmp::TITLE)
        return 0;
    else
        return qMax(1, parentItem->children.count());
}

void LibraryModel::setFilter(const QString &filter)
{
    m_filter = filter;
}

void LibraryModel::refresh()
{
    beginResetModel();
    m_rootItem->clear();

    QSqlDatabase db;

    if(QSqlDatabase::contains(CONNECTION_NAME))
    {
        db = QSqlDatabase::database(CONNECTION_NAME);
    }
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE", CONNECTION_NAME);
        db.setDatabaseName(Qmmp::configDir() + "/" + "library.sqlite");
        db.open();
    }

    if(!db.isOpen())
    {
        endResetModel();
        return;
    }

    QSqlQuery query(db);
    if(m_filter.isEmpty())
    {
        query.prepare("SELECT DISTINCT Artist from track_library ORDER BY Artist");
    }
    else
    {
        query.prepare("SELECT DISTINCT Artist from track_library WHERE SearchString LIKE :filter ORDER BY Artist");
        query.bindValue(":filter", QString("%%1%").arg(m_filter.toLower()));
    }

    if(!query.exec())
        qWarning("Library: exec error: %s", qPrintable(query.lastError().text()));

    while(query.next())
    {
        LibraryTreeItem *item = new LibraryTreeItem;
        item->name = query.value("Artist").toString();
        item->type = Qmmp::ARTIST;
        item->parent = m_rootItem;
        m_rootItem->children << item;
    }
    endResetModel();
}

QList<PlayListTrack *> LibraryModel::getTracks(const QModelIndex &index) const
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    QList<PlayListTrack *> tracks;
    if(!db.isOpen())
        return tracks;

    const LibraryTreeItem *item = static_cast<const LibraryTreeItem *>(index.internalPointer());

    if(item->type == Qmmp::TITLE)
    {
        QSqlQuery query(db);
        query.prepare("SELECT * from track_library WHERE Artist = :artist AND Album = :album AND Title = :title");
        query.bindValue(":artist", item->parent->parent->name);
        query.bindValue(":album", item->parent->name);
        query.bindValue(":title", item->name);

        if(!query.exec())
        {
            qWarning("Library: exec error: %s", qPrintable(query.lastError().text()));
            return tracks;
        }

        if(query.next())
        {
            tracks << createTrack(query);
        }
    }
    else if(item->type == Qmmp::ALBUM)
    {
        QSqlQuery query(db);
        query.prepare("SELECT * from track_library WHERE Artist = :artist AND Album = :album");
        query.bindValue(":artist", item->parent->name);
        query.bindValue(":album", item->name);

        if(!query.exec())
        {
            qWarning("Library: exec error: %s", qPrintable(query.lastError().text()));
            return tracks;
        }

        while(query.next())
        {
           tracks << createTrack(query);
        }
    }
    else if(item->type == Qmmp::ARTIST)
    {
        QSqlQuery query(db);
        query.prepare("SELECT * from track_library WHERE Artist = :artist");
        query.bindValue(":artist", item->name);

        if(!query.exec())
        {
            qWarning("Library: exec error: %s", qPrintable(query.lastError().text()));
            return tracks;
        }

        while(query.next())
        {
            tracks << createTrack(query);
        }
    }

    return tracks;
}

PlayListTrack *LibraryModel::createTrack(const QSqlQuery &query) const
{
    static const QHash<int, QString> metaColumns = {
        { Qmmp::TITLE, "Title" },
        { Qmmp::ARTIST, "Artist" },
        { Qmmp::ALBUMARTIST, "AlbumArtist" },
        { Qmmp::ALBUM, "Album" },
        { Qmmp::COMMENT, "Comment" },
        { Qmmp::GENRE, "Genre" },
        { Qmmp::COMPOSER, "Composer" },
        { Qmmp::YEAR, "Year" },
        { Qmmp::TRACK, "Track" },
        { Qmmp::DISCNUMBER, "DiscNumber" }
    };

    PlayListTrack *track = new PlayListTrack;
    track->setPath(query.value("URL").toString());
    track->setDuration(query.value("Duration").toLongLong());

    for(int key = Qmmp::TITLE; key <= Qmmp::DISCNUMBER; ++key)
    {
       QString value = query.value(metaColumns.value(key)).toString();
       track->setValue(static_cast<Qmmp::MetaData>(key), value);
    }

    QJsonDocument document = QJsonDocument::fromJson(query.value("AudioInfo").toByteArray());
    QJsonObject obj = document.object();
    track->setValue(Qmmp::BITRATE, obj.value("bitrate").toInt());
    track->setValue(Qmmp::SAMPLERATE, obj.value("samplerate").toInt());
    track->setValue(Qmmp::CHANNELS, obj.value("channels").toInt());
    track->setValue(Qmmp::BITS_PER_SAMPLE, obj.value("bitsPerSample").toInt());
    track->setValue(Qmmp::FORMAT_NAME, obj.value("formatName").toString());
    track->setValue(Qmmp::DECODER, obj.value("decoder").toString());
    track->setValue(Qmmp::FILE_SIZE, qint64(obj.value("fileSize").toDouble()));
    return track;
}
