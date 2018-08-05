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

#ifndef FLACMETADATAMODEL_H
#define FLACMETADATAMODEL_H

#include <taglib/flacfile.h>
#include <taglib/oggflacfile.h>
#include <taglib/xiphcomment.h>
#include <taglib/tfilestream.h>
#include <qmmp/metadatamodel.h>

class FLACMetaDataModel : public MetaDataModel
{
Q_OBJECT
public:
    FLACMetaDataModel(const QString &path, bool readOnly, QObject *parent);
    ~FLACMetaDataModel();
    QList<TagModel* > tags() const override;
    QPixmap cover() const override;
    QString coverPath() const override;

private:
    QString m_path;
    QList<TagModel* > m_tags;
    TagLib::File *m_file;
    TagLib::FileStream *m_stream;
};

class VorbisCommentModel : public TagModel
{
public:
    VorbisCommentModel(TagLib::Ogg::XiphComment *tag, TagLib::File *file);
    ~VorbisCommentModel();
    const QString name();
    const QString value(Qmmp::MetaData key);
    void setValue(Qmmp::MetaData key, const QString &value);
    void save();

private:
    TagLib::File *m_file;
    TagLib::Ogg::XiphComment *m_tag;
};

#endif // FLACMETADATAMODEL_H
