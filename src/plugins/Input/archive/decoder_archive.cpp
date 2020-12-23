/***************************************************************************
 *   Copyright (C) 2016-2021 by Ilya Kotov                                 *
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
#include <QRegularExpression>
#include <archive_entry.h>
#include "archiveinputdevice.h"
#include "archivetagreader.h"
#include "decoder_archive.h"

DecoderArchive::DecoderArchive(const QString &url) : m_url(url)
{}

DecoderArchive::~DecoderArchive()
{
    if(m_decoder)
    {
        delete m_decoder;
        m_decoder = nullptr;
    }
    if(m_input)
    {
        delete m_input;
        m_input = nullptr;
    }
}

bool DecoderArchive::initialize()
{
    QString filePath = m_url.section("#", -1);
    QString archivePath = m_url;
    archivePath.remove(QRegularExpression("^.+://"));
    archivePath.remove(QRegularExpression("#.+$"));

    if(!QFile::exists(archivePath))
    {
        qWarning("DecoderArchive: file '%s' not found", qPrintable(archivePath));
        return false;
    }

    //is this file supported by qmmp?
    QList<DecoderFactory *> filtered;
    for(DecoderFactory *f : Decoder::findByFileExtension(filePath))
    {
        if(!f->properties().noInput)
            filtered << f; //add all factories with streaming input
    }

    if(filtered.isEmpty())
    {
        qWarning("DecoderArchive: unable to find decoder");
        return false;
    }

    m_input = new ArchiveInputDevice(m_url);

    if(!m_input->isOpen())
    {
        qWarning("DecoderArchive: unable to open archive");
        return false;
    }

    DecoderFactory *factory = nullptr;
    if(filtered.size() == 1)
        factory = filtered.first();
    else
    {
        //multiple factories, so trying to determine by content
        for(DecoderFactory *fact : qAsConst(filtered))
        {
            if(fact->canDecode(m_input))
            {
                factory = fact;
                break;
            }
        }
    }

    if(!factory)
    {
        qWarning("DecoderArchive: unable to find decoder factory");
        return false;
    }

    qDebug("DecoderArchive: selected decoder: %s", qPrintable(factory->properties().shortName));

    ArchiveTagReader reader(m_input, m_url);
    addMetaData(reader.metaData());

    m_input->seek(0);
    m_decoder = factory->create(m_url, m_input);
    if(!m_decoder->initialize())
    {
        qWarning("DecoderArchive: unable to initialize decoder");
        return false;
    }
    configure(m_decoder->audioParameters());
    return true;
}

qint64 DecoderArchive::totalTime() const
{
    return m_decoder->totalTime();
}

void DecoderArchive::seek(qint64 time)
{
    m_decoder->seek(time);
}

qint64 DecoderArchive::read(unsigned char *data, qint64 maxSize)
{
    return m_decoder->read(data, maxSize);
}

int DecoderArchive::bitrate() const
{
    return m_decoder->bitrate();
}
