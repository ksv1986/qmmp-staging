/***************************************************************************
 *   Copyright (C) 2008-2020 by Ilya Kotov                                 *
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
#include <QFlags>
#include "qmmpui_export.h"

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
     * Registers command line options. Subclass should implement this function
     */
    virtual void registerOprions() = 0;
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
     * @param id Command identifier to execute
     * @param args Command arguments
     * @return Command output
     */
    virtual QString executeCommand(int id, const QStringList &args) = 0;
    /*!
     * Returns a list of help strings.
     */
    QStringList helpString() const;
    /*!
     * Returns help string for specified option.
     * \param id Command line option identifier.
     * \return
     */
    QString helpString(int id) const;
    /*!
     * Returtns identifier for specified option name or \b -1
     * if option is not supported.
     * \param name Command line option name.
     */
    int identify(const QString &name) const;
    /*!
     * These flags describe attributes that change the behavior of command line option
     * processing.
     */
    enum OptionFlag
    {
        HiddenFromHelp = 0x1,  /*!< Hide this option in the user-visible help output. */
        NoStart = 0x2          /*!< Do not start player instance (useful for extra help options). */
    };
    Q_DECLARE_FLAGS(OptionFlags, OptionFlag)
    /*!
     * Returtns flags for specified option identifier
     * \param id Command line option identifier.
     */
    CommandLineHandler::OptionFlags flags(int id) const;

protected:
    /*!
     * Registers command line option. This function should be called inside \b registerOprions() implementation.
     * \param id Option identifier.
     * \param name Option name.
     * \param helpString Option description.
     * \param values Value name list (optional).
     */
    void registerOption(int id, const QString &name, const QString &helpString, const QStringList &values = QStringList());
    /*!
     * Registers command line option. This function should be called inside \b registerOprions() implementation.
     * \param id Option identifier.
     * \param names Option names (i.e. short and long variants).
     * \param helpString Option description.
     * \param values Value name list (optional).
     */
    void registerOption(int id, const QStringList &names, const QString &helpString, const QStringList &values = QStringList());
    /*!
     * Changes command line option flags.
     * \param id Option identifier.
     * \param flags Option flags.
     */
    void setOptionFlags(int id, OptionFlags flags);

private:
    struct CommandLineOption
    {
        QStringList names;
        QStringList values;
        QString helpString;
        OptionFlags flags;

        inline bool operator == (const CommandLineOption &opt) const
        {
            return names == opt.names &&
                    values == opt.values &&
                    helpString == opt.helpString &&
                    flags == opt.flags;
        }
    };

    QMap<int, CommandLineOption> m_options;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(CommandLineHandler::OptionFlags)
Q_DECLARE_INTERFACE(CommandLineHandler,"CommandLineHandlerInterface/1.0")

#endif
