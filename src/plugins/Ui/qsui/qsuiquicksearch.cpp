/***************************************************************************
 *   Copyright (C) 2017 by Ilya Kotov                                      *
 *   forkotov02@hotmail.ru                                                 *
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
#include <QApplication>
#include <QDebug>
#include <qmmpui/playlistmanager.h>
#include <qmmpui/playlistmodel.h>
#include "qsuiquicksearch.h"

QSUIQuickSearch::QSUIQuickSearch(QWidget *parent) :
    QWidget(parent)
{
    m_manager = PlayListManager::instance();
    m_lineEdit = new QLineEdit(this);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);
    setLayout(layout);
    layout->addWidget(m_lineEdit);
    m_lineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QToolButton *downButton = new QToolButton(this);
    downButton->setAutoRaise(true);
    downButton->setIcon(style()->standardIcon(QStyle::QStyle::SP_ArrowDown));
    layout->addWidget(downButton);
    QToolButton *upButton = new QToolButton(this);
    upButton->setIcon(style()->standardIcon(QStyle::QStyle::SP_ArrowUp));
    upButton->setAutoRaise(true);
    layout->addWidget(upButton);

    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), SLOT(onFocusChanged(QWidget*,QWidget*)));
    connect(m_lineEdit, SIGNAL(textEdited(QString)), SLOT(onTextEdited(QString)));
    connect(downButton, SIGNAL(clicked(bool)), SLOT(onSearchDownClicked()));
    connect(upButton, SIGNAL(clicked(bool)), SLOT(onSearchUpClicked()));
}

void QSUIQuickSearch::onFocusChanged(QWidget *old, QWidget *now)
{
    if(now == m_lineEdit)
    {
        qDebug("focus in");
        search(m_lineEdit->text());
        updateSelection();
    }
    else if(old == m_lineEdit)
        qDebug("focus out");
}

void QSUIQuickSearch::onTextEdited(const QString &str)
{
    search(str);
    updateSelection();
}

void QSUIQuickSearch::onSearchUpClicked()
{
    PlayListModel *model = m_manager->selectedPlayList();
}

void QSUIQuickSearch::onSearchDownClicked()
{
    PlayListModel *model = m_manager->selectedPlayList();
}

void QSUIQuickSearch::search(const QString &str)
{
    m_indexes.clear();
    if(str.isEmpty())
        return;

    PlayListModel *model = m_manager->selectedPlayList();

    for(int i = 0; i < model->count(); ++i)
    {
        PlayListItem *item = model->item(i);
        if(item->isGroup())
            continue;

        if(!item->formattedTitles().filter(str, Qt::CaseInsensitive).isEmpty())
            m_indexes << i;
    }
}

void QSUIQuickSearch::updateSelection()
{
    PlayListModel *model = m_manager->selectedPlayList();
    model->clearSelection();
    model->setSelected(m_indexes);
}

