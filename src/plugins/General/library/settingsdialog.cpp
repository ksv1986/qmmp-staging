/***************************************************************************
 *   Copyright (C) 2020-2021 by Ilya Kotov                                 *
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

#include <qmmp/qmmp.h>
#include <qmmpui/filedialog.h>
#include <QDir>

#include <QSettings>
#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::SettingsDialog)
{
    m_ui->setupUi(this);
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_lastPath = settings.value("Library/last_path", QDir::homePath()).toString();
    QStringList dirs = settings.value("Library/dirs").toStringList();
    m_ui->dirsListWidget->addItems(dirs);
    m_ui->quickSearchCheckBox->setChecked(settings.value("Library/quick_search_visible", true).toBool());
}

SettingsDialog::~SettingsDialog()
{
    delete m_ui;
}

void SettingsDialog::accept()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("Library/last_path", m_lastPath);

    QStringList dirs;
    for(int i = 0; i < m_ui->dirsListWidget->count(); ++i)
        dirs << m_ui->dirsListWidget->item(i)->text();

    settings.setValue("Library/dirs", dirs);
    settings.setValue("Library/quick_search_visible", m_ui->quickSearchCheckBox->isChecked());
    QDialog::accept();
}

void SettingsDialog::on_addDirButton_clicked()
{
    QString path = FileDialog::getExistingDirectory(this, tr("Select Directories for Scanning"), m_lastPath);
    if(!path.isEmpty())
    {
        m_ui->dirsListWidget->addItem(path);
        m_lastPath = QFileInfo(path).absolutePath();
    }
}

void SettingsDialog::on_removeDirButton_clicked()
{
    QList<QListWidgetItem *> items = m_ui->dirsListWidget->selectedItems();
    qDeleteAll(items);
}
