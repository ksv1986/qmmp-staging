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

#ifndef DECODER_CDAUIDO_H
#define DECODER_CDAUIDO_H

#include <sys/types.h>
#include <cdio/cdio.h>
#include <QMultiHash>
#include <qmmp/decoder.h>
#include <qmmp/trackinfo.h>

class CDATrack
{
public:
    CDATrack()
    {}

    TrackInfo info;
    lsn_t first_sector = 0;
    lsn_t last_sector = 0;
};

class DecoderCDAudio : public Decoder
{
public:
    explicit DecoderCDAudio(const QString &url);
    virtual ~DecoderCDAudio();

    static QList <CDATrack> generateTrackList(const QString &device, TrackInfo::Parts parts = TrackInfo::AllParts);
    static qint64 calculateTrackLength(lsn_t startlsn, lsn_t endlsn);
    static void clearTrackCache();

     // Standard Decoder API
    bool initialize() override;
    qint64 totalTime() const override;
    int bitrate() const override;
    qint64 read(unsigned char *audio, qint64 maxSize) override;
    void seek(qint64 time) override;

private:
    //helper functions
    static void saveToCache(QList <CDATrack> tracks,  uint disc_id);
    static bool readFromCache(QList <CDATrack> *tracks, uint disc_id);
    // libcdio variables
    lsn_t m_first_sector = -1;
    lsn_t m_last_sector = -1;
    lsn_t m_current_sector = -1;
    CdIo_t *m_cdio = nullptr;
    //other variables
    QString m_url;
    int m_bitrate = 0;
    qint64 m_totalTime = 0;
    char *m_buffer;
    qint64 m_buffer_at = 0;
    //tracks cache
    static QList <CDATrack> m_track_cache;
};

#endif // DECODER_CDAUIDO_H
