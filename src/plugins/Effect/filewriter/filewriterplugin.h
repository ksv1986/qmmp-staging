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

#ifndef FILEWRITERPLUGIN_H
#define FILEWRITERPLUGIN_H

#include <QMap>
#include <QFile>
#include <qmmp/effect.h>
#include <qmmp/qmmp.h>
#include <vorbis/vorbisenc.h>

class FileWriterPlugin : public Effect
{
public:
    FileWriterPlugin();
    virtual ~FileWriterPlugin();

    void configure(quint32 srate, ChannelMap map);
    void applyEffect(Buffer *b);

private:
    void init(const QMap<Qmmp::MetaData, QString> &metaData);
    void deinit();
    void sendHeader();

    ogg_stream_state m_os; //take physical pages, weld into a logical stream of packets
    ogg_page         m_og; //one Ogg bitstream page.  Vorbis packets are inside */
    ogg_packet       m_op; //one raw packet of data for decode
    vorbis_info      m_vi; //struct that stores all the static vorbis bitstream settings
    vorbis_comment   m_vc; //struct that stores all the user comments
    vorbis_dsp_state m_vd; //central working state for the packet->PCM decoder
    vorbis_block     m_vb; //local working space for packet->PCM decode
    bool m_inited;
    QFile m_file;

};

#endif // FILEWRITERPLUGIN_H
