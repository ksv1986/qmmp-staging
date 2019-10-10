#include <QtDebug>
#include <QRegularExpression>
#include <QTextCodec>
#include <qmmp/trackinfo.h>
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
    m_charset = charset;
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

QString LyricsProvider::getUrl(const TrackInfo &track) const
{
    QString url = m_url;
    const QMap<QString, QString> replaceMap = generateReplaceMap(track);
    QMap<QString, QString>::const_iterator it = replaceMap.constBegin();
    while(it != replaceMap.constEnd())
    {
        QString value = it.value();

        for(const UrlFormat &format: m_urlFormats)
            value.replace(QRegularExpression(QString("[%1]").arg(QRegularExpression::escape(format.replace))), format.with);

        url.replace(it.key(), it.value());

        it++;
    }

    return url;
}

QString LyricsProvider::format(const QByteArray &data, const TrackInfo &track) const
{
    QTextCodec *codec = QTextCodec::codecForName(m_charset.toLatin1().constData());
    if(!codec)
        codec = QTextCodec::codecForName("UTF-8");

    QString content = codec->toUnicode(data);

    for(const QString &indicator : qAsConst(m_invalidIndicators))
    {
        if(content.contains(indicator))
            return QString();
    }

    const QMap<QString, QString> replaceMap = generateReplaceMap(track);

    for(const Rule &rule : qAsConst(m_extractRules))
    {
        Rule tmpRule = rule;

        for(Item &item : tmpRule)
        {
            QMap<QString, QString>::const_iterator it = replaceMap.constBegin();
            while(it != replaceMap.constEnd())
            {
                item.begin.replace(it.key(), it.value());
                item.url.replace(it.key(), it.value());
                it++;
            }
        }

        content = extract(content, rule);
    }

    for(const Rule &rule : qAsConst(m_excludeRules))
    {
        content = exclude(content, rule);
    }

    return content;
}

const QString &LyricsProvider::name() const
{
    return m_name;
}

QString LyricsProvider::fixCase(const QString &title) const
{
    QString out;
    QString::const_iterator it = title.constBegin();
    while (it != title.constEnd())
    {
        if(it == title.constBegin() || (it - 1)->isSpace())
            out.append(it->toUpper());
        else
            out.append(*it);

        it++;
    }

    return out;
}

QMap<QString, QString> LyricsProvider::generateReplaceMap(const TrackInfo &track) const
{
    QMap<QString, QString> replaceMap = {
        { "{artist}", track.value(Qmmp::ARTIST).toLower() },
        { "{artist2}", track.value(Qmmp::ARTIST).toLower().remove(' ') },
        { "{Artist}", track.value(Qmmp::ARTIST) },
        { "{ARTIST}", track.value(Qmmp::ARTIST).toUpper() },
        { "{a}", track.value(Qmmp::ARTIST).left(1).toLower() },
        { "{album}", track.value(Qmmp::ALBUM).toLower() },
        { "{album2}", track.value(Qmmp::ALBUM).toLower().remove(' ') },
        { "{Album}", track.value(Qmmp::ALBUM) },
        { "{title}",  track.value(Qmmp::TITLE).toLower() },
        { "{Title}", track.value(Qmmp::TITLE) },
        { "{Title2}", fixCase(track.value(Qmmp::TITLE)) },
        { "{track}", track.value(Qmmp::TRACK) },
        { "{year}",  track.value(Qmmp::YEAR) }
    };

    return replaceMap;
}

QString LyricsProvider::extract(const QString &content, const Rule &rule) const
{
    QString out = content;

    for(const Item &item : qAsConst(rule))
    {
        if(!item.url.isEmpty())
        {
            QString url = item.url;
            QString id = rule.count() >= 2 ? out.section(rule[1].begin, 1).section(rule[1].end, 0, 0) : QString();
            url.replace("{id}", id);
            return url;
        }
        else if(!item.tag.isEmpty())
        {
            out = out.section(item.tag, 1).section(QString("</") + item.tag.mid(1), 0, 0);
        }
        else
        {
            out = out.section(item.begin, 1).section(item.end, 0, 0);
        }
    }
    return out;
}

QString LyricsProvider::exclude(const QString &content, const LyricsProvider::Rule &rule) const
{
    QString out = content;

    for(const Item &item : qAsConst(rule))
    {
        if(!item.tag.isEmpty())
        {
            out = out.section(item.tag, 0, 0) + out.section(item.tag, 1).section(QString("</") + item.tag.mid(1), 1);
        }
        else
        {
            out = out.section(item.begin, 0, 0) + out.section(item.begin, 1).section(item.end, 1);
        }
    }
    return out;
}
