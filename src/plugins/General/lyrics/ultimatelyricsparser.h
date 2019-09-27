#ifndef ULTIMATELYRICSPARSER_H
#define ULTIMATELYRICSPARSER_H

#include <QString>
#include <QCoreApplication>
#include <QList>
#include "lyricsprovider.h"

class UltimateLyricsParser
{
    Q_DECLARE_TR_FUNCTIONS(UltimateLyricsParser)
public:
    UltimateLyricsParser();
    ~UltimateLyricsParser();

    bool load(const QString &path);
    const QString &errorString() const;
    const QList<LyricsProvider *> &providers();

private:
    QString m_errorString;
    QList<LyricsProvider *> m_providers;

};

#endif // ULTIMATELYRICSPARSER_H
