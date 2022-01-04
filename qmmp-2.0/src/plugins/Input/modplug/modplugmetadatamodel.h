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

#ifndef MODPLUGMETADATAMODEL_H
#define MODPLUGMETADATAMODEL_H

#include <qmmp/metadatamodel.h>

class CSoundFile;

class ModPlugMetaDataModel : public MetaDataModel
{
    Q_DECLARE_TR_FUNCTIONS(ModPlugMetaDataModel)
public:
    explicit ModPlugMetaDataModel(const QString &path);
    ~ModPlugMetaDataModel();
    QList<MetaDataItem> extraProperties() const override;
    QList<MetaDataItem> descriptions() const override;
    static QString getTypeName(quint32 type);

private:
    CSoundFile* m_soundFile = nullptr;
    QByteArray m_buffer;
    QString m_path;
};

#endif // MODPLUGMETADATAMODEL_H
