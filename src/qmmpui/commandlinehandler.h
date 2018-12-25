/***************************************************************************
 *   Copyright (C) 2008-2018 by Ilya Kotov                                 *
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
#ifndef COMMANDLINEHANDLER_H
#define COMMANDLINEHANDLER_H

#include <QString>
#include <QMap>
#include <QStringList>
#include <QtPlugin>
#include "qmmpui_export.h"


class CommandLineManager;
class QString;
class QObject;
class QStringList;


/*! @brief Abstract base class of the command line plugins.
 * @author Vladimir Kuznetsov <vovanec@gmail.ru>
 */
class QMMPUI_EXPORT CommandLineHandler
{
public:
    /*!
     * Object destructor
     */
    virtual ~CommandLineHandler() {}
    /*!
     * Returns command line plugin short name for internal usage.
     * Subclass should reimplement this function.
     */
    virtual QString shortName() const = 0;
    /*!
     * Returns translation file path without locale code and extension.
     * Subclass should reimplement this function.
     */
    virtual QString translation() const = 0;
    /*!
     * Executes given command.
     * Subclass should reimplement this function.
     * @param id Command id to execute
     * @param args Command arguments
     * @return Command output
     */
    virtual QString executeCommand(int id, const QStringList &args) = 0;

    QStringList helpString() const;
    int identify(const QString &name) const;

protected:
    void registerOption(int id, const QString &name, const QString &helpString, const QStringList &values = QStringList());
    void registerOption(int id, const QStringList &names, const QString &helpString, const QStringList &values = QStringList());

private:
    struct CommandLineOption
    {
        QStringList names;
        QStringList values;
        QString helpString;

        inline bool operator == (const CommandLineOption &opt) const
        {
            return names == opt.names &&
                    values == opt.values &&
                    helpString == opt.helpString;
        }
    };

    QMap<int, CommandLineOption> m_options;
};

Q_DECLARE_INTERFACE(CommandLineHandler,"CommandLineHandlerInterface/1.0")

#endif
