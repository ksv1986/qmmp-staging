/***************************************************************************
 *   Copyright (C) 2021 by Ilya Kotov                                      *
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

#include <QFile>
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <qmmp/metadatamodel.h>
#include "cueeditor_p.h"
#include "filedialog.h"
#include "ui_cueeditor.h"

CueEditor::CueEditor(MetaDataModel *model, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::CueEditor),
    m_model(model)
{
    m_ui->setupUi(this);
    m_ui->plainTextEdit->setPlainText(model->cue());
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_lastDir = settings.value("CueEditor/last_dir",  QDir::homePath()).toString();
}

CueEditor::~CueEditor()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("CueEditor/last_dir", m_lastDir);

    delete m_ui;
}

void CueEditor::save()
{
    QString data = m_ui->plainTextEdit->toPlainText().trimmed();
    if(data.isEmpty())
    {
        m_model->removeCue();
    }
    else
    {
        data.append(QChar::LineFeed);
        m_model->setCue(data);
    }
}

void CueEditor::on_loadButton_clicked()
{
    QString path = FileDialog::getOpenFileName(this, tr("Open CUE File"),
                                               m_lastDir,
                                               tr("CUE Files") +" (*.cue)");
    if(!path.isEmpty())
    {
        m_lastDir = QFileInfo(path).absoluteDir().path();
        QFile file(path);
        file.open(QIODevice::ReadOnly);
        m_ui->plainTextEdit->setPlainText(QString::fromUtf8(file.readAll()));
    }
}

void CueEditor::on_deleteButton_clicked()
{
    m_ui->plainTextEdit->clear();
}

void CueEditor::on_saveAsButton_clicked()
{

}
