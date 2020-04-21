/***************************************************************************
 *   Copyright (C) 2019-2020 by Ilya Kotov                                 *
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
#include <qmmp/qmmp.h>
#include "settingsdialog.h"
#include "ultimatelyricsparser.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::SettingsDialog)
{
    m_ui->setupUi(this);
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);  
    UltimateLyricsParser parser;
    parser.load(":/ultimate_providers.xml");
    QStringList enabledProviders = settings.value("Lyrics/enabled_providers", parser.defaultProviders()).toStringList();

    for(const LyricsProvider *provider : parser.providers())
    {
        QListWidgetItem *item = new QListWidgetItem(provider->name());
        item->setCheckState(enabledProviders.contains(provider->name()) ? Qt::Checked : Qt::Unchecked);
        m_ui->providersListWidget->addItem(item);
    }
}

SettingsDialog::~SettingsDialog()
{
    delete m_ui;
}

void SettingsDialog::accept()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    QStringList enabledProviders;
    for(int i = 0; i < m_ui->providersListWidget->count(); ++i)
    {
        if(m_ui->providersListWidget->item(i)->checkState() == Qt::Checked)
            enabledProviders << m_ui->providersListWidget->item(i)->text();
    }
    settings.setValue("Lyrics/enabled_providers", enabledProviders);
    QDialog::accept();
}
