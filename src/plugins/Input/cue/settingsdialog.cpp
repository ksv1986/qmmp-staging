/***************************************************************************
 *   Copyright (C) 2008-2012 by Ilya Kotov                                 *
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
#include <QRegularExpression>
#include <qmmp/qmmptextcodec.h>
#include <qmmp/qmmp.h>

#ifdef WITH_ENCA
#include <enca.h>
#endif

#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
        : QDialog(parent)
{
    ui.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    ui.cueEncComboBox->addItems(QmmpTextCodec::availableCharsets());

#ifdef WITH_ENCA
    size_t n = 0;
    const char **langs = enca_get_languages(&n);
    for (size_t i = 0; i < n; ++i)
        ui.encaAnalyserComboBox->addItem(langs[i]);
#endif
    QSettings settings;
    settings.beginGroup("CUE");
    int pos = ui.cueEncComboBox->findText(settings.value("encoding","UTF-8").toString());
    ui.cueEncComboBox->setCurrentIndex(pos);
#ifdef WITH_ENCA
    ui.autoCharsetCheckBox->setChecked(settings.value("use_enca", false).toBool());
    pos = ui.encaAnalyserComboBox->findText(settings.value("enca_lang", langs[n-1]).toString());
    ui.encaAnalyserComboBox->setCurrentIndex(pos);
#else
    ui.autoCharsetCheckBox->setEnabled(false);
#endif
    ui.dirtyCueCheckBox->setChecked(settings.value("dirty_cue", false).toBool());
    settings.endGroup();
}

SettingsDialog::~SettingsDialog()
{}

void SettingsDialog::accept()
{
    QSettings settings;
    settings.beginGroup("CUE");
    settings.setValue("encoding", ui.cueEncComboBox->currentText());
#ifdef WITH_ENCA
    settings.setValue("use_enca", ui.autoCharsetCheckBox->isChecked());
    settings.setValue("enca_lang", ui.encaAnalyserComboBox->currentText());
#endif
    settings.setValue("dirty_cue", ui.dirtyCueCheckBox->isChecked());
    settings.endGroup();
    QDialog::accept();
}
