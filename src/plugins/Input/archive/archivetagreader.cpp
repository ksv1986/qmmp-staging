#include <taglib/tiostream.h>
#include <taglib/fileref.h>
#include "archivetagreader.h"

class IODeviceStream : public TagLib::IOStream
{
public:
    IODeviceStream(QIODevice *input, const QString &fileName)
    {
        m_input = input;
        m_fileName = fileName;
    }

    virtual ~IODeviceStream() {}

    virtual TagLib::FileName name() const
    {
        return QStringToFileName(m_fileName);
    }
    virtual TagLib::ByteVector readBlock(unsigned long length)
    {
        QByteArray data = m_input->read(length);
        return TagLib::ByteVector(data.constData(), data.size());
    }
    virtual void writeBlock(const TagLib::ByteVector &)
    {}
    virtual void insert(const TagLib::ByteVector &, unsigned long, unsigned long)
    {}
    virtual void removeBlock(unsigned long, unsigned long)
    {}
    virtual bool readOnly() const
    {
        return true;
    }
    virtual bool isOpen() const
    {
        return m_input->isOpen();
    }
    virtual void seek(long offset, Position p = Beginning)
    {
        switch (p)
        {
        case Beginning:
            m_input->seek(offset);
            return;
        case Current:
            m_input->seek(m_input->pos() + offset);
            return;
        case End:
            m_input->seek(m_input->size() - offset);
            return;
        }
        return;
    }


    virtual void clear()
    {
        m_input->reset();
        TagLib::IOStream::clear();
    }
    virtual long tell() const
    {
        return m_input->pos();
    }
    virtual long length()
    {
        return m_input->size();
    }
    virtual void truncate(long)
    {}

private:
    QString m_fileName;
    QIODevice *m_input;

};

ArchiveTagReader::ArchiveTagReader(QIODevice *input, const QString &fileName)
{
    m_stream = new IODeviceStream(input, fileName);
}

ArchiveTagReader::~ArchiveTagReader()
{
    delete m_stream;
}

/*const QMap<Qmmp::MetaData, QString> ArchiveTagReader::metaData() const
{
    TagLib::FileRef file(m_stream);
    QMap<Qmmp::MetaData, QString> m;

    m[Qmmp::ALBUM, QString::fromUtf8(tag->album().toCString(true)).trimmed()];
    m[Qmmp::ARTIST, QString::fromUtf8(tag->artist().toCString(true)).trimmed()];
    m[Qmmp::COMMENT, QString::fromUtf8(tag->comment().toCString(true)).trimmed()];
    m[Qmmp::GENRE, QString::fromUtf8(tag->genre().toCString(true)).trimmed()];
    m[Qmmp::TITLE, QString::fromUtf8(tag->title().toCString(true)).trimmed()];
    m[Qmmp::YEAR, tag->year());
    m[Qmmp::TRACK, tag->track());

    return m;
}*/
