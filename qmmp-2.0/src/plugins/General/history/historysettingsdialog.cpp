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

#include <QSettings>
#include <qmmp/qmmp.h>
#include <qmmpui/metadataformattermenu.h>
#include "historysettingsdialog.h"
#include "ui_historysettingsdialog.h"

HistorySettingsDialog::HistorySettingsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::HistorySettingsDialog)
{
    m_ui->setupUi(this);
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_ui->titleLineEdit->setText(settings.value("History/title_format", "%if(%p,%p - %t,%t)").toString());

    MetaDataFormatterMenu *menu = new MetaDataFormatterMenu(MetaDataFormatterMenu::TITLE_MENU, this);
    m_ui->titleButton->setMenu(menu);
    m_ui->titleButton->setPopupMode(QToolButton::InstantPopup);
    connect(menu, SIGNAL(patternSelected(QString)), SLOT(addTitleString(QString)));
}

HistorySettingsDialog::~HistorySettingsDialog()
{
    delete m_ui;
}

void HistorySettingsDialog::accept()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("History/title_format", m_ui->titleLineEdit->text());
    QDialog::accept();
}

void HistorySettingsDialog::addTitleString(const QString &str)
{
    m_ui->titleLineEdit->insert(str);
}
