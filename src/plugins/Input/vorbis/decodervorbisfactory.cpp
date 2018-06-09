/***************************************************************************
 *   Copyright (C) 2008-2018 by Ilya Kotov                                 *
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
#include <QTranslator>
#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <taglib/vorbisfile.h>
#include <taglib/tfilestream.h>
#include "decoder_vorbis.h"
#include "vorbismetadatamodel.h"
#include "decodervorbisfactory.h"

// DecoderOggFactory
bool DecoderVorbisFactory::canDecode(QIODevice *input) const
{
    char buf[36];
    if (input->peek(buf, 36) == 36 && !memcmp(buf, "OggS", 4)
            && !memcmp(buf + 29, "vorbis", 6))
        return true;

    return false;
}

const DecoderProperties DecoderVorbisFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("Ogg Vorbis Plugin");
    properties.shortName = "vorbis";
    properties.filters << "*.ogg";
    properties.description = tr("Ogg Vorbis Files");
    properties.contentTypes << "application/ogg" << "audio/x-vorbis+ogg";
    properties.hasAbout = true;
    properties.hasSettings = false;
    properties.noInput = false;
    return properties;
}

Decoder *DecoderVorbisFactory::create(const QString &, QIODevice *input)
{
    return new DecoderVorbis(input);
}

MetaDataModel* DecoderVorbisFactory::createMetaDataModel(const QString &path, QObject *parent)
{
    return new VorbisMetaDataModel(path, parent);
}

QList<TrackInfo *> DecoderVorbisFactory::createPlayList(const QString &path, TrackInfo::Parts parts, QStringList *)
{
    TrackInfo *info = new TrackInfo(path);

    if(parts == TrackInfo::NoParts)
        return QList<TrackInfo*>() << info;

    TagLib::FileStream stream(QStringToFileName(path), true);
    TagLib::Ogg::Vorbis::File fileRef(&stream);

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
    }

    if((parts & TrackInfo::Properties) && fileRef.audioProperties())
    {
        info->setValue(Qmmp::BITRATE, fileRef.audioProperties()->bitrate());
        info->setValue(Qmmp::SAMPLERATE, fileRef.audioProperties()->sampleRate());
        info->setValue(Qmmp::CHANNELS, fileRef.audioProperties()->channels());
        info->setValue(Qmmp::BITS_PER_SAMPLE, 32);  //float
        info->setValue(Qmmp::FORMAT_NAME, "Ogg Vorbis");
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

void DecoderVorbisFactory::showSettings(QWidget *)
{}

void DecoderVorbisFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About Ogg Vorbis Audio Plugin"),
                        tr("Qmmp Ogg Vorbis Audio Plugin")+"\n"+
                        tr("Written by: Ilya Kotov <forkotov02@ya.ru>")+"\n"+
                        tr("Source code based on mq3 project")
                       );
}

QTranslator *DecoderVorbisFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/vorbis_plugin_") + locale);
    return translator;
}
