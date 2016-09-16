#ifndef ARCHIVETAGREADER_H
#define ARCHIVETAGREADER_H

#include <QIODevice>
#include <QMap>
#include <qmmp/qmmp.h>

class IODeviceStream;

class ArchiveTagReader
{
public:
    ArchiveTagReader(QIODevice *input, const QString &fileName);

    ~ArchiveTagReader();

    //const QMap<Qmmp::MetaData, QString>  metaData () const;



private:
    IODeviceStream *m_stream;
};

#endif // ARCHIVETAGREADER_H
