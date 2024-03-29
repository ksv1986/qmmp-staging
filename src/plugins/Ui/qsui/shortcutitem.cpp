/***************************************************************************
 *   Copyright (C) 2010-2015 by Ilya Kotov                                 *
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

#include <QAction>
#include <QDockWidget>
#include "actionmanager.h"
#include "shortcutitem.h"

ShortcutItem::ShortcutItem(QTreeWidgetItem *parent, int type) : QTreeWidgetItem(parent, QStringList()
        << ActionManager::instance()->action(type)->text().remove("&")
        << ActionManager::instance()->action(type)->shortcut().toString())
{
    m_action = ActionManager::instance()->action(type);
    setIcon(0, m_action->icon());
}

ShortcutItem::ShortcutItem(QTreeWidgetItem *parent, QDockWidget *w) : QTreeWidgetItem(parent, QStringList()
        << w->toggleViewAction()->text().remove("&")
        << w->toggleViewAction()->shortcut().toString())
{
    m_action = w->toggleViewAction();
}

ShortcutItem::~ShortcutItem()
{}

QAction *ShortcutItem::action()
{
    return m_action;
}
