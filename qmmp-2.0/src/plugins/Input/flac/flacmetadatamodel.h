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

#ifndef FLACMETADATAMODEL_H
#define FLACMETADATAMODEL_H

#include <taglib/flacfile.h>
#include <taglib/oggflacfile.h>
#include <taglib/xiphcomment.h>
#include <taglib/tfilestream.h>
#include <qmmp/metadatamodel.h>

class FLACMetaDataModel : public MetaDataModel
{
public:
    FLACMetaDataModel(const QString &path, bool readOnly);
    ~FLACMetaDataModel();
    QList<TagModel* > tags() const override;
    QPixmap cover() const override;
    QString coverPath() const override;
    void setCover(const QPixmap &pix) override;
    void removeCover() override;
    QString cue() const override;
    void setCue(const QString &content) override;
    void removeCue() override;

private:
    QString m_path;
    QList<TagModel* > m_tags;
    TagLib::Ogg::XiphComment *m_tag = nullptr;
    TagLib::File *m_file = nullptr;
    TagLib::FileStream *m_stream = nullptr;
};

class VorbisCommentModel : public TagModel
{
public:
    VorbisCommentModel(TagLib::Ogg::XiphComment *tag, TagLib::File *file);
    ~VorbisCommentModel();
    QString name() const override;
    QString value(Qmmp::MetaData key) const override;
    void setValue(Qmmp::MetaData key, const QString &value) override;
    void save() override;

private:
    TagLib::File *m_file;
    TagLib::Ogg::XiphComment *m_tag;
};

#endif // FLACMETADATAMODEL_H
