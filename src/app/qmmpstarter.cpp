/***************************************************************************
 *   Copyright (C) 2006-2017 by Ilya Kotov                                 *
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

#include <QApplication>
#include <QDir>
#include <QLocalServer>
#include <QLocalSocket>
#include <QSettings>
#include <QIcon>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <qmmp/qmmp.h>
#include <qmmpui/commandlinemanager.h>
#include <qmmpui/mediaplayer.h>
#include <qmmpui/playlistparser.h>
#include <qmmpui/uihelper.h>
#include <qmmpui/uiloader.h>
#include <qmmpui/qmmpuisettings.h>
#include "qmmpstarter.h"
#include "lxdesupport.h"
#include "builtincommandlineoption.h"

#ifdef Q_OS_WIN
#include <sstream>
#include <QMessageBox>
#endif

#ifdef Q_OS_WIN
#define UDS_PATH QString("qmmp")
#else
#define UDS_PATH QString("/tmp/qmmp.sock.%1").arg(getuid()).toLatin1().constData()
#endif

using namespace std;

QMMPStarter::QMMPStarter() : QObject()
{
    m_player = 0;
    m_core = 0;
    m_ui = 0;
    m_finished = false;
    m_exit_code = EXIT_SUCCESS;
#ifdef Q_OS_WIN
    m_named_mutex = 0;
#endif
    m_option_manager = new BuiltinCommandLineOption(this);
    QStringList tmp = qApp->arguments().mid(1);

    argString = tmp.join("|||");
    QHash <QString, QStringList> commands = m_option_manager->splitArgs(tmp);

    if(commands.keys().contains("--help"))
    {
        printUsage();
        m_finished = true;
        return;
    }
    if(commands.keys().contains("--version"))
    {
        printVersion();
        m_finished = true;
        return;
    }
    if(commands.keys().contains("--ui-list"))
    {
        printUserInterfaces();
        m_finished = true;
        return;
    }
    if(commands.keys().contains("--ui"))
    {
        QStringList args = commands.value("--ui");
        if(args.size() == 1)
            UiLoader::select(args.first());
    }

    if(!commands.isEmpty())
    {
        foreach(QString arg, commands.keys())
        {
            if(!m_option_manager->identify(arg) && !CommandLineManager::hasOption(arg) &&
                    arg != "--no-start" && arg != "--ui")
            {
                cout << qPrintable(tr("Unknown command")) << endl;
                m_exit_code = EXIT_FAILURE;
                m_finished = true;
                return;
            }
        }
    }

    m_server = new QLocalServer(this);
    m_socket = new QLocalSocket(this);
    bool noStart = commands.keys().contains("--no-start");

#ifdef Q_OS_WIN
    //Windows IPC implementation (named mutex and named pipe)
    m_named_mutex = CreateMutexA(NULL, TRUE, "QMMP-403cd318-cc7b-4622-8dfd-df18d1e70057");
    if(GetLastError() == NO_ERROR && !noStart)
    {
        m_server->listen (UDS_PATH);
        startPlayer();
    }
    else
    {
        m_socket->connectToServer(UDS_PATH); //connecting
        m_socket->waitForConnected();
        if(!m_socket->isValid()) //invalid connection
        {
            qWarning("QMMPStarter: unable to connect to server");
            m_exit_code = EXIT_FAILURE;
            m_finished = true;
            return;
        }
        writeCommand();
    }
#else
    if(!noStart && m_server->listen (UDS_PATH)) //trying to create server
    {
        startPlayer();
    }
    else if(QFile::exists(UDS_PATH))
    {
        m_socket->connectToServer(UDS_PATH); //connecting
        m_socket->waitForConnected();
        if(!m_socket->isValid()) //invalid connection
        {
            if(!QLocalServer::removeServer(UDS_PATH))
            {
                qWarning("QMMPStarter: unable to remove invalid socket file");
                m_exit_code = EXIT_FAILURE;
                m_finished = true;
                return;
            }
            qWarning("QMMPStarter: removed invalid socket file");
            if(noStart)
            {
                m_exit_code = EXIT_FAILURE;
                m_finished = true;
                return;
            }
            else if(m_server->listen (UDS_PATH))
                startPlayer();
            else
            {
                qWarning("QMMPStarter: server error: %s", qPrintable(m_server->errorString()));
                m_exit_code = EXIT_FAILURE;
                m_finished = true;
                return;
            }
        }
        else
            writeCommand();
    }
    else
        m_finished = true;
#endif
}

QMMPStarter::~QMMPStarter()
{
    if (m_ui)
        delete m_ui;
#ifdef Q_OS_WIN
    if(m_named_mutex)
        ReleaseMutex(m_named_mutex);
#endif
}

bool QMMPStarter::isFinished() const
{
    return m_finished;
}

int QMMPStarter::exitCode() const
{
    return m_exit_code;
}

void QMMPStarter::startPlayer()
{
    connect(m_server, SIGNAL(newConnection()), SLOT(readCommand()));
    QStringList args = argString.split("|||", QString::SkipEmptyParts);

#ifdef Q_OS_WIN
    QIcon::setThemeSearchPaths(QStringList() << qApp->applicationDirPath() + "/themes/");
    QIcon::setThemeName("oxygen");
#else
    //add extra theme path;
    QStringList theme_paths = QIcon::themeSearchPaths();
    QString share_path = qgetenv("XDG_DATA_HOME");
    if(share_path.isEmpty())
        share_path = QDir::homePath() + "/.local/share";
    theme_paths << share_path + "/icons";
    theme_paths.removeDuplicates();
    QIcon::setThemeSearchPaths(theme_paths);

    //load lxde icons
    LXDESupport::load();
#endif

    //prepare libqmmp and libqmmpui libraries for usage
    m_player = new MediaPlayer(this);
    m_core = SoundCore::instance();

    //additional featuries
    new UiHelper(this);

    //interface
    UiFactory *factory = UiLoader::selected();
    if(factory)
        m_ui = factory->create();
    else
    {
        qWarning("QMMPStarter: no user interface found");
        m_finished = true;
        m_exit_code = EXIT_FAILURE;
        return;
    }
    connect(qApp, SIGNAL(aboutToQuit()), SLOT(savePosition()));
    processCommandArgs(args, QDir::currentPath());
    if(args.isEmpty())
    {
        QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
        settings.beginGroup("General");
        if(settings.value("resume_playback", false).toBool())
        {
            qint64 pos =  settings.value("resume_playback_time").toLongLong();
            m_player->play(pos);
        }
    }
}

void QMMPStarter::savePosition()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("General");
    settings.setValue("resume_playback",m_core->state() == Qmmp::Playing &&
                      QmmpUiSettings::instance()->resumeOnStartup());
    settings.setValue("resume_playback_time", m_core->totalTime() > 0 ? m_core->elapsed() : 0);
    settings.endGroup();
    m_core->stop();
}

void QMMPStarter::writeCommand()
{
    QString workingDir = QDir::currentPath() + "|||";
    QByteArray barray;
    barray.append(workingDir.toUtf8 ());
    barray.append(argString.isEmpty() ? "--show-mw" : argString.toUtf8 ());
    while(!barray.isEmpty())
    {
        qint64 size = m_socket->write(barray);
        barray.remove(0, size);
    }
    m_socket->flush();
    //reading answer
    while(m_socket->waitForReadyRead(1500))
        cout << m_socket->readAll().data();

#ifndef Q_OS_WIN
    if (argString.isEmpty())
        printUsage();
#endif

    m_finished = true;
}

void QMMPStarter::readCommand()
{
    QLocalSocket *socket = m_server->nextPendingConnection();
    socket->waitForReadyRead();
    QByteArray inputArray = socket->readAll();
    if(inputArray.isEmpty())
    {
        socket->deleteLater();
        return;
    }
    QStringList slist = QString::fromUtf8(inputArray.data()).split("|||",QString::SkipEmptyParts);
    QString cwd = slist.takeAt(0);
    QString out = processCommandArgs(slist, cwd);
    if(!out.isEmpty())
    {
        //writing answer
        socket->write(out.toLocal8Bit());
        while(socket->waitForBytesWritten())
            socket->flush();
    }
    socket->deleteLater();
}

QString QMMPStarter::processCommandArgs(const QStringList &slist, const QString& cwd)
{
    if(slist.isEmpty())
        return QString();
    QStringList paths;
    foreach(QString arg, slist) //detect file/directory paths
    {
        if(arg.startsWith("-"))
            break;
        paths.append(arg);
    }
    if(!paths.isEmpty())
    {
        return m_option_manager->executeCommand(QString(), paths, cwd); //add paths only
    }
    QHash<QString, QStringList> commands = m_option_manager->splitArgs(slist);
    if(commands.isEmpty())
        return QString();

    QString out;
    foreach(QString key, commands.keys())
    {
        if(key == "--no-start" || key == "--ui")
            continue;
        if (CommandLineManager::hasOption(key))
            return CommandLineManager::executeCommand(key, commands.value(key));
        else if (m_option_manager->identify(key))
            out += m_option_manager->executeCommand(key, commands.value(key), cwd);
        else
            return QString();
    }
    return out;
}

void QMMPStarter::printUsage()
{
//show dialog with command line documentation under ms windows
#ifdef Q_OS_WIN
    stringstream tmp_stream;
    tmp_stream.copyfmt(cout);
    streambuf* old_stream = cout.rdbuf(tmp_stream.rdbuf());
#endif
    cout << qPrintable(tr("Usage: qmmp [options] [files]")) << endl;
    cout << qPrintable(tr("Options:")) << endl;
    cout << "--------" << endl;
    foreach (QString line, m_option_manager->helpString())
        cout << qPrintable(CommandLineManager::formatHelpString(line) ) << endl;
    CommandLineManager::printUsage();
    QStringList extraHelp;
    extraHelp << QString("--ui <name>") + "||" + tr("Start qmmp with the specified user interface");
    extraHelp << QString("--ui-list") + "||" + tr("List all available user interfaces");
    extraHelp << QString("--no-start") + "||" + tr("Don't start the application");
    extraHelp << QString("--help") + "||" + tr("Display this text and exit");
    extraHelp << QString("--version") + "||" + tr("Print version number and exit");
    extraHelp << "";
    extraHelp << tr("Home page: %1").arg("http://qmmp.ylsoftware.com");
    extraHelp << tr("Development page: %1").arg("https://sourceforge.net/p/qmmp-dev");
    extraHelp << tr("Bug tracker: %1").arg("https://sourceforge.net/p/qmmp-dev/tickets");
    foreach (QString line, extraHelp)
        cout << qPrintable(CommandLineManager::formatHelpString(line)) << endl;
#ifdef Q_OS_WIN
    string text = tmp_stream.str();
    QMessageBox::information(0, tr("Command Line Help"), QString::fromLocal8Bit(text.c_str()));
    cout.rdbuf(old_stream); //restore old stream buffer
#endif
}

void QMMPStarter::printVersion()
{
    //show dialog with qmmp version under ms windows
#ifdef Q_OS_WIN
    stringstream tmp_stream;
    tmp_stream.copyfmt(cout);
    streambuf* old_stream = cout.rdbuf(tmp_stream.rdbuf());
#endif
    cout << qPrintable(tr("QMMP version: %1").arg(Qmmp::strVersion())) << endl;
    cout << qPrintable(tr("Compiled with Qt version: %1").arg(QT_VERSION_STR)) << endl;
    cout << qPrintable(tr("Using Qt version: %1").arg(qVersion())) << endl;
#ifdef Q_OS_WIN
    string text = tmp_stream.str();
    QMessageBox::information(0, tr("Qmmp Version"), QString::fromLocal8Bit(text.c_str()));
    cout.rdbuf(old_stream); //restore old stream buffer
#endif
}

void QMMPStarter::printUserInterfaces()
{
    //show dialog with qmmp version under ms windows
#ifdef Q_OS_WIN
    stringstream tmp_stream;
    tmp_stream.copyfmt(cout);
    streambuf* old_stream = cout.rdbuf(tmp_stream.rdbuf());
#endif
    foreach (QString name, UiLoader::names())
        cout << qPrintable(name) << endl;
#ifdef Q_OS_WIN
    string text = tmp_stream.str();
    QMessageBox::information(0, tr("User Interfaces"), QString::fromLocal8Bit(text.c_str()));
    cout.rdbuf(old_stream); //restore old stream buffer
#endif
}
