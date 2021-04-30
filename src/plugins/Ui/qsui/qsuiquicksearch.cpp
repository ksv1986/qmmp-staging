/***************************************************************************
 *   Copyright (C) 2017 by Ilya Kotov                                      *
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

#include <QLineEdit>
#include <QHBoxLayout>
#include <QToolButton>
#include <QStyle>
#include <QEvent>
#include <QDebug>
#include <qmmpui/playlistmanager.h>
#include <qmmpui/playlistmodel.h>
#include "listwidget.h"
#include "qsuiquicksearch.h"

QSUiQuickSearch::QSUiQuickSearch(ListWidget *listWidget, QWidget *parent) :
    QWidget(parent)
{
    m_listWidget = listWidget;
    m_manager = PlayListManager::instance();
    m_lineEdit = new QLineEdit(this);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);
    layout->addWidget(m_lineEdit);
    m_lineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_lineEdit->setClearButtonEnabled(true);
    connect(m_lineEdit, SIGNAL(textEdited(QString)), m_listWidget, SLOT(setFilterString(QString)));
    connect(m_manager, SIGNAL(selectedPlayListChanged(PlayListModel*,PlayListModel*)), m_lineEdit, SLOT(clear()));
    connect(m_listWidget, SIGNAL(doubleClicked()), m_lineEdit, SLOT(clear()));
}
