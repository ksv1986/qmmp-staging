/***************************************************************************
 *   Copyright (C) 2010-2021 by Ilya Kotov                                 *
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
#include <QRegularExpression>
#include "settingsdialog.h"
#include "gmehelper.h"
#include "decoder_gme.h"
#include "decodergmefactory.h"

// DecoderGmeFactory

bool DecoderGmeFactory::canDecode(QIODevice *) const
{
    return false;
}

DecoderProperties DecoderGmeFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("GME Plugin");
    properties.filters = QStringList { "*.ay", "*.gbs", "*.gym", "*.hes", "*.kss", "*.nsf", "*.nsfe",
            "*.sap", "*.spc", "*.vgm", "*.vgz" };
    properties.description = tr("Game Music Files");
    //properties.contentType = ;
    properties.shortName = "gme";
    properties.hasAbout = true;
    properties.hasSettings = true;
    properties.noInput = true;
    properties.protocols = QStringList { "gme" };
    return properties;
}

Decoder *DecoderGmeFactory::create(const QString &path, QIODevice *input)
{
    Q_UNUSED(input);
    return new DecoderGme(path);
}

QList<TrackInfo *> DecoderGmeFactory::createPlayList(const QString &path, TrackInfo::Parts parts, QStringList *ignoredFiles)
{
    GmeHelper helper;
    //is it one track?
    if(path.contains("://"))
    {
        QString filePath = path;
        filePath.remove("gme://");
        filePath.remove(QRegularExpression("#\\d+$"));
        int track = path.section("#", -1).toInt();
        QList<TrackInfo *> list = createPlayList(filePath, parts, ignoredFiles);
        if (list.isEmpty() || track <= 0 || track > list.count())
        {
            qDeleteAll(list);
            list.clear();
            return list;
        }
        TrackInfo *info = list.takeAt(track - 1);
        qDeleteAll(list);
        return QList<TrackInfo *>() << info;
    }

    Music_Emu *emu = helper.load(path);
    if(!emu)
    {
        qWarning("DecoderGmeFactory: unable to open file");
        return QList<TrackInfo *>();
    }
    return helper.createPlayList(parts);
}

MetaDataModel* DecoderGmeFactory::createMetaDataModel(const QString &path, bool readOnly)
{
    Q_UNUSED(path);
    Q_UNUSED(readOnly);
    return nullptr;
}

void DecoderGmeFactory::showSettings(QWidget *parent)
{
   SettingsDialog *d = new SettingsDialog(parent);
   d->show();
}

void DecoderGmeFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About GME Audio Plugin"),
                        tr("Qmmp GME Audio Plugin")+"\n"+
                        tr("This plugin uses Game_Music_Emu library to play game music files")+"\n"+
                        tr("Written by: Ilya Kotov <forkotov02@ya.ru>"));
}

QString DecoderGmeFactory::translation() const
{
    return QLatin1String(":/gme_plugin_");
}
