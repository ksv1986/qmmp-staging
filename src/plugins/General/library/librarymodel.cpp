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
#include <qmmp/qmmp.h>
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
    return QStringList("text/uri-list");
}

QMimeData *LibraryModel::mimeData(const QModelIndexList &indexes) const
{
    QList<QUrl> urls;

    for(const QModelIndex &index : indexes)
    {
        if(index.isValid())
            urls << getUrls(index);
    }

    if(!urls.isEmpty())
    {
        QMimeData *mimeData = new QMimeData;
        mimeData->setUrls(urls);
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
        query.prepare("SELECT DISTINCT Album from track_library WHERE Artist = :artist");
        query.bindValue(":artist", parentItem->name);
        bool ok = query.exec();

        if(!ok)
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
        query.prepare("SELECT Title from track_library WHERE Artist = :artist AND Album = :album");
        query.bindValue(":artist", parentItem->parent->name);
        query.bindValue(":album", parentItem->name);
        bool ok = query.exec();

        if(!ok)
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
    bool ok = query.exec("SELECT DISTINCT Artist from track_library");

    if(!ok)
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

QList<QUrl> LibraryModel::getUrls(const QModelIndex &index) const
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
    QList<QUrl> urls;
    if(!db.isOpen())
        return urls;

    const LibraryTreeItem *item = static_cast<const LibraryTreeItem *>(index.internalPointer());

    if(item->type == Qmmp::TITLE)
    {
        QSqlQuery query(db);
        query.prepare("SELECT URL from track_library WHERE Artist = :artist AND Album = :album AND Title = :title");
        query.bindValue(":artist", item->parent->parent->name);
        query.bindValue(":album", item->parent->name);
        query.bindValue(":title", item->name);

        if(!query.exec())
        {
            qWarning("Library: exec error: %s", qPrintable(query.lastError().text()));
            return urls;
        }

        if(query.next())
        {
            QString path = query.value("URL").toString();
            urls << (path.contains("://") ? QUrl(path) : QUrl::fromLocalFile(path));
        }
    }
    else if(item->type == Qmmp::ALBUM)
    {
        QSqlQuery query(db);
        query.prepare("SELECT URL from track_library WHERE Artist = :artist AND Album = :album");
        query.bindValue(":artist", item->parent->name);
        query.bindValue(":album", item->name);

        if(!query.exec())
        {
            qWarning("Library: exec error: %s", qPrintable(query.lastError().text()));
            return urls;
        }

        while(query.next())
        {
            QString path = query.value("URL").toString();
            urls << (path.contains("://") ? QUrl(path) : QUrl::fromLocalFile(path));
        }
    }
    else if(item->type == Qmmp::ARTIST)
    {
        QSqlQuery query(db);
        query.prepare("SELECT URL from track_library WHERE Artist = :artist");
        query.bindValue(":artist", item->name);

        if(!query.exec())
        {
            qWarning("Library: exec error: %s", qPrintable(query.lastError().text()));
            return urls;
        }

        while(query.next())
        {
            QString path = query.value("URL").toString();
            urls << (path.contains("://") ? QUrl(path) : QUrl::fromLocalFile(path));
        }
    }

    return urls;
}
