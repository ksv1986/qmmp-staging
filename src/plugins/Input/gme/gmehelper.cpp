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

#include <QRegularExpression>
#include <QSettings>
#include <qmmp/qmmp.h>
#include "gmehelper.h"

GmeHelper::GmeHelper()
{
     m_emu = nullptr;

     QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
     m_fade_length = settings.value("GME/fadeout_length", 7000).toInt();
     if(settings.value("GME/fadeout", false).toBool())
         m_fade_length = 0;
}

GmeHelper::~GmeHelper()
{
    if(m_emu)
        gme_delete(m_emu);
    m_emu = nullptr;
}

Music_Emu *GmeHelper::load(const QString &url, int sample_rate)
{
    if(m_emu)
        gme_delete(m_emu);
    m_emu = nullptr;
    QString path = url;
    if(url.contains("://"))
    {
        path.remove("gme://");
        path.remove(QRegularExpression("#\\d+$"));
    }
    const char *err = nullptr;
    gme_type_t file_type;
    if((err = gme_identify_file(qPrintable(path),&file_type)))
    {
        qWarning("GmeHelper: %s", err);
        return nullptr;
    }
    if(!file_type)
    {
        qWarning("DecoderGme: unsupported music type");
        return nullptr;
    }
    if(!(m_emu = gme_new_emu(file_type, sample_rate)))
    {
        qWarning("GmeHelper: out of memory");
        return nullptr;
    }
    if((err = gme_load_file(m_emu, qPrintable(path))))
    {
        qWarning("GmeHelper: %s", err);
        return nullptr;
    }
    QString m3u_path = path.left(path.lastIndexOf("."));
    m3u_path.append(".m3u");
    gme_load_m3u(m_emu, qPrintable(m3u_path));
    m_path = path;
    return m_emu;
}

QList<TrackInfo *> GmeHelper::createPlayList(TrackInfo::Parts parts)
{
    QList<TrackInfo*> list;
    if(!m_emu)
        return list;
    int count = gme_track_count(m_emu);
    gme_info_t *track_info;
    for(int i = 0; i < count; ++i)
    {
        TrackInfo *info = new TrackInfo();
        if(!gme_track_info(m_emu, &track_info, i))
        {
            if(track_info->length <= 0)
                track_info->length = track_info->intro_length + track_info->loop_length * 2;
        }
        if(track_info->length <= 0)
            track_info->length = (long) (2.5 * 60 * 1000);
        if(track_info->length < m_fade_length)
            track_info->length += m_fade_length;
        if(parts & TrackInfo::MetaData)
        {
            info->setValue(Qmmp::ALBUM, track_info->game);
            info->setValue(Qmmp::TITLE, track_info->song);
            info->setValue(Qmmp::ARTIST, track_info->author);
            info->setValue(Qmmp::COMMENT, track_info->comment);
            info->setValue(Qmmp::TRACK, i+1);
        }
        if(parts & TrackInfo::Properties)
        {
            info->setValue(Qmmp::BITRATE, 8);
            info->setValue(Qmmp::SAMPLERATE, 44100);
            info->setValue(Qmmp::CHANNELS, 2);
            info->setValue(Qmmp::BITS_PER_SAMPLE, 16);
            info->setValue(Qmmp::FORMAT_NAME, track_info->system);
        }

        info->setPath("gme://" + m_path + QString("#%1").arg(i+1));
        info->setDuration(track_info->length);
        gme_free_info(track_info);
        list << info;
    }
    return list;
}

int GmeHelper::fadeLength()
{
    return m_fade_length;
}
