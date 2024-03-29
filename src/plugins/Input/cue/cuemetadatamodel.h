/***************************************************************************
 *   Copyright (C) 2009-2022 by Ilya Kotov                                 *
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

#ifndef CUEMETADATAMODEL_H
#define CUEMETADATAMODEL_H

#include <qmmp/metadatamodel.h>

class CueFile;
class QmmpTextCodec;

class CUEMetaDataModel : public MetaDataModel
{
public:
    explicit CUEMetaDataModel(bool readOnly, const QString &url);
    ~CUEMetaDataModel();
    QList<MetaDataItem> extraProperties() const override;
    QString coverPath() const override;
    QString cue() const override;
    void setCue(const QString &content) override;
    void removeCue() override;

private:
    QString m_dataFilePath, m_cueFilePath;
    mutable QmmpTextCodec *m_codec = nullptr;
};

#endif // CUEMETADATAMODEL_H
