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
#include <qmmp/qmmp.h>
#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::SettingsDialog)
{
    m_ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Shout");
    m_ui->hostLineEdit->setText(settings.value("host", "127.0.0.1").toString());
    m_ui->portSpinBox->setValue(settings.value("port", 8000).toInt());
    m_ui->mountPointLineEdit->setText(settings.value("mount", "qmmp.out").toString());
    m_ui->userLineEdit->setText(settings.value("user", "source").toString());
    m_ui->passwLineEdit->setText(settings.value("passw", "hackme").toString());
    m_ui->publicCheckBox->setChecked(settings.value("public", false).toBool());
    m_ui->qualitySpinBox->setValue(settings.value("vorbis_quality", 0.8).toDouble());
    m_ui->srateSpinBox->setValue(settings.value("sample_rate", 44100).toInt());
    settings.endGroup();
}

SettingsDialog::~SettingsDialog()
{
    delete m_ui;
}

void SettingsDialog::accept()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Shout");
    settings.setValue("host",  m_ui->hostLineEdit->text());
    settings.setValue("port", m_ui->portSpinBox->value());
    settings.setValue("mount", m_ui->mountPointLineEdit->text());
    settings.setValue("user", m_ui->userLineEdit->text());
    settings.setValue("passw", m_ui->passwLineEdit->text());
    settings.setValue("public", m_ui->publicCheckBox->isChecked());
    settings.setValue("vorbis_quality", m_ui->qualitySpinBox->value());
    settings.setValue("sample_rate", m_ui->srateSpinBox->value());
    settings.endGroup();
    QDialog::accept();
}
