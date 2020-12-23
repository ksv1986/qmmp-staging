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

#include <QRegularExpression>
#include <QFileInfo>
#include <QStringList>
#include <QProcess>
#include "mplayermetadatamodel.h"

MplayerMetaDataModel::MplayerMetaDataModel(const QString &path)
    : MetaDataModel(true, MetaDataModel::CompletePropertyList),
      m_path(path)
{}

MplayerMetaDataModel::~MplayerMetaDataModel()
{}

QList<MetaDataItem> MplayerMetaDataModel::extraProperties() const
{
    QList<MetaDataItem> ep;
    ep << MetaDataItem(tr("Size"), QFileInfo(m_path).size ()/1024, tr("KiB"));
    //prepare and start mplayer process
    QStringList args = { "-slave", "-identify", "-frames", "0", "-vo", "null", "-ao", "null", m_path };
    QProcess mplayer_process;
    mplayer_process.start("mplayer", args);
    mplayer_process.waitForFinished();
    QString str = QString::fromLocal8Bit(mplayer_process.readAll()).trimmed();
    const QStringList lines = str.split("\n");
    //mplayer std output parsing
    QRegularExpression rx_id("^(ID_.*)=(.*)");
    QMap<QString, QString> params;
    for(const QString &line : qAsConst(lines))
    {
        QRegularExpressionMatch match = rx_id.match(line.trimmed());
        if(match.hasMatch())
            params.insert(match.captured(1), match.captured(2));
    }
    //general info
    ep << MetaDataItem(tr("Demuxer"), params["ID_DEMUXER"]);
    //video info
    ep << MetaDataItem(tr("Video format"), params["ID_VIDEO_FORMAT"]);
    ep << MetaDataItem(tr("FPS"), params["ID_VIDEO_FPS"]);
    ep << MetaDataItem(tr("Video codec"), params["ID_VIDEO_CODEC"]);
    ep << MetaDataItem(tr("Aspect ratio"), params["ID_VIDEO_ASPECT"]);
    ep << MetaDataItem(tr("Video bitrate"), params["ID_VIDEO_BITRATE"].toInt() / 1000, tr("kbps"));
    ep << MetaDataItem(tr("Resolution"), QString("%1x%2").arg(params["ID_VIDEO_WIDTH"]).arg(params["ID_VIDEO_HEIGHT"]));
    ep << MetaDataItem(tr("Audio codec"), params["ID_AUDIO_CODEC"]);
    ep << MetaDataItem(tr("Sample rate"), params["ID_AUDIO_RATE"], tr("Hz"));
    ep << MetaDataItem(tr("Audio bitrate"), params["ID_AUDIO_BITRATE"].toInt() / 1000, tr("kbps"));
    ep << MetaDataItem(tr("Channels"), params["ID_AUDIO_NCH"]);
    return ep;
}
