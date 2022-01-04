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
#include <QDir>
#include <libmodplug/stdafx.h>
#include <libmodplug/it_defs.h>
#include <libmodplug/sndfile.h>
#include "decoder_modplug.h"
#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
        : QDialog(parent)
{
    m_ui.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    connect(m_ui.surDepthSlider, &QSlider::valueChanged, m_ui.label_11, qOverload<int>(&QLabel::setNum));
    connect(m_ui.surDelaySlider, &QSlider::valueChanged, m_ui.label_12, qOverload<int>(&QLabel::setNum));
    connect(m_ui.reverbDepthSlider, &QSlider::valueChanged, m_ui.label_9, qOverload<int>(&QLabel::setNum));
    connect(m_ui.reverbDelaySlider, &QSlider::valueChanged, m_ui.label_10, qOverload<int>(&QLabel::setNum));
    connect(m_ui.bassRangeSlider, &QSlider::valueChanged, m_ui.label_14, qOverload<int>(&QLabel::setNum));
    connect(m_ui.bassAmountSlider, &QSlider::valueChanged, m_ui.label_13, qOverload<int>(&QLabel::setNum));

    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("ModPlug");
    //general
    m_ui.noiseCheckBox->setChecked(settings.value("NoiseReduction", false).toBool());
    m_ui.fileNameCheckBox->setChecked(settings.value("UseFileName", false).toBool());
    m_ui.amigaCheckBox->setChecked(settings.value("GrabAmigaMOD", true).toBool());
    //settings.value("Oversampling", true).toBool();
    //settings.value("VolumeRamp", true).toBool();
    //settings.value("FastInfo", true).toBool();
    //channels number
    if (settings.value("Channels", 2).toInt() == 2)
        m_ui.stereoRadioButton->setChecked(true);
    else
        m_ui.monoRadioButton->setChecked(true);
    //bits number
    if (settings.value("Bits", 16).toInt() == 8)
        m_ui.bit8RadioButton->setChecked(true);
    else
        m_ui.bit16RadioButton->setChecked(true);
    //resampling frequency
    int freq = settings.value("Frequency", 44100).toInt();
    if (freq == 48000)
        m_ui.khz48RadioButton->setChecked(true);
    else if (freq == 44100)
        m_ui.khz44RadioButton->setChecked(true);
    else if (freq == 22050)
        m_ui.khz22RadioButton->setChecked(true);
    else
        m_ui.khz11RadioButton->setChecked(true);
    //resampling mode
    int res = settings.value("ResamplineMode", SRCMODE_POLYPHASE).toInt();
    if (res == SRCMODE_NEAREST)
        m_ui.resampNearestRadioButton->setChecked(true);
    else if (res == SRCMODE_LINEAR)
        m_ui.resampLinearRadioButton->setChecked(true);
    else if (res == SRCMODE_SPLINE)
        m_ui.resampSplineRadioButton->setChecked(true);
    else
        m_ui.resampPolyphaseRadioButton->setChecked(true);
    //reverberation
    m_ui.reverbGroupBox->setChecked(settings.value("Reverb", false).toBool());
    m_ui.reverbDepthSlider->setValue(settings.value("ReverbDepth", 30).toInt());
    m_ui.reverbDelaySlider->setValue(settings.value("ReverbDelay", 100).toInt());
    //surround
    m_ui.surGroupBox->setChecked(settings.value("Surround", true).toBool());
    m_ui.surDepthSlider->setValue(settings.value("SurroundDepth", 20).toInt());
    m_ui.surDelaySlider->setValue(settings.value("SurroundDelay", 20).toInt());
    //bass
    m_ui.bassGroupBox->setChecked(settings.value("Megabass", false).toBool());
    m_ui.bassAmountSlider->setValue(settings.value("BassAmount", 40).toInt());
    m_ui.bassRangeSlider->setValue(settings.value("BassRange", 30).toInt());
    //preamp
    m_ui.preampGroupBox->setChecked(settings.value("PreAmp", false).toBool());
    connect(m_ui.preampSlider, SIGNAL(valueChanged(int)), SLOT(setPreamp(int)));
    m_ui.preampSlider->setValue(int(settings.value("PreAmpLevel", 0.0f).toDouble()*10));
    //looping
    int l = settings.value("LoopCount", 0).toInt();
    if (l == 0)
        m_ui.dontLoopRadioButton->setChecked(true);
    else if (l < 0)
        m_ui.loopForeverRadioButton->setChecked(true);
    else
    {
        m_ui.loopRadioButton->setChecked(true);
        m_ui.loopSpinBox->setValue(l);
    }
    settings.endGroup();
}


SettingsDialog::~SettingsDialog()
{}

void SettingsDialog::writeSettings()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("ModPlug");
    //general
    settings.setValue("NoiseReduction", m_ui.noiseCheckBox->isChecked());
    settings.setValue("UseFileName", m_ui.fileNameCheckBox->isChecked());
    settings.setValue("GrabAmigaMOD", m_ui.amigaCheckBox->isChecked());
    //settings.value("Oversampling", true).toBool();
    //settings.value("VolumeRamp", true).toBool();
    //settings.value("FastInfo", true).toBool();
    //channels number
    settings.setValue("Channels", m_ui.stereoRadioButton->isChecked() ? 2 : 1 );
    //bits number
    settings.setValue("Bits", m_ui.bit8RadioButton->isChecked() ? 8 : 16 );
    //resampling frequency
    if (m_ui.khz48RadioButton->isChecked())
        settings.setValue("Frequency", 48000);
    else if (m_ui.khz44RadioButton->isChecked())
        settings.setValue("Frequency", 44100);
    else if (m_ui.khz22RadioButton->isChecked())
        settings.setValue("Frequency", 22050);
    else
        settings.setValue("Frequency", 11025);
    //resampling mode
    if (m_ui.resampNearestRadioButton->isChecked())
        settings.setValue("ResamplineMode", SRCMODE_NEAREST);
    else if (m_ui.resampLinearRadioButton->isChecked())
        settings.setValue("ResamplineMode", SRCMODE_LINEAR);
    else if (m_ui.resampSplineRadioButton->isChecked())
        settings.setValue("ResamplineMode", SRCMODE_SPLINE);
    else
        settings.setValue("ResamplineMode", SRCMODE_POLYPHASE);
    //reverberation
    settings.setValue("Reverb",  m_ui.reverbGroupBox->isChecked());
    settings.setValue("ReverbDepth", m_ui.reverbDepthSlider->value());
    settings.setValue("ReverbDelay", m_ui.reverbDelaySlider->value());
    //surround
    settings.setValue("Surround", m_ui.surGroupBox->isChecked());
    settings.setValue("SurroundDepth", m_ui.surDepthSlider->value());
    settings.setValue("SurroundDelay", m_ui.surDelaySlider->value());
    //bass
    settings.setValue("Megabass", m_ui.bassGroupBox->isChecked());
    settings.setValue("BassAmount", m_ui.bassAmountSlider->value());
    settings.setValue("BassRange", m_ui.bassRangeSlider->value());
    //preamp
    settings.setValue("PreAmp", m_ui.preampGroupBox->isChecked());
    connect(m_ui.preampSlider, SIGNAL(valueChanged(int)), SLOT(setPreamp(int)));
    settings.setValue("PreAmpLevel", (double) m_ui.preampSlider->value()/10);
    //looping
    if (m_ui.dontLoopRadioButton->isChecked())
        settings.setValue("LoopCount", 0);
    else if (m_ui.loopForeverRadioButton->isChecked())
        settings.setValue("LoopCount", -1);
    else
        settings.setValue("LoopCount", m_ui.loopSpinBox->value());
    settings.endGroup();
    //apply settings for the created decoder
    if (DecoderModPlug::instance())
    {
        //DecoderModPlug::instance()->mutex()->lock();
        DecoderModPlug::instance()->readSettings();
        //DecoderModPlug::instance()->mutex()->unlock();
    }
}

void SettingsDialog::setPreamp(int preamp)
{
    m_ui.preampLabel->setText(QString("%1").arg((double) preamp/10));
}

void SettingsDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    switch ((int) m_ui.buttonBox->buttonRole(button))
    {
    case QDialogButtonBox::AcceptRole:
        writeSettings();
        accept();
        break;
    case QDialogButtonBox::ApplyRole:
        writeSettings();
        break;
    }
}
