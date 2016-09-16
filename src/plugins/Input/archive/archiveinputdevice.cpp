#include "archiveinputdevice.h"

ArchiveInputDevice::ArchiveInputDevice(archive *a, archive_entry *e, QObject *parent) : QIODevice(parent)
{
    m_archive = a;
    m_entry = e;
    open(QIODevice::ReadOnly);
    m_buffer.open(QBuffer::ReadWrite);
}

bool ArchiveInputDevice::seek(qint64 pos)
{
    QIODevice::seek(pos);

    if(pos > m_buffer.size())
    {
        char tmp[1024];
        qint64 delta = pos - m_buffer.size();
        while (delta > 0)
        {
            qint64 r = qMin(qint64(sizeof(tmp)), delta);
            r = archive_read_data(m_archive, tmp, r);
            m_buffer.buffer().append(tmp, r);
            delta -= r;
        }

    }

    m_buffer.seek(pos);
    return true;
}

qint64 ArchiveInputDevice::size() const
{
    return archive_entry_size(m_entry);
}

qint64 ArchiveInputDevice::readData(char *data, qint64 maxSize)
{
    if(m_buffer.atEnd())
    {
        char tmp[maxSize];
        int r = archive_read_data(m_archive, tmp, maxSize);
        m_buffer.buffer().append(tmp, r);
    }
    return m_buffer.read(data, maxSize);
}

qint64 ArchiveInputDevice::writeData(const char *, qint64)
{
    return -1;
}
