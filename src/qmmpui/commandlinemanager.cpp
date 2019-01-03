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

#include <QDir>
#include <QPluginLoader>
#include <QObject>
#include <QList>
#include <QApplication>
#include <QTranslator>
#include <cstdlib>
#include <iostream>
#include <qmmp/qmmp.h>
#include <qmmp/soundcore.h>
#include "uihelper.h"
#include "mediaplayer.h"
#include "commandlinemanager.h"

QList<CommandLineHandler *> *CommandLineManager::m_options = nullptr;
QHash<CommandLineHandler*, QString> *CommandLineManager::m_files = nullptr;

void CommandLineManager::checkOptions()
{
    if (!m_options)
    {
        m_options = new QList<CommandLineHandler *>;
        m_files = new QHash<CommandLineHandler*, QString>;

        foreach (QString filePath, Qmmp::findPlugins("CommandLineOptions"))
        {
            QPluginLoader loader(filePath);
            QObject *plugin = loader.instance();
            if (loader.isLoaded())
                /*qDebug("CommandLineManager: loaded plugin %s", qPrintable(QFileInfo(filePath).filePath()));*/;
            else
                qWarning("CommandLineManager: %s", qPrintable(loader.errorString ()));

            CommandLineHandler *option = nullptr;
            if (plugin)
                option = qobject_cast<CommandLineHandler *>(plugin);

            if (option)
            {
                m_options->append(option);
                m_files->insert(option, filePath);
                if(!option->translation().isEmpty())
                {
                    QTranslator *translator = new QTranslator(qApp);
                    translator->load(option->translation() + Qmmp::systemLanguageID());
                    qApp->installTranslator(translator);
                }
                option->registerOprions();
            }
        }
    }
}

QString CommandLineManager::executeCommand(const QString &name, const QStringList &args)
{
    checkOptions();
    bool started = UiHelper::instance() && SoundCore::instance() && MediaPlayer::instance();

    foreach(CommandLineHandler *opt, *m_options)
    {
        int id = opt->identify(name);
        if(id < 0)
            continue;

        if(started || (opt->flags(id) & CommandLineHandler::NO_START))
            return opt->executeCommand(id, args);
        else
        {
            qWarning("CommandLineManager: player objects are not created");
            return QString();
        }


    }
    return QString();
}

bool CommandLineManager::hasOption(const QString &opt_str, CommandLineHandler::OptionFlags *flags)
{
    checkOptions();
    if(flags)
        *flags = nullptr;
    foreach(CommandLineHandler *opt, *m_options)
    {
        int id = opt->identify(opt_str);
        if(id >= 0)
        {
            if(flags)
                *flags = opt->flags(id);
            return true;
        }
    }
    return false;
}

void CommandLineManager::printUsage()
{
    checkOptions();
    foreach(CommandLineHandler *opt, *m_options)
    {
        foreach(QString line, opt->helpString())
        {
            QString str = formatHelpString(line);
            if(!str.isEmpty())
                std::cout << qPrintable(str) << std::endl;
        }
    }
}

QString CommandLineManager::formatHelpString(const QString &line)
{
    QStringList list = line.split("||", QString::SkipEmptyParts);
    if(list.count() == 1)
        return list.at(0);
    else if(list.count() >= 2)
        return list.at(0).leftJustified(25) + list.at(1);
    return QString();
}
