#ifndef QMMPTEXTCODEC_H
#define QMMPTEXTCODEC_H

#include <QByteArray>
#include <QStringList>
#include "qmmp_export.h"

typedef void *iconv_t;

class QMMP_EXPORT QmmpTextCodec
{
public:
    QmmpTextCodec(const QByteArray &charset);
    ~QmmpTextCodec();

    const QByteArray &name() const;
    QString toUnicode(const QByteArray &a) const;
    QString toUnicode(const char *chars) const;
    QByteArray fromUnicode(const QString &str) const;
    static const QStringList &availableCharsets();

private:
    iconv_t m_to = nullptr, m_from = nullptr;
    QByteArray m_name;
};

#endif // QMMPTEXTCODEC_H
