/***************************************************************************
 *   Copyright (C) 2008-2017 by Ilya Kotov                                 *
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
#ifndef COMMANDLINEOPTION_H
#define COMMANDLINEOPTION_H

#include "qmmpui_export.h"

class CommandLineManager;
class QString;
class QObject;
class QStringList;

/*! @brief Helper class to store command line plugin properties.
 * @author Ilya Kotov <forkotov02@ya.ru>
 */
class CommandLineProperties
{
public:
    QString shortName;      /*!< Input plugin short name for internal usage */
    QStringList helpString; /*!< A list of specially formatted help strings.
                                 Example: "--help||Display this text and exit" */
};

/*! @brief Abstract base class of the command line plugins.
 * @author Vladimir Kuznetsov <vovanec@gmail.ru>
 */
class QMMPUI_EXPORT CommandLineOption
{
public:
    /*!
     * Returns command line plugin properties.
     */
    virtual CommandLineProperties properties() const = 0;
    /*!
     * Returns \b true if \b opt_str string can be processed,
     * otherise \b false
     */
    virtual bool identify(const QString& opt_str) const = 0;
    /*!
     * Parses \b opt_str args(if needed), executes command.
     * @param opt_str Command to execute
     * @param args Command arguments
     * @return Command output
     */
    virtual QString executeCommand(const QString &opt_str, const QStringList &args) = 0;
    /*!
     * Object destructor
     */
    virtual ~CommandLineOption() {}
    /*!
     * Returns translation file path without locale code and extension
     */
    virtual QString translation() const = 0;
};

Q_DECLARE_INTERFACE(CommandLineOption,"CommandLineOptionInterface/1.0")

#endif
