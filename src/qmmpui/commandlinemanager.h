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
#ifndef COMMANDLINEMANAGER_H
#define COMMANDLINEMANAGER_H

#include <QHash>
#include "general.h"
#include "commandlinehandler.h"
#include "qmmpui_export.h"


/*! @brief Helper class used for handle command line plugins.
 * @author Ilya Kotov <forkotov02@ya.ru>
 */
class QMMPUI_EXPORT CommandLineManager
{
public:
    /*!
     * Executes command \b opt_str.
     * @param name Command line option name.
     * @param args Command arguments.
     * @return Command output result.
     */
    static QString executeCommand(const QString &name, const QStringList &args = QStringList());
    /*!
     * Return \b true if command \b opt_str is supported, otherwise returns \b false.
     */
    static bool hasOption(const QString &opt_str, CommandLineHandler::OptionFlags *flags = 0);
    /*!
     * Prints usage to stdout.
     */
    static void printUsage();
    /*!
     * Prepares help string for output.
     * @param line Specially formatted help string. Example: "--command||description".
     */
    static QString formatHelpString(const QString &line);

private:
    static void checkOptions();
    static QList<CommandLineHandler *> *m_options;
    static QHash<CommandLineHandler*, QString> *m_files;
};

#endif
