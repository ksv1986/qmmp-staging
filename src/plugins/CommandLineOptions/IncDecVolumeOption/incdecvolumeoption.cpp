/***************************************************************************
 *   Copyright (C) 2008-2021 by Ilya Kotov                                 *
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
#include <QPair>
#include <qmmp/soundcore.h>
#include "incdecvolumeoption.h"

void IncDecVolumeCommandLineOption::registerOprions()
{
    registerOption(VOLUME_UP, "--volume-inc", tr("Increase volume by 5 steps"));
    registerOption(VOLUME_DOWN, "--volume-dec", tr("Decrease volume by 5 steps"));
}

QString IncDecVolumeCommandLineOption::shortName() const
{
    return "IncDecVolumeCommandLineOption";
}

QString IncDecVolumeCommandLineOption::translation() const
{
    return QLatin1String(":/incdecvolume_plugin_");
}

QString IncDecVolumeCommandLineOption::executeCommand(int id, const QStringList &args)
{
    Q_UNUSED(args);

    switch (id)
    {
    case VOLUME_UP:
        SoundCore::instance()->volumeUp();
        break;
    case VOLUME_DOWN:
        SoundCore::instance()->volumeDown();
        break;
    default:
        break;
    }

    return QString();
}
