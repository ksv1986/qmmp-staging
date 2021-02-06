/***************************************************************************
 *   Copyright (C) 2009-2021 by Ilya Kotov                                 *
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
#include <QtDebug>
#include <qmmp/metadatamanager.h>
#include "cuefile.h"
#include "cuemetadatamodel.h"

CUEMetaDataModel::CUEMetaDataModel(const QString &url) : MetaDataModel(false, IsCueEditable)
{
    CueFile file(url);
    if (file.isEmpty())
    {
        qWarning("CUEMetaDataModel: invalid cue file");
        return;
    }
    int track = url.section("#", -1).toInt();
    m_dataFilePath = file.dataFilePath(track);
    m_cueFilePath = file.cueFilePath();
    if(!QFileInfo(m_cueFilePath).isWritable())
        setReadOnly(true);
}

CUEMetaDataModel::~CUEMetaDataModel()
{}

QList<MetaDataItem> CUEMetaDataModel::extraProperties() const
{
    QList<MetaDataItem> ep;
    MetaDataModel *model = MetaDataManager::instance()->createMetaDataModel(m_dataFilePath, true);
    if(model)
    {
        ep = model->extraProperties();
        delete model;
    }
    return ep;
}

QString CUEMetaDataModel::coverPath() const
{
    return MetaDataManager::instance()->findCoverFile(m_dataFilePath);
}

QString CUEMetaDataModel::cue() const
{
    QFile file(m_cueFilePath);
    file.open(QIODevice::ReadOnly);
    return QString::fromUtf8(file.readAll());
}

void CUEMetaDataModel::setCue(const QString &content)
{

}

void CUEMetaDataModel::removeCue()
{

}
