/***************************************************************************
 *   Copyright (C) 2008-2021 by Ilya Kotov                                 *
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
#include <QStringList>
#include <qmmp/qmmp.h>

extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/dict.h>
}
#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
        : QDialog(parent)
{
    m_ui.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    QStringList filters = {
        "*.wma", "*.ape", "*.tta", "*.m4a", "*.m4b", "*.aac", "*.ra", "*.shn", "*.vqf", "*.ac3", "*.tak", "*.dsf", "*.dsdiff"
    };
    filters = settings.value("FFMPEG/filters", filters).toStringList();

    m_ui.wmaCheckBox->setEnabled(avcodec_find_decoder(AV_CODEC_ID_WMAV1));
    m_ui.wmaCheckBox->setChecked(filters.contains("*.wma") && avcodec_find_decoder(AV_CODEC_ID_WMAV1));
    m_ui.apeCheckBox->setEnabled(avcodec_find_decoder(AV_CODEC_ID_APE));
    m_ui.apeCheckBox->setChecked(filters.contains("*.ape") && avcodec_find_decoder(AV_CODEC_ID_APE));
    m_ui.ttaCheckBox->setEnabled(avcodec_find_decoder(AV_CODEC_ID_TTA));
    m_ui.ttaCheckBox->setChecked(filters.contains("*.tta") && avcodec_find_decoder(AV_CODEC_ID_TTA));
    m_ui.aacCheckBox->setEnabled(avcodec_find_decoder(AV_CODEC_ID_AAC));
    m_ui.aacCheckBox->setChecked(filters.contains("*.aac") && avcodec_find_decoder(AV_CODEC_ID_AAC));
    m_ui.mp3CheckBox->setEnabled(avcodec_find_decoder(AV_CODEC_ID_MP3));
    m_ui.mp3CheckBox->setChecked(filters.contains("*.mp3") && avcodec_find_decoder(AV_CODEC_ID_MP3));
    m_ui.mp4CheckBox->setEnabled(avcodec_find_decoder(AV_CODEC_ID_AAC));
    m_ui.mp4CheckBox->setChecked(filters.contains("*.m4a") && (avcodec_find_decoder(AV_CODEC_ID_AAC)
                                                               || avcodec_find_decoder(AV_CODEC_ID_ALAC)));
    m_ui.raCheckBox->setEnabled(avcodec_find_decoder(AV_CODEC_ID_RA_288));
    m_ui.raCheckBox->setChecked(filters.contains("*.ra") && avcodec_find_decoder(AV_CODEC_ID_RA_288));
    m_ui.shCheckBox->setChecked(filters.contains("*.shn") && avcodec_find_decoder(AV_CODEC_ID_SHORTEN));
    m_ui.ac3CheckBox->setEnabled(avcodec_find_decoder(AV_CODEC_ID_EAC3));
    m_ui.ac3CheckBox->setChecked(filters.contains("*.ac3") && avcodec_find_decoder(AV_CODEC_ID_EAC3));
    m_ui.dtsCheckBox->setEnabled(avcodec_find_decoder(AV_CODEC_ID_DTS));
    m_ui.dtsCheckBox->setChecked(filters.contains("*.dts") && avcodec_find_decoder(AV_CODEC_ID_DTS));
    m_ui.mkaCheckBox->setEnabled(avcodec_find_decoder(AV_CODEC_ID_TRUEHD));
    m_ui.mkaCheckBox->setChecked(filters.contains("*.mka") && avcodec_find_decoder(AV_CODEC_ID_TRUEHD));
    m_ui.vqfCheckBox->setEnabled(avcodec_find_decoder(AV_CODEC_ID_TWINVQ));
    m_ui.vqfCheckBox->setChecked(filters.contains("*.vqf") && avcodec_find_decoder(AV_CODEC_ID_TWINVQ));
    m_ui.takCheckBox->setEnabled(avcodec_find_decoder(AV_CODEC_ID_TAK));
    m_ui.takCheckBox->setChecked(filters.contains("*.tak") && avcodec_find_decoder(AV_CODEC_ID_TAK));
    m_ui.dsdCheckBox->setEnabled(avcodec_find_decoder(AV_CODEC_ID_DSD_LSBF));
    m_ui.dsdCheckBox->setChecked(filters.contains("*.dsdiff") && avcodec_find_decoder(AV_CODEC_ID_DSD_LSBF));
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::accept()
{
    QStringList filters;
    if (m_ui.mp3CheckBox->isChecked())
        filters << "*.mp3";
    if (m_ui.wmaCheckBox->isChecked())
        filters << "*.wma";
    if (m_ui.apeCheckBox->isChecked())
        filters << "*.ape";
    if (m_ui.ttaCheckBox->isChecked())
        filters << "*.tta";
    if (m_ui.aacCheckBox->isChecked())
        filters << "*.aac";
    if (m_ui.mp4CheckBox->isChecked())
        filters << "*.m4a" << "*.m4b";
    if (m_ui.raCheckBox->isChecked())
        filters << "*.ra";
    if (m_ui.shCheckBox->isChecked())
        filters << "*.shn";
    if (m_ui.ac3CheckBox->isChecked())
        filters << "*.ac3";
    if (m_ui.dtsCheckBox->isChecked())
        filters << "*.dts";
    if (m_ui.mkaCheckBox->isChecked())
        filters << "*.mka";
    if (m_ui.vqfCheckBox->isChecked())
        filters << "*.vqf";
    if (m_ui.takCheckBox->isChecked())
        filters << "*.tak";
    if (m_ui.dsdCheckBox->isChecked())
        filters << "*.dsf" << "*.dsdiff";
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("FFMPEG/filters", filters);
    QDialog::accept();
}
