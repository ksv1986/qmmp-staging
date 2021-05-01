#include <iconv.h>
#include <QtDebug>
#include "qmmptextcodec.h"

QmmpTextCodec::QmmpTextCodec(const QString &charset) : m_name(charset.toUpper())
{
    qDebug() << Q_FUNC_INFO << charset;

    if((m_to = iconv_open(charset.toLatin1().constData(), "UTF-16")) == (iconv_t)(-1))
    {
        qWarning("QmmpTextCodec: error: %s", strerror(errno));
        m_to = nullptr;
    }

    if((m_from = iconv_open("UTF-16", charset.toUpper().toLatin1().constData())) == (iconv_t)(-1))
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

const QString &QmmpTextCodec::name() const
{
    return m_name;
}

QString QmmpTextCodec::toUnicode(const QByteArray &a) const
{
    if(!m_from)
        return QString::fromUtf8(a);

    int invalidCount = 0;
    int remainingCount = 0;
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

    qDebug() << inBytesLeft << outBytesLeft;

    while(inBytesLeft > 0)
    {
        size_t ret = iconv(m_from,  &inBytes, &inBytesLeft, &outBytes, &outBytesLeft);

        if(ret == (size_t) -1)
        {


            if(errno == E2BIG)
            {
                int offset = ba.size() - outBytesLeft;
                ba.resize(ba.size() * 2);
                outBytes = ba.data() + offset;
                outBytesLeft = ba.size() - offset;
                continue;
            }

            if(errno == EILSEQ)
            {
                // conversion stopped because of an invalid character in the sequence
                ++invalidCount;
            }
            else if (errno == EINVAL)
            {
                // conversion stopped because the remaining inBytesLeft make up
                // an incomplete multi-byte sequence; save them for later
                //state->saveChars(inBytes, inBytesLeft);
                remainingCount = inBytesLeft;
                break;
            }

            if (errno == EILSEQ || errno == EINVAL)
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

    qDebug() << "!" << inBytesLeft << (ba.size() - outBytesLeft) / 2;

    return QString::fromUtf16((const char16_t *)ba.constData(), (ba.size() - outBytesLeft) / 2);
}

QString QmmpTextCodec::toUnicode(const char *chars) const
{
    return toUnicode(QByteArray(chars));
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
