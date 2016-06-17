/***************************************************************************
 *   Copyright (C) 2016 by Ilya Kotov                                      *
 *   forkotov02@hotmail.ru                                                 *
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

#include <QProcess>
#include <QIcon>
#include "unitysupport.h"

void UnitySupport::load()
{
    if(qgetenv("XDG_CURRENT_DESKTOP") != "Unity" && qgetenv("XDG_CURRENT_DESKTOP") != "GNOME")
        return;

    QProcess process;
    process.start("dconf read /org/gnome/desktop/interface/icon-theme");
    process.waitForFinished(3000);
    QString iconTheme = QString::fromLocal8Bit(process.readAllStandardOutput().trimmed());
    if(!iconTheme.isEmpty())
        QIcon::setThemeName(iconTheme.remove("'"));
}
