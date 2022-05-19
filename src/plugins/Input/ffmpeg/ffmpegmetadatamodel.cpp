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

#include <QRegularExpression>
#include <QtDebug>
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}
#include <taglib/apefile.h>
#include <stdint.h>
#include <qmmp/metadatamanager.h>
#include "ffmpegmetadatamodel.h"

FFmpegMetaDataModel::FFmpegMetaDataModel(const QString &path, bool readOnly) : MetaDataModel(readOnly)
{
    AVFormatContext *in = nullptr;
    m_path = path;

    if(path.startsWith("ffmpeg://"))
    {
        m_path.remove("ffmpeg://");
        m_path.remove(QRegularExpression("#\\d+$"));
    }
    else if(path.startsWith("m4b://"))
    {
        m_path.remove("m4b://");
        m_path.remove(QRegularExpression("#\\d+$"));
    }

#ifdef Q_OS_WIN
    if (avformat_open_input(&in, m_path.toUtf8().constData(), nullptr, nullptr) < 0)
#else
    if (avformat_open_input(&in, m_path.toLocal8Bit().constData(), nullptr, nullptr) < 0)
#endif
        return;

    if(in)
    {
        avformat_find_stream_info(in, nullptr);
        av_read_play(in);

        AVCodecParameters *c = nullptr;

        for (uint idx = 0; idx < in->nb_streams; idx++)
        {
            c = in->streams[idx]->codecpar;

            if (c->codec_type == AVMEDIA_TYPE_VIDEO && c->codec_id == AV_CODEC_ID_MJPEG)
                break;
        }
        if (c)
        {
            AVPacket pkt;
            av_read_frame(in, &pkt);
            m_pixmap.loadFromData(QByteArray((const char*)pkt.data, pkt.size));
        }

        avformat_close_input(&in);
    }

    if(!readOnly && m_path.endsWith(".ape", Qt::CaseInsensitive))
    {
        m_stream = new TagLib::FileStream(QStringToFileName(m_path), readOnly);
        m_file = new TagLib::APE::File(m_stream);
        m_tag = m_file->APETag();
        m_tags << new ApeTagModel(m_file);
        setReadOnly(m_file->readOnly());
        setDialogHints(MetaDataModel::IsCueEditable);
    }
    else
    {
        setReadOnly(true);
    }
}

FFmpegMetaDataModel::~FFmpegMetaDataModel()
{
    qDeleteAll(m_tags);
    m_tags.clear();
    delete m_file;
    delete m_stream;
}

QList<TagModel *> FFmpegMetaDataModel::tags() const
{
    return m_tags;
}

QPixmap FFmpegMetaDataModel::cover() const
{
    return m_pixmap;
}

QString FFmpegMetaDataModel::coverPath() const
{
    return MetaDataManager::instance()->findCoverFile(m_path);
}

QString FFmpegMetaDataModel::cue() const
{
    if(m_tag && m_tag->itemListMap().contains("CUESHEET"))
        return TStringToQString(m_tag->itemListMap()["CUESHEET"].toString());

    return QString();
}

void FFmpegMetaDataModel::setCue(const QString &content)
{
    if(!m_tag)
        m_tag = m_file->APETag(true);

    m_tag->addValue("CUESHEET", QStringToTString(content));
    m_file->save();
    m_tag = m_file->APETag();
}

void FFmpegMetaDataModel::removeCue()
{
    if(m_tag)
    {
        m_tag->removeItem("CUESHEET");
        m_file->save();
        m_tag = m_file->APETag();
    }
}

ApeTagModel::ApeTagModel(TagLib::APE::File *file) : m_file(file)
{}

ApeTagModel::~ApeTagModel()
{}

QString ApeTagModel::name() const
{
    return QLatin1String("APE");
}

QList<Qmmp::MetaData> ApeTagModel::keys() const
{
    QList<Qmmp::MetaData> list = TagModel::keys();
    list.removeAll(Qmmp::DISCNUMBER);
    return list;
}

QString ApeTagModel::value(Qmmp::MetaData key) const
{
    TagLib::APE::Tag *tag = m_file->APETag(false);
    if(!tag)
        return QString();

    switch(key)
    {
    case Qmmp::TITLE:
        return TStringToQString(tag->title());
    case Qmmp::ARTIST:
        return TStringToQString(tag->artist());
    case Qmmp::ALBUMARTIST:
        if(!tag->itemListMap()["ALBUM ARTIST"].isEmpty())
            return TStringToQString(tag->itemListMap()["ALBUM ARTIST"].toString());
        break;
    case Qmmp::ALBUM:
        return TStringToQString(tag->album());
    case Qmmp::COMMENT:
        return TStringToQString(tag->comment());
    case Qmmp::GENRE:
        return TStringToQString(tag->genre());
    case Qmmp::COMPOSER:
        if(!tag->itemListMap()["COMPOSER"].isEmpty())
             return TStringToQString(tag->itemListMap()["COMPOSER"].toString());
        break;
    case Qmmp::YEAR:
        return QString::number(tag->year());
    case Qmmp::TRACK:
        return QString::number(tag->track());
    default:
        return QString();
    }

    return QString();
}

void ApeTagModel::setValue(Qmmp::MetaData key, const QString &value)
{
    TagLib::APE::Tag *tag = m_file->APETag(false);
    if(!tag)
        return;

    TagLib::String str = QStringToTString(value);

    switch(key)
    {
    case Qmmp::TITLE:
        tag->setTitle(str);
        break;
    case Qmmp::ARTIST:
        tag->setArtist(str);
        break;
    case Qmmp::ALBUMARTIST:
        tag->addValue("ALBUM ARTIST", str, true);
        return;
    case Qmmp::ALBUM:
        tag->setAlbum(str);
        break;
    case Qmmp::COMMENT:
        tag->setComment(str);
        break;
    case Qmmp::COMPOSER:
        tag->addValue("COMPOSER", str, true);
        return;
    case Qmmp::GENRE:
        tag->setGenre(str);
        break;
    case Qmmp::YEAR:
        tag->setYear(value.toInt());
        break;
    case Qmmp::TRACK:
        tag->setTrack(value.toInt());
    default:
        ;
    }
}

bool ApeTagModel::exists() const
{
    return m_file->hasAPETag();
}

void ApeTagModel::create()
{
    m_file->APETag(true);
    m_strip = false;
}

void ApeTagModel::remove()
{
    m_strip = true;
}

void ApeTagModel::save()
{
    if(m_strip)
        m_file->strip();

    m_strip = false;
    m_file->save();
}
