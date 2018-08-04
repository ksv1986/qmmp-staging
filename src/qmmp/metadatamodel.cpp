/***************************************************************************
 *   Copyright (C) 2009-2018 by Ilya Kotov                                 *
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

#include "metadatamodel.h"

MetaDataItem::MetaDataItem(const QString &name, const QVariant &value, const QString &suffix) :
    m_name(name), m_suffix(suffix), m_value(value)
{}

const QString &MetaDataItem::name() const
{
    return m_name;
}

void MetaDataItem::setName(const QString &name)
{
    m_name = name;
}

const QVariant &MetaDataItem::value() const
{
    return m_value;
}

void MetaDataItem::setValue(const QString &value)
{
    m_value = value;
}

const QString &MetaDataItem::suffix() const
{
    return m_suffix;
}

void MetaDataItem::setSuffix(const QString &suffix)
{
    m_suffix = suffix;
}

MetaDataModel::MetaDataModel(QObject *parent) : QObject(parent)
{}

MetaDataModel::~MetaDataModel()
{}

QHash<QString, QString> MetaDataModel::audioProperties()
{
    return QHash<QString, QString> ();
}

QHash<QString, QString> MetaDataModel::descriptions()
{
    return QHash<QString, QString> ();
}

QList<TagModel* > MetaDataModel::tags()
{
    return QList<TagModel* > ();
}

QPixmap MetaDataModel::cover()
{
    return QPixmap();
}

QString MetaDataModel::coverPath()
{
    return QString();
}
