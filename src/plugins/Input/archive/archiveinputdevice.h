#ifndef ARCHIVEINPUTDEVICE_H
#define ARCHIVEINPUTDEVICE_H

#include <QIODevice>
#include <QBuffer>
#include <archive.h>
#include <archive_entry.h>

class ArchiveInputDevice : public QIODevice
{
    Q_OBJECT
public:
    ArchiveInputDevice(struct archive *a, struct archive_entry *e, QObject *parent);

    bool seek(qint64 pos);
    qint64 size() const;

protected:
    virtual qint64 readData(char *data, qint64 maxSize);
    virtual qint64 writeData(const char *, qint64);

private:
    struct archive *m_archive;
    struct archive_entry *m_entry;
    QBuffer m_buffer;

};

#endif // ARCHIVEINPUTDEVICE_H
