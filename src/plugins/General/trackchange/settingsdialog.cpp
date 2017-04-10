/***************************************************************************
 *   Copyright (C) 2013-2017 by Ilya Kotov                                 *
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
#include <QMenu>
#include <qmmp/qmmp.h>
#include <qmmpui/metadataformattermenu.h>
#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent)
{
    m_ui.setupUi(this);
    addMenu(m_ui.newTrackButton);
    addMenu(m_ui.endOfTrackButton);
    addMenu(m_ui.endOfPlayListButton);
    addMenu(m_ui.titleChangeButton);

    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_ui.newTrackLineEdit->setText(settings.value("TrackChange/new_track_command").toString());
    m_ui.endOfTrackLineEdit->setText(settings.value("TrackChange/end_of_track_command").toString());
    m_ui.endOfPlayListLineEdit->setText(settings.value("TrackChange/end_of_pl_command").toString());
    m_ui.titleChangeLineEdit->setText(settings.value("TrackChange/title_change_command").toString());
}

SettingsDialog::~SettingsDialog()
{}

void SettingsDialog::accept()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("TrackChange/new_track_command", m_ui.newTrackLineEdit->text());
    settings.setValue("TrackChange/end_of_track_command",  m_ui.endOfTrackLineEdit->text());
    settings.setValue("TrackChange/end_of_pl_command",  m_ui.endOfPlayListLineEdit->text());
    settings.setValue("TrackChange/title_change_command", m_ui.titleChangeLineEdit->text());
    QDialog::accept();
}

void SettingsDialog::addMenu(QToolButton *button)
{
    MetaDataFormatterMenu *menu = new MetaDataFormatterMenu(MetaDataFormatterMenu::TITLE_MENU, this);
    button->setMenu(menu);
    button->setPopupMode(QToolButton::InstantPopup);
    connect(menu, SIGNAL(patternSelected(QString)), SLOT(addTemplateString(QString)));
}

void SettingsDialog::addTemplateString(const QString &str)
{
    QMenu *menu = qobject_cast<QMenu*> (sender());
    if(!menu)
        return;

    if(m_ui.newTrackButton->menu() == menu)
    {
        m_ui.newTrackLineEdit->insert(str);
    }
    else if(m_ui.endOfTrackButton->menu() == menu)
    {
        m_ui.endOfTrackLineEdit->insert(str);
    }
    else if(m_ui.endOfPlayListButton->menu() == menu)
    {
        m_ui.endOfPlayListLineEdit->insert(str);
    }
    else if(m_ui.titleChangeButton->menu() == menu)
    {
        m_ui.titleChangeLineEdit->insert(str);
    }
}
