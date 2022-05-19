/***************************************************************************
 *   Copyright (C) 2020-2022 by Ilya Kotov                                 *
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
#include <QMenu>
#include <QContextMenuEvent>
#include <QIcon>
#include <QLabel>
#include <qmmp/qmmp.h>
#include "librarymodel.h"
#include "ui_librarywidget.h"
#include "librarywidget.h"

LibraryWidget::LibraryWidget(bool dialog, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::LibraryWidget)
{
    m_ui->setupUi(this);
    m_model = new LibraryModel(this);
    m_ui->treeView->setModel(m_model);

    if(dialog)
    {
        setWindowFlags(Qt::Dialog);
        setAttribute(Qt::WA_DeleteOnClose);
        setAttribute(Qt::WA_QuitOnClose, false);
    }
    else
    {
        m_ui->buttonBox->hide();
    }

    m_menu = new QMenu(this);
    m_menu->addAction(QIcon::fromTheme("list-add"), tr("&Add to Playlist"), this, SLOT(addSelected()));
    m_menu->addAction(QIcon::fromTheme("dialog-information"), tr("&View Track Details"), this, SLOT(showInformation()));
    m_menu->addSeparator();
    m_filterAction = m_menu->addAction(tr("Quick Search"), m_ui->filterLineEdit, SLOT(setVisible(bool)));
    m_filterAction->setCheckable(true);

    QSettings settings;
    m_filterAction->setChecked(settings.value("Library/quick_search_visible", true).toBool());
    m_ui->filterLineEdit->setVisible(m_filterAction->isChecked());
    if(dialog)
        restoreGeometry(settings.value("Library/geometry").toByteArray());
}

LibraryWidget::~LibraryWidget()
{
    QSettings settings;
    settings.setValue("Library/quick_search_visible", m_filterAction->isChecked());

    delete m_ui;
}

void LibraryWidget::refresh()
{
    m_ui->filterLineEdit->clear();
    m_model->refresh();
}

void LibraryWidget::setBusyMode(bool enabled)
{
    if(m_busyIndicator)
    {
        delete m_busyIndicator;
        m_busyIndicator = nullptr;
    }

    if(enabled)
    {
        m_busyIndicator = new QLabel(tr("Scanning directories..."), this);
        m_busyIndicator->setFrameShape(QFrame::Box);
        m_busyIndicator->resize(m_busyIndicator->sizeHint());
        m_busyIndicator->move(width() / 2 - m_busyIndicator->width() / 2 , height() / 2 - m_busyIndicator->height() / 2);
        m_busyIndicator->setAutoFillBackground(true);
        m_busyIndicator->show();

        m_ui->treeView->setEnabled(false);
        m_ui->filterLineEdit->setEnabled(false);
    }
    else
    {
        m_ui->treeView->setEnabled(true);
        m_ui->filterLineEdit->setEnabled(true);
    }
}

void LibraryWidget::closeEvent(QCloseEvent *)
{
    if(isWindow())
    {
        QSettings settings;
        settings.setValue("Library/geometry", saveGeometry());
    }
}

void LibraryWidget::contextMenuEvent(QContextMenuEvent *e)
{
    m_menu->exec(mapToGlobal(e->pos()));
}

void LibraryWidget::on_filterLineEdit_textChanged(const QString &text)
{
    m_model->setFilter(text);
    m_model->refresh();
    if(m_model->rowCount() <= 4)
        m_ui->treeView->expandAll();
}

void LibraryWidget::addSelected()
{
    m_model->add(m_ui->treeView->selectionModel()->selectedIndexes());
}

void LibraryWidget::showInformation()
{
    m_model->showInformation(m_ui->treeView->selectionModel()->selectedIndexes());
}
