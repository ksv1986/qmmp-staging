#include "lyricsprovider.h"

LyricsProvider::LyricsProvider()
{

}

void LyricsProvider::setName(const QString &name)
{
    m_name = name;
}

void LyricsProvider::setTitle(const QString &title)
{
    m_title = title;
}

void LyricsProvider::setCharset(const QString &charset)
{
    m_charser = charset;
}

void LyricsProvider::setUrl(const QString &url)
{
    m_url = url;
}

void LyricsProvider::addUrlFormat(const QString &replace, const QString &with)
{
    m_urlFormats << UrlFormat{ .replace = replace, .with = with };
}

void LyricsProvider::addRule(const QList<QPair<QString, QString> > &args, bool exclude)
{
    Rule rule;
    for(const QPair<QString, QString> &i : qAsConst(args))
    {
        Item item;
        if(!i.first.isEmpty() && !i.second.isEmpty())
        {
            item.begin = i.first;
            item.end = i.second;
        }
        else if(i.first.contains("://")) //url
        {
            item.url = i.first;
        }
        else
        {
            item.tag = i.first;
        }
        rule << item;
    }
    if(exclude)
        m_excludeRules << rule;
    else
        m_extractRules << rule;
}

void LyricsProvider::addInvalidIndicator(const QString &indicator)
{
    m_invalidIndicators << indicator;
}
