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

QString LyricsProvider::format(const QByteArray &data) const
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
