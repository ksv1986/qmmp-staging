/***************************************************************************
 *   Copyright (C) 2021 by Ilya Kotov                                      *
 *   forkotov02@ya.ru                                                      *
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
