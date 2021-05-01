#ifndef QMMPTEXTCODEC_H
#define QMMPTEXTCODEC_H

#include <QString>
#include <QByteArray>
#include <QStringList>
#include "qmmp_export.h"

typedef void *iconv_t;

class QMMP_EXPORT QmmpTextCodec
{
public:
    QmmpTextCodec(const QString &charset);
    ~QmmpTextCodec();

    const QString &name() const;
    QString toUnicode(const QByteArray &a) const;
    QString toUnicode(const char *chars) const;


    static const QStringList &availableCharsets();

private:
    iconv_t m_to = nullptr, m_from = nullptr;
    QString m_name;
};

#endif // QMMPTEXTCODEC_H
