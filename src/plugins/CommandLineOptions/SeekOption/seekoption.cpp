/***************************************************************************
 *   Copyright (C) 2010-2020 by Ilya Kotov                                 *
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

#include <QtPlugin>
#include <QLocale>
#include <QRegularExpression>
#include <qmmp/soundcore.h>
#include "seekoption.h"

void SeekOption::registerOprions()
{
    registerOption(SEEK, "--seek", tr("Seek to position in the current track"), QStringList() << "time");
    registerOption(SEEK_FWD, "--seek-fwd", tr("Seek forward"), QStringList() << "time");
    registerOption(SEEK_BWD, "--seek-bwd", tr("Seek backwards"), QStringList() << "time");
}

QString SeekOption::shortName() const
{
    return QLatin1String("SeekOption");
}

QString SeekOption::translation() const
{
    return QLatin1String(":/seek_plugin_");
}

QString SeekOption::executeCommand(int id, const QStringList &args)
{
    SoundCore *core = SoundCore::instance();
    if(core->state() != Qmmp::Playing && core->duration())
        return QString();
    if(args.isEmpty())
        return QString();

    int seek_pos = -1;
    int elapsed = core->elapsed() / 1000;

    static const QRegularExpression seek_regexp1 ("^([0-9]{1,4})$");
    static const QRegularExpression seek_regexp2 ("^([0-9]{1,2}):([0-9]{1,2})$");

    QRegularExpressionMatch match;
    if((match = seek_regexp1.match(args.first())).hasMatch())
        seek_pos = match.captured(1).toInt();
    else if((match = seek_regexp2.match(args.first())).hasMatch())
        seek_pos = match.captured(1).toInt()*60 + match.captured(2).toInt();

    switch (id) {
    case SEEK: //seek absolute
        break;
    case SEEK_FWD:
        seek_pos += elapsed;
        break;
    case SEEK_BWD:
        seek_pos = elapsed - seek_pos;
        break;
    default:
        break;
    }

    qDebug("SeekOption: position = %d", seek_pos);

    if(seek_pos >= 0 && seek_pos < core->duration())
        core->seek(seek_pos * 1000);

    return QString();
}
