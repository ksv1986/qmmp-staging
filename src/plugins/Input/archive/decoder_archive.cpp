#include <QFile>
#include <archive_entry.h>
#include "archiveinputdevice.h"
#include "decoder_archive.h"

DecoderArchive::DecoderArchive(const QString &url)
{
    m_url = url;
    m_archive = 0;
    m_decoder = 0;
    m_input = 0;
}

DecoderArchive::~DecoderArchive()
{
    if(m_decoder)
    {
        delete m_decoder;
        m_decoder = 0;
    }
    if(m_input)
    {
        delete m_input;
        m_input = 0;
    }
    if(m_archive)
    {
        archive_read_close(m_archive);
        archive_read_free(m_archive);
        m_archive = 0;
    }
}

bool DecoderArchive::initialize()
{
    QString filePath = m_url.section("#", -1);
    QString archivePath = m_url;
    archivePath.remove(QRegExp("^.+://"));
    archivePath.remove(QRegExp("#.+$"));

    if(!QFile::exists(archivePath))
    {
        qWarning("DecoderArc: file '%s' not found", qPrintable(archivePath));
        return false;
    }

    QList<DecoderFactory*> filtered;
    foreach (DecoderFactory *fact, Decoder::enabledFactories())
    {
        if(fact->properties().noInput)
            continue;

        foreach(QString filter, fact->properties().filters)
        {
            QRegExp regexp(filter, Qt::CaseInsensitive, QRegExp::Wildcard);
            if (regexp.exactMatch(filePath))
            {
                filtered.append(fact);
                break;
            }
        }
    }

    if(filtered.isEmpty())
    {
        qWarning("DecoderArc: unable to find decoder");
        return false;
    }

    m_archive = archive_read_new();
    archive_read_support_filter_all(m_archive);
    archive_read_support_format_all(m_archive);

    int r = archive_read_open_filename(m_archive, archivePath.toLocal8Bit().constData(), 10240);
    if (r != ARCHIVE_OK)
    {
        qWarning("DecoderArc: unable to open file '%s', error code: %d", qPrintable(archivePath), r);
        return false;
    }

    struct archive_entry *entry = 0;

    while (archive_read_next_header(m_archive, &entry) == ARCHIVE_OK)
    {
        QString pathName = QString::fromLocal8Bit(archive_entry_pathname(entry));
        if(!pathName.startsWith("/"))
            pathName.prepend("/");

        if(archive_entry_filetype(entry) == AE_IFREG && filePath == pathName)
        {
            m_input = new ArchiveInputDevice(m_archive, entry, 0);
            break;
        }
        archive_read_data_skip(m_archive);
    }

    if(!m_input)
    {
        qWarning("DecoderArc: unable to find file '%s' inside archive '%s'", qPrintable(filePath),
                 qPrintable(archivePath));
        return false;
    }

    DecoderFactory *factory = 0;
    if(filtered.size() == 1)
        factory = filtered.first();
    else
    {
        //several factories, so trying to determine by content
        foreach (DecoderFactory *fact, filtered)
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
        qWarning("DecoderArc: unable to find supported decoder factory");
        return false;
    }

    qDebug("DecoderArc: selected decoder: %s", qPrintable(factory->properties().shortName));
    qDebug("+");
    m_decoder = factory->create(m_url, m_input);
    qDebug("+1");
    if(!m_decoder->initialize())
    {
        qWarning("DecoderArc: unable to initialize decoder");
        return false;
    }
    qDebug("+2");
    configure(m_decoder->audioParameters().sampleRate(), m_decoder->audioParameters().channelMap(),
              m_decoder->audioParameters().format());
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
