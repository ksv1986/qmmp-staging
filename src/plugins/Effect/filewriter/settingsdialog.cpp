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

#include <QSettings>
#include <QDesktopServices>
#include <QMenu>
#include <qmmp/qmmp.h>
#include <qmmpui/filedialog.h>
#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
 : QDialog(parent)
{
    m_ui.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);

    QMenu *fileNameMenu = new QMenu(this);
    fileNameMenu->addAction(tr("Artist"))->setData("%p");
    fileNameMenu->addAction(tr("Album"))->setData("%a");
    fileNameMenu->addAction(tr("Album Artist"))->setData("%aa");
    fileNameMenu->addAction(tr("Title"))->setData("%t");
    fileNameMenu->addAction(tr("Track Number"))->setData("%n");
    fileNameMenu->addAction(tr("Two-digit Track Number"))->setData("%NN");
    fileNameMenu->addAction(tr("Genre"))->setData("%g");
    fileNameMenu->addAction(tr("Comment"))->setData("%c");
    fileNameMenu->addAction(tr("Composer"))->setData("%C");
    fileNameMenu->addAction(tr("Duration"))->setData("%l");
    fileNameMenu->addAction(tr("Disc Number"))->setData("%D");
    fileNameMenu->addAction(tr("File Name"))->setData("%f");
    fileNameMenu->addAction(tr("File Path"))->setData("%F");
    fileNameMenu->addAction(tr("Year"))->setData("%y");
    fileNameMenu->addAction(tr("Condition"))->setData("%p%if(%p&%t, - ,)%t");
    m_ui.fileNameButton->setMenu(fileNameMenu);
    m_ui.fileNameButton->setPopupMode(QToolButton::InstantPopup);
    connect(fileNameMenu, SIGNAL(triggered(QAction *)), SLOT(addTitleString(QAction *)));

    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    QString outDir = QDesktopServices::storageLocation(QDesktopServices::MusicLocation);
    m_ui.outDirEdit->setText(settings.value("FileWriter/out_dir", outDir).toString());
    m_ui.outFileEdit->setText(settings.value("FileWriter/file_name", "%p%if(%p&%t, - ,)%t").toString());
    m_ui.qualitySpinBox->setValue(settings.value("FileWriter/vorbis_quality", 0.8).toFloat());
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::accept()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("FileWriter/out_dir", m_ui.outDirEdit->text());
    settings.setValue("FileWriter/file_name", m_ui.outFileEdit->text());
    settings.setValue("FileWriter/vorbis_quality", m_ui.qualitySpinBox->value());
    QDialog::accept();
}

void SettingsDialog::addTitleString(QAction *a)
{
    if (m_ui.outFileEdit->cursorPosition () < 1)
        m_ui.outFileEdit->insert(a->data().toString());
    else
        m_ui.outFileEdit->insert(" - "+a->data().toString());
}

void SettingsDialog::on_dirButton_clicked()
{
    QString dir = FileDialog::getExistingDirectory(this, tr("Choose a directory"),
                                        m_ui.outDirEdit->text());
    if(!dir.isEmpty())
        m_ui.outDirEdit->setText(dir);
}
