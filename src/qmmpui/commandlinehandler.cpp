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
        if(opt.flags & HiddenFromHelp)
            continue;
        else if(opt.values.isEmpty())
            out << opt.names.join(", ") + "||" + opt.helpString;
        else
            out << opt.names.join(", ") + " <" + opt.values.join("> <") + ">||" + opt.helpString;
    }
    return out;
}

QString CommandLineHandler::helpString(int id) const
{
    if(m_options[id].values.isEmpty())
        return m_options[id].names.join(", ") + "||" + m_options[id].helpString;
    else
        return m_options[id].names.join(", ") + " <" + m_options[id].values.join("> <") + ">||" + m_options[id].helpString;
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

CommandLineHandler::OptionFlags CommandLineHandler::flags(int id) const
{
    return m_options.value(id).flags;
}

void CommandLineHandler::registerOption(int id, const QString &name, const QString &helpString, const QStringList &values)
{
    registerOption(id, QStringList() << name, helpString, values);
}

void CommandLineHandler::registerOption(int id, const QStringList &names, const QString &helpString, const QStringList &values)
{
    CommandLineOption opt;
    opt.names = names;
    opt.values = values;
    opt.helpString = helpString;
    m_options.insert(id, opt);
}

void CommandLineHandler::setOptionFlags(int id, OptionFlags flags)
{
    m_options[id].flags = flags;
}
