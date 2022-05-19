/***************************************************************************
 *   Copyright (C) 2010-2022 by Ilya Kotov                                 *
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

#ifndef SEEKOPTION_H
#define SEEKOPTION_H

#include <QString>
#include <QObject>
#include <QStringList>
#include <qmmpui/commandlinehandler.h>
#include <qmmpui/commandlinemanager.h>


class SeekOption : public QObject, public CommandLineHandler
{
Q_OBJECT
Q_PLUGIN_METADATA(IID "org.qmmp.qmmpui.CommandLineHandlerInterface.1.0")
Q_INTERFACES(CommandLineHandler)
public:
    void registerOprions() override;
    QString shortName() const override;
    QString translation() const override;
    QString executeCommand(int id, const QStringList &args) override;

private:
    enum Command
    {
        SEEK = 0,
        SEEK_FWD,
        SEEK_BWD
    };
};

#endif
