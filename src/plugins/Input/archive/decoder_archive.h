#ifndef DECODERARCHIVE_H
#define DECODERARCHIVE_H

#include <QString>
#include <archive.h>
#include <qmmp/decoder.h>


class DecoderArchive : public Decoder
{
public:
    DecoderArchive(const QString &url);

    virtual ~DecoderArchive();

    bool initialize();
    qint64 totalTime() const;
    void seek(qint64 time);
    qint64 read(unsigned char *data, qint64 maxSize);
    int bitrate() const;

private:
    QString m_url;
    struct archive *m_archive;
    Decoder *m_decoder;
    QIODevice *m_input;

};

#endif // DECODERARCHIVE_H
