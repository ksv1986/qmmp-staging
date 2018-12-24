/***************************************************************************
 *   Copyright (C) 2008-2019 by Ilya Kotov                                 *
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

#include "commandlinehandler.h"

QStringList CommandLineHandler::helpString() const
{
    QStringList out;
    foreach (const CommandLineOption &opt, m_options.values())
    {
        if(opt.values.isEmpty())
            out << opt.names.join(", ") + "||" + opt.helpString;
        else
            out << opt.names.join(", ") + " <" + opt.values.join("> <") + ">||" + opt.helpString;
    }
    return out;
}

int CommandLineHandler::identify(const QString &name) const
{
    foreach (const CommandLineOption &opt, m_options.values())
    {
        if(opt.names.contains(name))
            return m_options.key(opt);
    }
    return -1;
}

void CommandLineHandler::registerOption(int id, const QStringList &names, const QString &helpString)
{
    registerOption(id, names, QStringList(), helpString);
}

void CommandLineHandler::registerOption(int id, const QStringList &names, const QStringList &values, const QString &helpString)
{
    CommandLineOption opt;
    opt.names = names;
    opt.values = values;
    opt.helpString = helpString;
    m_options.insert(id, opt);
}
