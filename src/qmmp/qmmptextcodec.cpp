/***************************************************************************
 *   Copyright (C) 2021 by Ilya Kotov                                      *
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

#include <iconv.h>
#include <errno.h>
#include <QtDebug>
#include "qmmptextcodec.h"

QmmpTextCodec::QmmpTextCodec(const QByteArray &charset) : m_name(charset.toUpper())
{
    if(m_name == "UTF-8" || m_name == "UTF-16")
        return;

    if((m_to = iconv_open(m_name.constData(), "UTF-16")) == (iconv_t)(-1))
    {
        qWarning("QmmpTextCodec: error: %s", strerror(errno));
        m_to = nullptr;
    }

    if((m_from = iconv_open("UTF-16", m_name.constData())) == (iconv_t)(-1))
    {
        qWarning("QmmpTextCodec: error: %s", strerror(errno));
        m_from = nullptr;
    }
}

QmmpTextCodec::~QmmpTextCodec()
{
    if(m_to)
        iconv_close(m_to);
    if(m_from)
        iconv_close(m_from);
}

const QByteArray &QmmpTextCodec::name() const
{
    return m_name;
}

QString QmmpTextCodec::toUnicode(const QByteArray &a) const
{
    if(m_name == "UTF-16")
        return QString::fromUtf16(reinterpret_cast<const char16_t *>(a.data()), a.size() / 2);
    else if(!m_from || m_name == "UTF-8")
        return QString::fromUtf8(a);

    size_t inBytesLeft = 0;
    size_t outBytesLeft = 0;

    // reset state
    iconv(m_from, nullptr, &inBytesLeft, nullptr, &outBytesLeft);

    char *inBytes = const_cast<char *>(a.data());
    inBytesLeft = a.size();
    outBytesLeft = a.size() * 2 + 2;
    QByteArray ba(outBytesLeft, Qt::Uninitialized);
    char *outBytes = ba.data();
    outBytesLeft = ba.size();

    while(inBytesLeft > 0)
    {
        size_t ret = iconv(m_from,  &inBytes, &inBytesLeft, &outBytes, &outBytesLeft);

        if(ret == (size_t) -1)
        {
            if(errno == E2BIG) //increase buffer size
            {
                int offset = ba.size() - outBytesLeft;
                ba.resize(ba.size() * 2);
                outBytes = ba.data() + offset;
                outBytesLeft = ba.size() - offset;
                continue;
            }

            if(errno == EINVAL)
                break;
            else if(errno == EILSEQ)
            {
                // skip the next character
                ++inBytes;
                --inBytesLeft;
                continue;
            }

            //fallback
            return QString::fromLatin1(a);
        }
    }

    return QString::fromUtf16(reinterpret_cast<const char16_t *>(ba.constData()), (ba.size() - outBytesLeft) / 2);
}

QString QmmpTextCodec::toUnicode(const char *chars) const
{
    return toUnicode(QByteArray(chars));
}

QByteArray QmmpTextCodec::fromUnicode(const QString &str) const
{
    if(m_name == "UTF-16")
        return QByteArray(reinterpret_cast<const char*>(str.utf16()), str.size() * 2);
    else if(!m_from || m_name == "UTF-8")
        return str.toUtf8();

    size_t inBytesLeft = 0;
    size_t outBytesLeft = 0;

    // reset state
    iconv(m_to, nullptr, &inBytesLeft, nullptr, &outBytesLeft);

    char *inBytes =  const_cast<char *>(reinterpret_cast<const char*>(str.utf16()));
    inBytesLeft = str.size() * 2;
    outBytesLeft = str.size() * 2;

    QByteArray ba(outBytesLeft, Qt::Uninitialized);
    char *outBytes = ba.data();
    outBytesLeft = ba.size();

    while(inBytesLeft > 0)
    {
        size_t ret = iconv(m_to,  &inBytes, &inBytesLeft, &outBytes, &outBytesLeft);

        if(ret == (size_t) -1)
        {
            if(errno == E2BIG) //increase buffer size
            {
                int offset = ba.size() - outBytesLeft;
                ba.resize(ba.size() * 2);
                outBytes = ba.data() + offset;
                outBytesLeft = ba.size() - offset;
                continue;
            }

            if(errno == EINVAL)
                break;
            else if(errno == EILSEQ)
            {
                // skip the next character
                ++inBytes;
                --inBytesLeft;
                continue;
            }

            //fallback
            return str.toLatin1();
        }
    }

    ba.resize(ba.size() - outBytesLeft);

    return ba;
}

const QStringList &QmmpTextCodec::availableCharsets()
{
    static const QStringList charsets = {
        "BIG5",
        "EUC-JP",
        "EUC-KR",
        "GB18030",
        "GBK",
        "IBM866",
        "ISO-2022-JP",
        "ISO-8859-10",
        "ISO-8859-13",
        "ISO-8859-14",
        "ISO-8859-15",
        "ISO-8859-16",
        "ISO-8859-1",
        "ISO-8859-2",
        "ISO-8859-3",
        "ISO-8859-4",
        "ISO-8859-5",
        "ISO-8859-6",
        "ISO-8859-7",
        "ISO-8859-8",
        "ISO-8859-8-I",
        "KOI8-R",
        "KOI8-U",
        "MACINTOSH",
        "SHIFT_JIS",
        "UTF-32",
        "UTF-32LE",
        "UTF-32BE",
        "UTF-16",
        "UTF-16LE",
        "UTF-16BE",
        "UTF-8",
        "WINDOWS-1250",
        "WINDOWS-1251",
        "WINDOWS-1252",
        "WINDOWS-1253",
        "WINDOWS-1254",
        "WINDOWS-1255",
        "WINDOWS-1256",
        "WINDOWS-1257",
        "WINDOWS-1258",
        "WINDOWS-874"
    };

    return charsets;
}
