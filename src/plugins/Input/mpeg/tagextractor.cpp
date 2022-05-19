/***************************************************************************
 *   Copyright (C) 2008-2022 by Ilya Kotov                                 *
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

#include <QIODevice>
#include <QSettings>
#include <QByteArray>
#include <QBuffer>
#include <QSettings>
#include <QSet>
#include <stdlib.h>
#ifdef WITH_LIBRCD
#include <librcd.h>
#endif
#include <qmmp/qmmptextcodec.h>
#include "tagextractor.h"

#define CSTR_TO_QSTR(str,utf) codec->toUnicode(str.toCString(utf)).trimmed()

bool TagExtractor::m_using_rusxmms = false;

TagExtractor::TagExtractor(QIODevice *d) : m_input(d)
{}

TagExtractor::~TagExtractor()
{}

QMap<Qmmp::MetaData, QString> TagExtractor::id3v2tag() const
{
    QByteArray array = m_input->peek(2048);
    int offset = array.indexOf("ID3");
    if (offset < 0)
        return QMap<Qmmp::MetaData, QString>();

    ID3v2Tag tag(&array, offset);
    if (tag.isEmpty())
        return QMap<Qmmp::MetaData, QString>();

    QSettings settings;
    settings.beginGroup("MPEG");
    QByteArray codecName = settings.value("ID3v2_encoding","UTF-8").toByteArray();

    if(m_using_rusxmms || codecName.contains("UTF") || codecName.isEmpty())
        codecName = "UTF-8";

    if(!m_using_rusxmms && settings.value("detect_encoding", false).toBool())
    {
        QByteArray detectedCharset = TagExtractor::detectCharset(&tag);
        if(!detectedCharset.isEmpty())
            codecName = detectedCharset;
    }
    settings.endGroup();

    QmmpTextCodec *codec = new QmmpTextCodec(codecName);

    bool utf = codec->name().contains("UTF");

    QMap<Qmmp::MetaData, QString> tags = {
        { Qmmp::ARTIST, CSTR_TO_QSTR(tag.artist(), utf) },
        { Qmmp::ALBUM, CSTR_TO_QSTR(tag.album(), utf) },
        { Qmmp::COMMENT, CSTR_TO_QSTR(tag.comment(), utf) },
        { Qmmp::GENRE, CSTR_TO_QSTR(tag.genre(), utf) },
        { Qmmp::TITLE, CSTR_TO_QSTR(tag.title(), utf) },
        { Qmmp::YEAR, QString::number(tag.year()) },
        { Qmmp::TRACK, QString::number(tag.track()) },
    };

    if(!tag.frameListMap()["TCOM"].isEmpty())
    {
        TagLib::String composer = tag.frameListMap()["TCOM"].front()->toString();
        tags.insert(Qmmp::COMPOSER, codec->toUnicode(composer.toCString(utf)).trimmed());
    }
    if(!tag.frameListMap()["TPOS"].isEmpty())
    {
        TagLib::String disc = tag.frameListMap()["TPOS"].front()->toString();
        tags.insert(Qmmp::DISCNUMBER, QString(disc.toCString()).trimmed());
    }

    delete codec;

    return tags;
}

void TagExtractor::setForceUtf8(bool enabled)
{
    m_using_rusxmms = enabled;
}

QByteArray TagExtractor::detectCharset(const TagLib::Tag *tag)
{
    if(tag->title().isLatin1() && tag->album().isLatin1() &&
            tag->artist().isLatin1() && tag->comment().isLatin1())
    {
#ifdef WITH_LIBRCD
        QSet<int> charsets;
        charsets << rcdGetRussianCharset(tag->title().toCString(), 0);
        charsets << rcdGetRussianCharset(tag->artist().toCString(), 0);
        charsets << rcdGetRussianCharset(tag->album().toCString(), 0);
        charsets << rcdGetRussianCharset(tag->comment().toCString(), 0);

        if(charsets.contains(RUSSIAN_CHARSET_WIN))
            return "WINDOWS-1251";
        else if(charsets.contains(RUSSIAN_CHARSET_ALT))
            return "IBM866";
        else if(charsets.contains(RUSSIAN_CHARSET_KOI))
            return "KOI8-R";
        else if(charsets.contains(RUSSIAN_CHARSET_UTF8))
            return "UTF-8";
        else if(charsets.contains(RUSSIAN_CHARSET_LATIN))
            return "ISO-8859-1";
#else
        return QByteArray();
#endif
    }
    return "UTF-8";
}

ID3v2Tag::ID3v2Tag(QByteArray *array, long offset) : TagLib::ID3v2::Tag(),
    m_offset(offset)
{
    m_buf = new QBuffer(array);
    m_buf->open(QIODevice::ReadOnly);
    read();
}

ID3v2Tag::~ID3v2Tag()
{
    delete m_buf;
}

void ID3v2Tag::read ()
{
    m_buf->seek(m_offset);
    uint to_read = TagLib::ID3v2::Header::size();
    if (to_read > 2048 - uint(m_offset))
        return;
    header()->setData(TagLib::ByteVector(m_buf->read(to_read).data(), to_read));
    to_read = header()->tagSize();
    if (!to_read ||  2048 < m_offset + TagLib::ID3v2::Header::size())
        return;
    QByteArray array = m_buf->read(to_read);
    TagLib::ByteVector v(array.data(), array.size());
    parse(v);
}
