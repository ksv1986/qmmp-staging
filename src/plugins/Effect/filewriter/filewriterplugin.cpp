/***************************************************************************
 *   Copyright (C) 2017 by Ilya Kotov                                      *
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

#include <QSettings>
#include <QDesktopServices>
#include <time.h>
#include <qmmp/soundcore.h>
#include <qmmpui/metadataformatter.h>
#include "filewriterplugin.h"

FileWriterPlugin::FileWriterPlugin()
{
    qsrand(time(NULL));
    m_inited = false;
}

FileWriterPlugin::~FileWriterPlugin()
{
    deinit();
}

void FileWriterPlugin::configure(quint32 srate, ChannelMap map)
{
    Effect::configure(srate, map);
    if(SoundCore::instance()->state() == Qmmp::Playing ||  SoundCore::instance()->state() == Qmmp::Paused)
        init(SoundCore::instance()->metaData());
}

void FileWriterPlugin::applyEffect(Buffer *b)
{
    if(!b->metaData.isNull())
        init(*b->metaData);

    if(!m_inited || !b->samples)
        return;

    int frames = b->samples / channels();
    float **buffer = vorbis_analysis_buffer(&m_vd, frames);

    for(int i = 0; i < frames; i++)
    {
        for(int c = 0; c < channels(); ++c)
            buffer[c][i] = b->data[i * channels() + c];
    }

    vorbis_analysis_wrote(&m_vd, frames);

    bool ok = true;

    while(ok && vorbis_analysis_blockout(&m_vd, &m_vb) == 1)
    {
        // analysis, assume we want to use bitrate management
        vorbis_analysis(&m_vb, NULL);
        vorbis_bitrate_addblock(&m_vb);

        while(ok && vorbis_bitrate_flushpacket(&m_vd, &m_op))
        {
            // weld the packet into the bitstream
            ogg_stream_packetin(&m_os, &m_op);
            // write out pages (if any)
            bool eos = false;
            while(!eos)
            {
                if(ogg_stream_pageout(&m_os, &m_og))
                {

                    if(m_file.write((char*)m_og.header, m_og.header_len) != m_og.header_len)
                    {
                        ok = false;
                        break;
                    }

                    if(m_file.write((char*)m_og.body, m_og.body_len) != m_og.body_len)
                    {
                        ok = false;
                        break;
                    }

                    eos = (ogg_page_eos(&m_og) != 0);
                }
                else
                {
                    eos = true;
                }
            }
        }
    }

    if(!ok)
    {
        qWarning("FileWriterPlugin: unable to write file: output disabled");
        deinit();
    }
}

void FileWriterPlugin::init(const QMap<Qmmp::MetaData, QString> &metaData)
{
    deinit();

    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    float quality = settings.value("FileWriter/vorbis_quality", 0.8).toFloat();
    QString outDir = QDesktopServices::storageLocation(QDesktopServices::MusicLocation);
    outDir = settings.value("FileWriter/out_dir", outDir).toString();
    QString fileName = settings.value("FileWriter/file_name", "%p%if(%p&%t, - ,)%t").toString();
    if(fileName.isEmpty())
        fileName = metaData[Qmmp::URL].section("/", 1);

    MetaDataFormatter formatter(fileName);
    fileName = formatter.format(metaData, SoundCore::instance()->totalTime() / 1000);
    if(!fileName.endsWith(".ogg", Qt::CaseInsensitive))
        fileName.append(".ogg");

    m_file.setFileName(outDir + "/" + fileName);

    int j = 1;
    while(m_file.exists())
    {
        m_file.setFileName(outDir + "/" + fileName.left(fileName.count() - 4) +
                           QString("-%1.ogg").arg(j++));
    }

    qDebug("FileWriterPlugin: writing file '%s'", qPrintable(m_file.fileName()));

    if(!m_file.open(QIODevice::WriteOnly))
    {
        qWarning("FileWriterPlugin: unable to create output file, error: %s",
                 qPrintable(m_file.errorString()));
        return;
    }

    vorbis_info_init(&m_vi);
    vorbis_encode_init_vbr(&m_vi, channels(), sampleRate(), quality);
    vorbis_comment_init(&m_vc);
    vorbis_analysis_init(&m_vd, &m_vi);
    vorbis_block_init(&m_vd,&m_vb);
    ogg_stream_init(&m_os,qrand());
    vorbis_comment_clear(&m_vc);

    static const struct
    {
        Qmmp::MetaData key;
        const char *tag;
    } tag_map[]  = {
        { Qmmp::TITLE, "title"},
        { Qmmp::ARTIST, "artist"},
        { Qmmp::ALBUM, "album"},
        { Qmmp::COMMENT, "comment"},
        { Qmmp::GENRE, "genre"},
        { Qmmp::TRACK, "tracknumber"},
        { Qmmp::YEAR, "date"},
        { Qmmp::COMPOSER, "composer"},
        { Qmmp::DISCNUMBER, "discnumber"},
        { Qmmp::UNKNOWN, 0}
    };

    int i = 0;
    while(tag_map[i].key != Qmmp::UNKNOWN)
    {
        if(!metaData[tag_map[i].key].isEmpty())
            vorbis_comment_add_tag(&m_vc, tag_map[i].tag, metaData.value(tag_map[i].key).toUtf8().constData());
        i++;
    }

    sendHeader();
    m_inited = true;
}

void FileWriterPlugin::deinit()
{
    if(m_inited)
    {
        ogg_stream_clear(&m_os);
        vorbis_block_clear(&m_vb);
        vorbis_dsp_clear(&m_vd);
        vorbis_comment_clear(&m_vc);
        vorbis_info_clear(&m_vi);
        m_file.close();
        m_inited = false;
    }
}

void FileWriterPlugin::sendHeader()
{
    ogg_packet header;
    ogg_packet header_comm;
    ogg_packet header_code;

    vorbis_analysis_headerout(&m_vd, &m_vc, &header, &header_comm, &header_code);
    ogg_stream_packetin(&m_os, &header); // automatically placed in its own page
    ogg_stream_packetin(&m_os, &header_comm);
    ogg_stream_packetin(&m_os, &header_code);
}
