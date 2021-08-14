/***************************************************************************
 *   Copyright (C) 2013-2021 by Ilya Kotov                                 *
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

#include <QMessageBox>
#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <taglib/opusfile.h>
#include "decoder_opus.h"
#include "opusmetadatamodel.h"
#include "decoderopusfactory.h"

// DecoderOpusFactory
bool DecoderOpusFactory::canDecode(QIODevice *input) const
{
    char buf[36];
    if (input->peek(buf, 36) == 36 && !memcmp(buf, "OggS", 4)
            && !memcmp(buf + 28, "OpusHead", 8))
        return true;
    return false;
}

DecoderProperties DecoderOpusFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("Opus Plugin");
    properties.shortName = "opus";
    properties.filters = QStringList { "*.opus" };
    properties.description = tr("Ogg Opus Files");
    properties.contentTypes = QStringList { "audio/opus" };
    properties.hasAbout = true;
    properties.hasSettings = false;
    properties.noInput = false;
    return properties;
}

Decoder *DecoderOpusFactory::create(const QString &url, QIODevice *input)
{
    return new DecoderOpus(url, input);
}

MetaDataModel* DecoderOpusFactory::createMetaDataModel(const QString &path, bool readOnly)
{
    return new OpusMetaDataModel(path, readOnly);
}

QList<TrackInfo *> DecoderOpusFactory::createPlayList(const QString &path, TrackInfo::Parts parts, QStringList *)
{
    TrackInfo *info = new TrackInfo(path);

    if(parts == TrackInfo::Parts())
        return QList<TrackInfo*>() << info;

    TagLib::Ogg::Opus::File fileRef(QStringToFileName(path));

    if((parts & TrackInfo::MetaData) && fileRef.tag() && !fileRef.tag()->isEmpty())
    {
        TagLib::Ogg::XiphComment *tag = fileRef.tag();
        info->setValue(Qmmp::ALBUM, TStringToQString(tag->album()));
        info->setValue(Qmmp::ARTIST, TStringToQString(tag->artist()));
        info->setValue(Qmmp::COMMENT, TStringToQString(tag->comment()));
        info->setValue(Qmmp::GENRE, TStringToQString(tag->genre()));
        info->setValue(Qmmp::TITLE, TStringToQString(tag->title()));
        info->setValue(Qmmp::YEAR, tag->year());
        info->setValue(Qmmp::TRACK, tag->track());
        TagLib::Ogg::FieldListMap items = tag->fieldListMap();
        if(!items["ALBUMARTIST"].isEmpty())
            info->setValue(Qmmp::ALBUMARTIST, TStringToQString(items["ALBUMARTIST"].toString()));
        if(!items["COMPOSER"].isEmpty())
            info->setValue(Qmmp::COMPOSER, TStringToQString(items["COMPOSER"].toString()));
        if(!items["DISCNUMBER"].isEmpty())
            info->setValue(Qmmp::DISCNUMBER, TStringToQString(items["DISCNUMBER"].toString()));
    }

    if((parts & TrackInfo::Properties) && fileRef.audioProperties())
    {
        info->setValue(Qmmp::BITRATE, fileRef.audioProperties()->bitrate());
        info->setValue(Qmmp::SAMPLERATE, fileRef.audioProperties()->sampleRate());
        info->setValue(Qmmp::CHANNELS, fileRef.audioProperties()->channels());
        info->setValue(Qmmp::BITS_PER_SAMPLE, 32); //float
        info->setValue(Qmmp::FORMAT_NAME, "Ogg Opus");
        info->setDuration(fileRef.audioProperties()->lengthInMilliseconds());
    }

    if((parts & TrackInfo::ReplayGainInfo) && fileRef.tag() && !fileRef.tag()->isEmpty())
    {
        TagLib::Ogg::XiphComment *tag = fileRef.tag();
        TagLib::Ogg::FieldListMap items = tag->fieldListMap();
        if(items.contains("REPLAYGAIN_TRACK_GAIN"))
            info->setValue(Qmmp::REPLAYGAIN_TRACK_GAIN, TStringToQString(items["REPLAYGAIN_TRACK_GAIN"].front()));
        if(items.contains("REPLAYGAIN_TRACK_PEAK"))
            info->setValue(Qmmp::REPLAYGAIN_TRACK_PEAK, TStringToQString(items["REPLAYGAIN_TRACK_PEAK"].front()));
        if(items.contains("REPLAYGAIN_ALBUM_GAIN"))
            info->setValue(Qmmp::REPLAYGAIN_ALBUM_GAIN, TStringToQString(items["REPLAYGAIN_ALBUM_GAIN"].front()));
        if(items.contains("REPLAYGAIN_ALBUM_PEAK"))
            info->setValue(Qmmp::REPLAYGAIN_ALBUM_PEAK, TStringToQString(items["REPLAYGAIN_ALBUM_PEAK"].front()));
    }

    return QList<TrackInfo*>() << info;
}

void DecoderOpusFactory::showSettings(QWidget *)
{}

void DecoderOpusFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About Opus Audio Plugin"),
                        tr("Qmmp Opus Audio Plugin")+"\n"+
                        tr("Written by: Ilya Kotov <forkotov02@ya.ru>"));
}

QString DecoderOpusFactory::translation() const
{
    return QLatin1String(":/opus_plugin_");
}
