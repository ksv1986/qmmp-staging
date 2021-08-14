/***************************************************************************
 *   Copyright (C) 2008-2021 by Ilya Kotov                                 *
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

#include <QFile>
#include <QMessageBox>
#include "aacfile.h"
#include "decoder_aac.h"
#include "decoderaacfactory.h"


// DecoderAACFactory

bool DecoderAACFactory::canDecode(QIODevice *input) const
{
    AACFile aac_file(input, false, false);
    return aac_file.isValid();
}

DecoderProperties DecoderAACFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("AAC Plugin");
    properties.filters = QStringList { "*.aac" };
    properties.description = tr("AAC Files");
    properties.contentTypes = QStringList { "audio/aacp", "audio/aac" };
    properties.shortName = "aac";
    properties.hasAbout = true;
    properties.hasSettings = false;
    return properties;
}

Decoder *DecoderAACFactory::create(const QString &, QIODevice *input)
{
    return new DecoderAAC(input);
}

QList<TrackInfo *> DecoderAACFactory::createPlayList(const QString &path, TrackInfo::Parts parts, QStringList *)
{
    TrackInfo *info = new TrackInfo(path);

    if(parts == TrackInfo::Parts())
        return QList<TrackInfo*>() << info;

    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
    {
        delete info;
        return QList<TrackInfo*>() << info;
    }

    AACFile aac_file(&file, parts & TrackInfo::MetaData);

    if(!aac_file.isValid())
    {
        delete info;
        return QList<TrackInfo *>();
    }

    if(parts & TrackInfo::MetaData)
        info->setValues(aac_file.metaData());

    if(parts & TrackInfo::Properties)
    {
        info->setValue(Qmmp::BITRATE, aac_file.bitrate());
        info->setValue(Qmmp::SAMPLERATE, aac_file.samplerate());
        //info->setValue(Qmmp::CHANNELS, aac_file.
        info->setValue(Qmmp::FORMAT_NAME, "AAC");
        info->setDuration(aac_file.duration());
    }

    return QList<TrackInfo*>() << info;
}

MetaDataModel* DecoderAACFactory::createMetaDataModel(const QString &path, bool readOnly)
{
    Q_UNUSED(path);
    Q_UNUSED(readOnly);
    return nullptr;
}

void DecoderAACFactory::showSettings(QWidget *)
{}

void DecoderAACFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About AAC Audio Plugin"),
                        tr("Qmmp AAC Audio Plugin")+"\n"+
                        tr("Written by: Ilya Kotov <forkotov02@ya.ru>"));
}

QString DecoderAACFactory::translation() const
{
    return QLatin1String(":/aac_plugin_");
}
