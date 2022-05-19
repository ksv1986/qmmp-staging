/***************************************************************************
 *   Copyright (C) 2008-2022 by Ilya Kotov                                 *
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
    QSettings settings;
    QStringList disabledFilters = { "*.mp3" };
    disabledFilters = settings.value("FFMPEG/disabled_filters", disabledFilters).toStringList();

    m_ui.wmaCheckBox->setEnabled(avcodec_find_decoder(AV_CODEC_ID_WMAV1));
    m_ui.wmaCheckBox->setChecked(!disabledFilters.contains("*.wma") && avcodec_find_decoder(AV_CODEC_ID_WMAV1));
    m_ui.apeCheckBox->setEnabled(avcodec_find_decoder(AV_CODEC_ID_APE));
    m_ui.apeCheckBox->setChecked(!disabledFilters.contains("*.ape") && avcodec_find_decoder(AV_CODEC_ID_APE));
    m_ui.ttaCheckBox->setEnabled(avcodec_find_decoder(AV_CODEC_ID_TTA));
    m_ui.ttaCheckBox->setChecked(!disabledFilters.contains("*.tta") && avcodec_find_decoder(AV_CODEC_ID_TTA));
    m_ui.aacCheckBox->setEnabled(avcodec_find_decoder(AV_CODEC_ID_AAC));
    m_ui.aacCheckBox->setChecked(!disabledFilters.contains("*.aac") && avcodec_find_decoder(AV_CODEC_ID_AAC));
    m_ui.mp3CheckBox->setEnabled(avcodec_find_decoder(AV_CODEC_ID_MP3));
    m_ui.mp3CheckBox->setChecked(!disabledFilters.contains("*.mp3") && avcodec_find_decoder(AV_CODEC_ID_MP3));
    m_ui.mp4CheckBox->setEnabled(avcodec_find_decoder(AV_CODEC_ID_AAC));
    m_ui.mp4CheckBox->setChecked(!disabledFilters.contains("*.m4a") && (avcodec_find_decoder(AV_CODEC_ID_AAC)
                                                               || avcodec_find_decoder(AV_CODEC_ID_ALAC)));
    m_ui.raCheckBox->setEnabled(avcodec_find_decoder(AV_CODEC_ID_RA_288));
    m_ui.raCheckBox->setChecked(!disabledFilters.contains("*.ra") && avcodec_find_decoder(AV_CODEC_ID_RA_288));
    m_ui.shCheckBox->setChecked(!disabledFilters.contains("*.shn") && avcodec_find_decoder(AV_CODEC_ID_SHORTEN));
    m_ui.ac3CheckBox->setEnabled(avcodec_find_decoder(AV_CODEC_ID_EAC3));
    m_ui.ac3CheckBox->setChecked(!disabledFilters.contains("*.ac3") && avcodec_find_decoder(AV_CODEC_ID_EAC3));
    m_ui.dtsCheckBox->setEnabled(avcodec_find_decoder(AV_CODEC_ID_DTS));
    m_ui.dtsCheckBox->setChecked(!disabledFilters.contains("*.dts") && avcodec_find_decoder(AV_CODEC_ID_DTS));
    m_ui.mkaCheckBox->setEnabled(avcodec_find_decoder(AV_CODEC_ID_TRUEHD));
    m_ui.mkaCheckBox->setChecked(!disabledFilters.contains("*.mka") && avcodec_find_decoder(AV_CODEC_ID_TRUEHD));
    m_ui.vqfCheckBox->setEnabled(avcodec_find_decoder(AV_CODEC_ID_TWINVQ));
    m_ui.vqfCheckBox->setChecked(!disabledFilters.contains("*.vqf") && avcodec_find_decoder(AV_CODEC_ID_TWINVQ));
    m_ui.takCheckBox->setEnabled(avcodec_find_decoder(AV_CODEC_ID_TAK));
    m_ui.takCheckBox->setChecked(!disabledFilters.contains("*.tak") && avcodec_find_decoder(AV_CODEC_ID_TAK));
    m_ui.dsdCheckBox->setEnabled(avcodec_find_decoder(AV_CODEC_ID_DSD_LSBF));
    m_ui.dsdCheckBox->setChecked(!disabledFilters.contains("*.dsdiff") && avcodec_find_decoder(AV_CODEC_ID_DSD_LSBF));
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::accept()
{
    QStringList disabledFilters;
    if (!m_ui.mp3CheckBox->isChecked())
        disabledFilters << "*.mp3";
    if (!m_ui.wmaCheckBox->isChecked())
        disabledFilters << "*.wma";
    if (!m_ui.apeCheckBox->isChecked())
        disabledFilters << "*.ape";
    if (!m_ui.ttaCheckBox->isChecked())
        disabledFilters << "*.tta";
    if (!m_ui.aacCheckBox->isChecked())
        disabledFilters << "*.aac";
    if (!m_ui.mp4CheckBox->isChecked())
        disabledFilters << "*.m4a" << "*.m4b";
    if (!m_ui.raCheckBox->isChecked())
        disabledFilters << "*.ra";
    if (!m_ui.shCheckBox->isChecked())
        disabledFilters << "*.shn";
    if (!m_ui.ac3CheckBox->isChecked())
        disabledFilters << "*.ac3";
    if (!m_ui.dtsCheckBox->isChecked())
        disabledFilters << "*.dts";
    if (!m_ui.mkaCheckBox->isChecked())
        disabledFilters << "*.mka";
    if (!m_ui.vqfCheckBox->isChecked())
        disabledFilters << "*.vqf";
    if (!m_ui.takCheckBox->isChecked())
        disabledFilters << "*.tak";
    if (!m_ui.dsdCheckBox->isChecked())
        disabledFilters << "*.dsf" << "*.dsdiff";
    QSettings settings;
    settings.setValue("FFMPEG/disabled_filters", disabledFilters);
    QDialog::accept();
}
