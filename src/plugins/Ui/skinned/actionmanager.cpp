/***************************************************************************
 *   Copyright (C) 2010-2016 by Ilya Kotov                                 *
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

#include <QSettings>
#include <QAction>
#include <QIcon>
#include <QFile>
#include <QApplication>
#include <qmmp/qmmp.h>
#include "actionmanager.h"

ActionManager *ActionManager::m_instance = nullptr;

ActionManager::ActionManager(QObject *parent) :
    QObject(parent)
{
    m_instance = this;
    m_settings = new QSettings(Qmmp::configFile(), QSettings::IniFormat);
    m_settings->beginGroup("SkinnedShortcuts");
    m_actions = {
        //playback
        { PLAY, createAction(tr("&Play"), "play", tr("X"), "media-playback-start") },
        { PAUSE, createAction(tr("&Pause"), "pause", tr("C"), "media-playback-pause") },
        { STOP, createAction(tr("&Stop"), "stop", tr("V"), "media-playback-stop") },
        { PREVIOUS, createAction(tr("&Previous"), "previous", tr("Z"), "media-skip-backward") },
        { NEXT, createAction(tr("&Next"), "next", tr("B"), "media-skip-forward") },
        { PLAY_PAUSE, createAction(tr("&Play/Pause"), "play_pause", tr("Space")) },
        { JUMP, createAction(tr("&Jump to Track"), "jump", tr("J"), "go-up") },
        { REPEAT_ALL, createAction2(tr("&Repeat Playlist"), "repeate_playlist", tr("R")) },
        { REPEAT_TRACK, createAction2(tr("&Repeat Track"), "repeate_track", tr("Ctrl+R")) },
        { SHUFFLE, createAction2(tr("&Shuffle"), "shuffle", tr("S")) },
        { NO_PL_ADVANCE, createAction2(tr("&No Playlist Advance"), "no_playlist_advance", tr("Ctrl+N")) },
        { STOP_AFTER_SELECTED, createAction(tr("&Stop After Selected"), "stop_after_selected", tr("Ctrl+S")) },
        { TRANSIT_BETWEEN_PLAYLISTS, createAction2(tr("&Transit between playlists"), "transit_between_playlists") },
        { CLEAR_QUEUE, createAction(tr("&Clear Queue"), "clear_queue", tr("Alt+Q")) },
        //view
        { SHOW_PLAYLIST, createAction2(tr("Show Playlist"), "show_playlist", tr("Alt+E")) },
        { SHOW_EQUALIZER, createAction2(tr("Show Equalizer"), "show_equalizer", tr("Alt+G")) },
        { WM_ALLWAYS_ON_TOP, createAction2(tr("Always on Top"), "always_on_top") },
        { WM_STICKY, createAction2(tr("Put on All Workspaces"), "sticky") },
        { WM_DOUBLE_SIZE, createAction2(tr("Double Size"), "double_size", tr("Meta+D")) },
        { WM_ANTIALIASING, createAction2(tr("Anti-aliasing"), "anti_aliasing") },
        //volume
        { VOL_ENC, createAction(tr("Volume &+"), "vol_enc", tr("0")) },
        { VOL_DEC, createAction(tr("Volume &-"), "vol_dec", tr("9")) },
        { VOL_MUTE, createAction2(tr("&Mute"), "vol_mute", tr("M")) },
        //playlist
        { PL_ADD_FILE, createAction(tr("&Add File"), "add_file", tr("F"), "audio-x-generic") },
        { PL_ADD_DIRECTORY, createAction(tr("&Add Directory"), "add_dir", tr("D"), "folder") },
        { PL_ADD_URL, createAction(tr("&Add Url"), "add_url", tr("U"), "network-server") },
        { PL_REMOVE_SELECTED, createAction(tr("&Remove Selected"), "remove_selected", tr("Del"), "edit-delete") },
        { PL_REMOVE_ALL, createAction(tr("&Remove All"), "remove_all", "", "edit-clear") },
        { PL_REMOVE_UNSELECTED, createAction(tr("&Remove Unselected"), "remove_unselected","", "edit-delete") },
        { PL_REMOVE_INVALID, createAction(tr("Remove unavailable files"), "remove_invalid", "", "dialog-error") },
        { PL_REMOVE_DUPLICATES, createAction(tr("Remove duplicates"), "remove_duplicates") },
        { PL_REFRESH, createAction(tr("Refresh"), "refresh", "F5", "view-refresh") },
        { PL_ENQUEUE, createAction(tr("&Queue Toggle"), "enqueue", tr("Q")) },
        { PL_INVERT_SELECTION, createAction(tr("Invert Selection"), "invert_selection") },
        { PL_CLEAR_SELECTION, createAction(tr("&Select None"), "clear_selection") },
        { PL_SELECT_ALL, createAction(tr("&Select All"), "select_all", tr("Ctrl+A"), "edit-select-all") },
        { PL_SHOW_INFO, createAction(tr("&View Track Details"), "show_info", tr("Alt+I"), "dialog-information") },
        { PL_NEW, createAction(tr("&New List"), "new_pl", tr("Ctrl+T"), "document-new") },
        { PL_CLOSE, createAction(tr("&Delete List"), "close_pl", tr("Ctrl+W"), "window-close") },
        { PL_LOAD, createAction(tr("&Load List"), "load_pl", tr("O"), "document-open") },
        { PL_SAVE, createAction(tr("&Save List"), "save_pl", tr("Shift+S"), "document-save-as") },
        { PL_RENAME, createAction(tr("&Rename List"), "pl_rename", tr("F2")) },
        { PL_SELECT_NEXT, createAction(tr("&Select Next Playlist"), "next_pl", tr("Ctrl+PgDown"), "go-next") },
        { PL_SELECT_PREVIOUS, createAction(tr("&Select Previous Playlist"), "prev_pl", tr("Ctrl+PgUp"), "go-previous") },
        { PL_SHOW_MANAGER, createAction(tr("&Show Playlists"), "show_pl_manager", tr("P"), "view-list-details") },
        { PL_GROUP_TRACKS, createAction2(tr("&Group Tracks"), "group_tracks", tr("Ctrl+G")) },
        { PL_SHOW_HEADER, createAction2(tr("&Show Column Headers"), "show_header", tr("Ctrl+H")) },
        { PL_SHOW_TABBAR, createAction2(tr("Show &Tab Bar"), "show_pl_tabbar", tr("Alt+T")) },
        //other
        { SETTINGS, createAction(tr("&Settings"), "show_settings", tr("Ctrl+P"), "configure") },
        { ABOUT, createAction(tr("&About"), "about") },
        { ABOUT_QT, createAction(tr("&About Qt"), "about_qt") },
        { QUIT, createAction(tr("&Exit"), "exit", tr("Ctrl+Q"), "application-exit") },
      };
    m_settings->endGroup();
    readStates();
    delete m_settings;
    m_settings = nullptr;
    m_actions[ABOUT]->setIcon(qApp->windowIcon());
    connect(m_actions[WM_DOUBLE_SIZE], SIGNAL(toggled(bool)), m_actions[WM_ANTIALIASING], SLOT(setEnabled(bool)));
    m_actions[WM_ANTIALIASING]->setEnabled(false);
}

ActionManager::~ActionManager()
{
    saveStates();
    m_instance = nullptr;
}

QAction *ActionManager::action(int type)
{
    return m_actions[type];
}

QAction *ActionManager::use(int type, const QObject *receiver, const char *member)
{
    QAction *act = m_actions[type];
    connect(act,SIGNAL(triggered(bool)), receiver, member);
    return act;
}

QList<QAction *> ActionManager::actions()
{
    return m_actions.values();
}

ActionManager* ActionManager::instance()
{
    return m_instance;
}

QAction *ActionManager::createAction(QString name, QString confKey, QString key, QString iconName)
{
    QAction *action = new QAction(name, this);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    action->setShortcutVisibleInContextMenu(true);
#endif
    action->setShortcut(m_settings->value(confKey, key).toString());
    action->setProperty("defaultShortcut", key);
    action->setObjectName(confKey);
    if(iconName.isEmpty())
        return action;
    if(QFile::exists(iconName))
        action->setIcon(QIcon(iconName));
    else
        action->setIcon(QIcon::fromTheme(iconName));
    return action;
}

QAction *ActionManager::createAction2(QString name, QString confKey, QString key)
{
    QAction *action = createAction(name, confKey, key);
    action->setCheckable(true);
    return action;
}

void ActionManager::readStates()
{
    m_settings->beginGroup("Skinned");
    m_actions[PL_SHOW_HEADER]->setChecked(m_settings->value("pl_show_header", false).toBool());
    m_actions[PL_SHOW_TABBAR]->setChecked(m_settings->value("pl_show_tabbar", false).toBool());
    m_settings->endGroup();
}

void ActionManager::saveStates()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Skinned");
    settings.setValue("pl_show_header", m_actions[PL_SHOW_HEADER]->isChecked());
    settings.setValue("pl_show_tabbar", m_actions[PL_SHOW_TABBAR]->isChecked());
    settings.endGroup();
}

void ActionManager::saveActions()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    for(const QAction *action : m_actions.values())
    {
        settings.setValue(QString("SkinnedShortcuts/")+action->objectName(), action->shortcut());
    }
}

void ActionManager::resetShortcuts()
{
    for(QAction *action : m_actions.values())
    {
        action->setShortcut(action->property("defaultShortcut").toString());
    }
}
