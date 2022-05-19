/***************************************************************************
 *   Copyright (C) 2009-2012 by Artur Guzik                                *
 *   a.guzik88@gmail.com                                                   *
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

#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "kdenotify.h"
#include <qmmp/qmmp.h>
#include <qmmpui/templateeditor.h>

#include <QSettings>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::SettingsDialog)
{
    m_ui->setupUi(this);

    QSettings settings;
    settings.beginGroup("Kde_Notifier");
    m_ui->notifyDelaySpinBox->setValue(settings.value("notify_duration",5000).toInt()/1000);
    m_ui->showCoversCheckBox->setChecked(settings.value("show_covers",true).toBool());
    m_ui->updateNotifyCheckBox->setChecked(settings.value("update_notify",true).toBool());
    m_ui->volumeCheckBox->setChecked(settings.value("volume_notification", false).toBool());
    m_template = settings.value("template", DEFAULT_TEMPLATE).toString();
    settings.endGroup();
}

SettingsDialog::~SettingsDialog()
{
    delete m_ui;
}

void SettingsDialog::accept()
{
    QSettings settings;
    settings.beginGroup("Kde_Notifier");
    settings.setValue("notify_duration",m_ui->notifyDelaySpinBox->value()*1000);
    settings.setValue("show_covers",m_ui->showCoversCheckBox->isChecked());
    settings.setValue("template",m_template);
    settings.setValue("update_notify",m_ui->updateNotifyCheckBox->isChecked());
    settings.setValue("volume_notification", m_ui->volumeCheckBox->isChecked());
    settings.endGroup();
    QDialog::accept();
}

void SettingsDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void SettingsDialog::on_templateButton_clicked()
{
    QString t = TemplateEditor::getTemplate(this, tr("Notification Template"), m_template,
                                            DEFAULT_TEMPLATE);
    if(!t.isEmpty())
        m_template = t;
}
