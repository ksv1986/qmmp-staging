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
#include <QPushButton>
#include <QSettings>
#include <qmmp/qmmp.h>
#include "outputpipewire.h"
#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog* SettingsDialog::instance;

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::SettingsDialog)
{
    instance = this;

    m_ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);

    auto pin = m_ui->pinCheckBox;
    auto combo = m_ui->sinkComboBox;

    const auto o = OutputPipeWire::instance;
    if (!o) {
        pin->setEnabled(false);
        combo->setCurrentText("Not playing");
        combo->setEnabled(false);
        m_ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        return;
    }

    QSettings settings;
    pin->setChecked(settings.value(Settings::KeyPin, false).toBool());
    auto sink = settings.value(Settings::KeySink, QString{}).toString();

    combo->addItem("default", PW_ID_ANY);
    for (auto it = o->m_sinks.cbegin(); it != o->m_sinks.cend(); ++it)
    {
        combo->addItem(it.key(), it.value());
        if (it.key() == sink)
            combo->setCurrentIndex(combo->count() - 1);
    }
}

SettingsDialog::~SettingsDialog()
{
    delete m_ui;
    instance = nullptr;
}

void SettingsDialog::accept()
{
    QSettings settings;
    settings.setValue(Settings::KeySink, m_ui->sinkComboBox->currentIndex() ? m_ui->sinkComboBox->currentText() : QString());
    settings.setValue(Settings::KeyPin, m_ui->pinCheckBox->checkState() == Qt::Checked);
    QDialog::accept();
}
