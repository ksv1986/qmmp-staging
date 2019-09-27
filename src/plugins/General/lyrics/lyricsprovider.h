#ifndef LYRICSPROVIDER_H
#define LYRICSPROVIDER_H

#include <QString>
#include <QList>
#include <QPair>

class LyricsProvider
{
public:
    LyricsProvider();

    void setName(const QString &name);
    void setTitle(const QString &title);
    void setCharset(const QString &charset);
    void setUrl(const QString &url);
    void addUrlFormat(const QString &replace, const QString &with);
    void addRule(const QList<QPair<QString, QString> > &args, bool exclude = false);
    void addInvalidIndicator(const QString &indicator);

private:
    QString m_name, m_title;
    QString m_charser = QLatin1String("utf-8");
    QString m_url;

    struct UrlFormat
    {
        QString replace, with;
    };

    struct Item
    {
        QString begin, end, tag, url;
    };

    typedef QList<Item> Rule;

    QList<UrlFormat> m_urlFormats;
    QList<Rule> m_extractRules;
    QList<Rule> m_excludeRules;
    QStringList m_invalidIndicators;

};

#endif // LYRICSPROVIDER_H
