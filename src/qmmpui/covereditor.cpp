/***************************************************************************
 *   Copyright (C) 2018 by Ilya Kotov                                      *
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

#include <QVBoxLayout>
#include "coverviewer_p.h"
#include "covereditor_p.h"

CoverEditor::CoverEditor(MetaDataModel *model, const QString &coverPath, QWidget *parent) :
    QWidget(parent)
{
    m_ui.setupUi(this);
    m_model = model;
    m_coverPath = coverPath;
    m_editable = m_model && (m_model->dialogHints() & MetaDataModel::IS_COVER_EDITABLE) && !m_model->isReadOnly();

    m_ui.sourceComboBox->addItem(tr("External file"));
    m_ui.sourceComboBox->addItem(tr("Tag"));

    m_viewer = new CoverViewer(this);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(m_viewer);
    m_ui.frame->setLayout(layout);

    if(m_model && !m_model->cover().isNull())
        m_ui.sourceComboBox->setCurrentIndex(1);
    else
        m_ui.sourceComboBox->setCurrentIndex(0);

    on_sourceComboBox_activated(m_ui.sourceComboBox->currentIndex());

    if(!m_model || m_coverPath.isEmpty() || (!m_editable && m_model->cover().isNull()))
        m_ui.sourceComboBox->setEnabled(false);
}

bool CoverEditor::isEditable() const
{
    return m_editable;
}

void CoverEditor::save()
{

}

void CoverEditor::on_sourceComboBox_activated(int index)
{
    if(index == 0)
    {
        m_viewer->setPixmap(QPixmap(m_coverPath));
        m_ui.loadButton->setEnabled(false);
        m_ui.deleteButton->setEnabled(false);
        m_ui.saveAsButton->setEnabled(m_viewer->hasPixmap());
    }
    else if(index == 1)
    {
        m_viewer->setPixmap(m_model->cover());
        m_ui.loadButton->setEnabled(m_editable);
        m_ui.deleteButton->setEnabled(m_editable && m_viewer->hasPixmap());
        m_ui.saveAsButton->setEnabled(m_viewer->hasPixmap());
    }
}

void CoverEditor::on_loadButton_clicked()
{

}

void CoverEditor::on_deleteButton_clicked()
{

}

void CoverEditor::on_saveAsButton_clicked()
{
    m_viewer->saveAs();
}
