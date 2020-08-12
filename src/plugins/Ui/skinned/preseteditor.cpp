/***************************************************************************
 *   Copyright (C) 2006-2020 by Ilya Kotov                                 *
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

#include <QIcon>
#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0)) //qAsConst template
#include <qmmp/qmmp.h>
#endif
#include "eqpreset.h"
#include "preseteditor.h"

PresetEditor::PresetEditor(QWidget *parent)
        : QDialog(parent)
{
    m_ui.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    connect(m_ui.loadButton,SIGNAL(clicked()),SLOT(loadPreset()));
    connect(m_ui.deleteButton,SIGNAL(clicked()),SLOT(deletePreset()));
    m_ui.loadButton->setIcon(QIcon::fromTheme("document-open"));
    m_ui.deleteButton->setIcon(QIcon::fromTheme("edit-delete"));
}


PresetEditor::~PresetEditor()
{
    while (m_ui.presetListWidget->count () !=0)
        m_ui.presetListWidget->takeItem (0);

    while (m_ui.autoPresetListWidget->count () !=0)
        m_ui.autoPresetListWidget->takeItem (0);
}

void PresetEditor::addPresets(const QList<EQPreset*> &presets)
{
    for(QListWidgetItem *item : qAsConst(presets))
    {
        m_ui.presetListWidget->addItem(item);
    }
}

void PresetEditor::addAutoPresets(const QList<EQPreset*> &presets)
{
    for(QListWidgetItem *item : qAsConst(presets))
    {
        m_ui.autoPresetListWidget->addItem(item);
    }
}

void PresetEditor::loadPreset()
{
    EQPreset* preset = nullptr;
    if (m_ui.tabWidget->currentIndex () == 0)
        preset = static_cast<EQPreset *>(m_ui.presetListWidget->currentItem());
    if (m_ui.tabWidget->currentIndex () == 1)
        preset = static_cast<EQPreset *>(m_ui.autoPresetListWidget->currentItem());
    if (preset)
        emit presetLoaded(preset);
}

void PresetEditor::deletePreset()
{
    EQPreset* preset = nullptr;
    if (m_ui.tabWidget->currentIndex () == 0)
        preset = static_cast<EQPreset *>(m_ui.presetListWidget->currentItem());
    if (m_ui.tabWidget->currentIndex () == 1)
        preset = static_cast<EQPreset *>(m_ui.autoPresetListWidget->currentItem());
    if (preset)
        emit presetDeleted(preset);
}
