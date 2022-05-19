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

#ifndef FFMPEGMETADATAMODEL_H
#define FFMPEGMETADATAMODEL_H

#include <taglib/apefile.h>
#include <taglib/apetag.h>
#include <taglib/tfilestream.h>
#include <qmmp/metadatamodel.h>

class FFmpegMetaDataModel : public MetaDataModel
{
public:
    explicit FFmpegMetaDataModel(const QString &path, bool readOnly);
    ~FFmpegMetaDataModel();
    QList<TagModel *> tags() const override;
    QPixmap cover() const override;
    QString coverPath() const override;
    QString cue() const override;
    void setCue(const QString &content) override;
    void removeCue() override;

private:
    QPixmap m_pixmap;
    QList<TagModel* > m_tags;
    TagLib::APE::Tag *m_tag;
    TagLib::APE::File *m_file = nullptr;
    TagLib::FileStream *m_stream = nullptr;
    QString m_path;
};

class ApeTagModel : public TagModel
{
public:
    ApeTagModel(TagLib::APE::File *file);
    ~ApeTagModel();
    QString name() const override;
    QList<Qmmp::MetaData> keys() const override;
    QString value(Qmmp::MetaData key) const override;
    void setValue(Qmmp::MetaData key, const QString &value) override;
    bool exists() const override;
    void create() override;
    void remove() override;
    void save() override;

private:
    TagLib::APE::File *m_file;
    bool m_strip = false;

};

#endif // FFMPEGMETADATAMODEL_H
