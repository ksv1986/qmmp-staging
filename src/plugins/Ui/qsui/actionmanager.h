/***************************************************************************
 *   Copyright (C) 2010-2021 by Ilya Kotov                                 *
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

#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include <QObject>
#include <QString>
#include <QHash>
#include <QList>
#include <QToolBar>
#include <utility>

class QAction;
class QSettings;
class QDockWidget;

#define SET_ACTION(type, receiver, member) ActionManager::instance()->use(type, receiver, member)
#define ACTION(type) ActionManager::instance()->action(type)

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class ActionManager : public QObject
{
    Q_OBJECT
public:
    explicit ActionManager(QObject *parent = nullptr);
    ~ActionManager();

    enum Type
    {
        PLAY = 0,
        PAUSE,
        STOP,
        PREVIOUS,
        NEXT,
        PLAY_PAUSE,
        JUMP,
        EJECT,
        RECORD,

        REPEAT_ALL,
        REPEAT_TRACK,
        SHUFFLE,
        NO_PL_ADVANCE,
        TRANSIT_BETWEEN_PLAYLISTS,
        STOP_AFTER_SELECTED,
        CLEAR_QUEUE,

        WM_ALLWAYS_ON_TOP,
        WM_STICKY,
        UI_ANALYZER,              //external
        UI_FILEBROWSER,           //external
        UI_COVER,                 //external
        UI_PLAYLIST_BROWSER,      //external
        UI_WAVEFORM_SEEKBAR,      //external
        UI_SHOW_TABS,
        UI_SHOW_TITLEBARS,
        UI_BLOCK_TOOLBARS,

        VOL_ENC,
        VOL_DEC,
        VOL_MUTE,

        PL_ADD_FILE,
        PL_ADD_DIRECTORY,
        PL_ADD_URL,
        PL_REMOVE_SELECTED,
        PL_REMOVE_ALL,
        PL_REMOVE_UNSELECTED,
        PL_REMOVE_INVALID,
        PL_REMOVE_DUPLICATES,
        PL_REFRESH,
        PL_ENQUEUE,
        PL_INVERT_SELECTION,
        PL_CLEAR_SELECTION,
        PL_SELECT_ALL,
        PL_SHOW_INFO,
        PL_NEW,
        PL_CLOSE,
        PL_LOAD,
        PL_SAVE,
        PL_RENAME,
        PL_SELECT_NEXT,
        PL_SELECT_PREVIOUS,
        PL_GROUP_TRACKS,
        PL_SHOW_HEADER,

        EQUALIZER,
        SETTINGS,
        ABOUT_UI,
        ABOUT,
        ABOUT_QT,
        QUIT,

        //widgets
        UI_POS_SLIDER,            //external
        UI_VOL_SLIDER,            //external
        UI_QUICK_SEARCH,          //external
        UI_SEPARATOR,
    };

    struct ToolBarInfo
    {
        QString title;
        QString uid;
        QStringList actionNames;
        QSize iconSize;
    };

    QAction *action(int type);
    QAction *use(int type, const QObject *receiver, const char *member);
    QList<QAction *> actions() const;
    QList<QDockWidget *> dockWidgtes() const;
    bool hasDockWidgets() const;
    void saveActions();
    void resetShortcuts();
    void registerAction(int id, QAction *action, const QString &confKey, const QString &key);
    void registerWidget(int id, QWidget *w, const QString &text, const QString &name);
    void registerDockWidget(QDockWidget *w, const QString &confKey, const QString &key);
    void removeDockWidget(QDockWidget *w);
    QToolBar *createToolBar(const ToolBarInfo &info, QWidget *parent);
    void updateToolBar(QToolBar *toolBar, const ToolBarInfo &info);
    ActionManager::ToolBarInfo defaultToolBar() const;
    QList<ToolBarInfo> readToolBarSettings() const;
    void writeToolBarSettings(const QList<ToolBarInfo> &l);

    static ActionManager* instance();

private:
    QAction *createAction(const QString &name, const QString &confKey, const QString &key = QString(),
                          const QString &iconName = QString());
    QAction *createAction2(const QString &name, const QString &confKey, const QString &key = QString(),
                           const QString &iconName = QString());
    void readStates();
    void saveStates();

    QSettings *m_settings;
    QHash<int, QAction *> m_actions;
    QHash<QDockWidget *, std::pair<QString, QString>> m_dockWidgets; //widget, key, default shortcut
    static ActionManager *m_instance;

};

#endif // ACTIONMANAGER_H
