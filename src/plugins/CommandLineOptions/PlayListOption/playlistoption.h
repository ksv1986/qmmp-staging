/***************************************************************************
 *   Copyright (C) 2011-2021 by Ilya Kotov                                 *
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

#ifndef PLAYLISTOPTION_H
#define PLAYLISTOPTION_H

#include <QString>
#include <QObject>
#include <QStringList>
#include <qmmpui/commandlinehandler.h>
#include <qmmpui/commandlinemanager.h>

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class PlayListOption : public QObject, public CommandLineHandler
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
        PL_HELP = 0,
        PL_LIST,
        PL_DUMP,
        PL_PLAY,
        PL_NEXT,
        PL_PREV,
        PL_CLEAR,
        PL_REPEATE_TOGGLE,
        PL_SHUFFLE_TOGGLE,
        PL_STATE
    };

    QString boolToText(bool enabled);
};

#endif
