/***************************************************************************
 *   Copyright (C) 2019-2020 by Ilya Kotov                                 *
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
    LyricsProvider *provider(const QString &name) const;
    static const QStringList &defaultProviders();

private:
    QString m_errorString;
    QList<LyricsProvider *> m_providers;

};

#endif // ULTIMATELYRICSPARSER_H
