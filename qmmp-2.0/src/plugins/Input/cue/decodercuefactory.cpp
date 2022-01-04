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

#include <QMessageBox>
#include "decoder_cue.h"
#include "cuemetadatamodel.h"
#include "cuefile.h"
#include "settingsdialog.h"
#include "decodercuefactory.h"

// DecoderCUEFactory

bool DecoderCUEFactory::canDecode(QIODevice *) const
{
    return false;
}

DecoderProperties DecoderCUEFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("CUE Plugin");
    properties.shortName = "cue";
    properties.filters = QStringList { "*.cue" };
    properties.description = tr("CUE Files");
    properties.protocols = QStringList { "cue" };
    properties.hasAbout = true;
    properties.hasSettings = true;
    properties.noInput = true;
    return properties;
}

Decoder *DecoderCUEFactory::create(const QString &path, QIODevice *input)
{
    Q_UNUSED(input);
    return new DecoderCUE(path);
}

QList<TrackInfo *> DecoderCUEFactory::createPlayList(const QString &path, TrackInfo::Parts parts, QStringList *ignoredPaths)
{
    Q_UNUSED(parts);
    CueFile cueFile(path);
    if(path.contains("://"))
    {
        int track = path.section("#", -1).toInt();
        return cueFile.createPlayList(track);
    }
    else
    {
        ignoredPaths->append(cueFile.dataFilePaths());
        return cueFile.createPlayList();
    }
}

MetaDataModel* DecoderCUEFactory::createMetaDataModel(const QString &path, bool readOnly)
{
    return path.startsWith("cue://") ? new CUEMetaDataModel(readOnly, path) : nullptr;
}

void DecoderCUEFactory::showSettings(QWidget *parent)
{
     SettingsDialog *s = new SettingsDialog(parent);
     s->show();
}

void DecoderCUEFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About CUE Audio Plugin"),
                        tr("Qmmp CUE Audio Plugin")+"\n"+
                        tr("Written by: Ilya Kotov <forkotov02@ya.ru>"));
}

QString DecoderCUEFactory::translation() const
{
    return  QLatin1String(":/cue_plugin_");
}
