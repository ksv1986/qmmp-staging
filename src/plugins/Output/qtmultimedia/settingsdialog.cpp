/***************************************************************************
 *   Copyright (C) 2015 by Ivan Ponomarev                                  *
 *   ivantrue@gmail.com                                                    *
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
#include <QAudioDevice>
#include <QMediaDevices>
#include <QDebug>
#include <qmmp/qmmp.h>

#include "settingsdialog.h"

SettingsDialog::SettingsDialog (QWidget *parent) : QDialog (parent)
{
    ui.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

	const QSettings settings;
    const QByteArray default_device = settings.value("QTMULTIMEDIA/device").toByteArray();

	//Default item always has index = 0
    ui.deviceComboBox->addItem(tr("Default"));
    ui.deviceComboBox->setCurrentIndex(0);

    const QList<QAudioDevice> devices = QMediaDevices::audioOutputs();
	int index = 1;
    for(const QAudioDevice &info : devices)
	{
        ui.deviceComboBox->addItem(info.description(), info.id());
        if (info.id() == default_device)
			ui.deviceComboBox->setCurrentIndex(index);
		++index;
	}
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::accept()
{
    qDebug("%s", Q_FUNC_INFO);
    QSettings settings;
    settings.setValue("QTMULTIMEDIA/device", ui.deviceComboBox->currentData().toByteArray());
    QDialog::accept();
}
