/***************************************************************************
 *   Copyright (C) 2019 by Ilya Kotov                                      *
 *   forkotov02@ya.ru                                                      *
 *                                                                         *
 *   Based on Amarok 2 Ultimate Lyrics script                              *
 *   Copyright (C) 2009-2010 Vladimir Brkic <vladimir_brkic@yahoo.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#ifndef LYRICSPROVIDER_H
#define LYRICSPROVIDER_H

#include <QString>
#include <QList>
#include <QMap>

class PlayListTrack;
class TrackInfo;

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
    QString getUrl(const TrackInfo &track) const;
    QString format(const QByteArray &data, const TrackInfo &track) const;
    const QString &name() const;
    void skipRules(bool skip);

private:

    struct UrlFormat
    {
        QString replace, with;
    };

    struct Item
    {
        QString begin, end, tag, url;
    };

    typedef QList<Item> Rule;

    QString fixCase(const QString &title) const;
    QMap<QString, QString> generateReplaceMap(const TrackInfo &track) const;
    QString extract(const QString &content, const Rule &rule) const;
    QString exclude(const QString &content, const Rule &rule) const;

    QString m_name, m_title;
    QString m_charset = QLatin1String("utf-8");
    QString m_url;
    QList<UrlFormat> m_urlFormats;
    QList<Rule> m_extractRules;
    QList<Rule> m_excludeRules;
    QStringList m_invalidIndicators;
    bool m_skipRules = false;
};

#endif // LYRICSPROVIDER_H
