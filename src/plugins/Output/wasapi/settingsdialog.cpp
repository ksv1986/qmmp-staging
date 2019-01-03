/***************************************************************************
 *   Copyright (C) 2017-2018 by Ilya Kotov                                 *
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
#include <initguid.h>
#include <audioclient.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <mmreg.h>
#include <functiondiscoverykeys_devpkey.h>
#include <qmmp/qmmp.h>
#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent)
{
    m_ui.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    enumDevices();

    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    QString id = settings.value("WASAPI/device", "default").toString();
    int index = m_ui.deviceComboBox->findData(id);
    m_ui.exclusiveModeCheckBox->setChecked(settings.value("WASAPI/exclusive_mode", false).toBool());
    m_ui.deviceComboBox->setCurrentIndex(qMax(index, 0));
}

void SettingsDialog::accept()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    int index = m_ui.deviceComboBox->currentIndex();
    settings.setValue("WASAPI/device", m_ui.deviceComboBox->itemData(index).toString());
    settings.setValue("WASAPI/exclusive_mode", m_ui.exclusiveModeCheckBox->isChecked());
    QDialog::accept();
}

void SettingsDialog::enumDevices()
{
    m_ui.deviceComboBox->clear();
    m_ui.deviceComboBox->addItem(tr("Default"), "default");

    IMMDeviceEnumerator *pEnumerator = nullptr;
    HRESULT result = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&pEnumerator);
    if(result != S_OK)
    {
        qWarning("SettingsDialog: CoCreateInstance failed, error code = 0x%lx", result);
        pEnumerator = nullptr;
    }

    IMMDeviceCollection *pEndpoints = nullptr;
    IMMDevice *pEndpoint = nullptr;
    LPWSTR pwszID = nullptr;
    IPropertyStore *pProps = nullptr;
    UINT count = 0;

    result = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pEndpoints);
    if(result != S_OK)
    {
        qWarning("SettingsDialog: IMMDeviceEnumerator::EnumAudioEndpoints failed, error code = 0x%lx", result);
        pEndpoints = nullptr;
    }

    if(pEndpoints)
    {
        pEndpoints->GetCount(&count);
        if(result != S_OK)
        {
            qWarning("SettingsDialog: IMMDeviceCollection::GetCount failed, error code = 0x%lx", result);
            count = 0;
        }

        for(UINT i = 0; i < count; ++i)
        {
            result = pEndpoints->Item(i, &pEndpoint);
            if(result != S_OK)
            {
                qWarning("SettingsDialog: IMMDeviceCollection::Item failed, error code = 0x%lx", result);
                pEndpoint = nullptr;
                break;
            }

            result = pEndpoint->GetId(&pwszID);
            if(result != S_OK)
            {
                qWarning("SettingsDialog: IMMDevice::GetId failed, error code = 0x%lx", result);
                pwszID = nullptr;
                break;
            }

            result = pEndpoint->OpenPropertyStore(STGM_READ, &pProps);
            if(result != S_OK)
            {
                qWarning("SettingsDialog: IMMDevice::GetId failed, error code = 0x%lx", result);
                pProps = nullptr;
                break;
            }

            PROPVARIANT varName;
            PropVariantInit(&varName);  // Initialize container for property value.
            result = pProps->GetValue(PKEY_Device_FriendlyName, &varName);  // Get the endpoint's friendly-name property.
            if(result != S_OK)
            {
                qWarning("SettingsDialog: IMMDevice::GetId failed, error code = 0x%lx", result);
                PropVariantClear(&varName);
                break;
            }

            m_ui.deviceComboBox->addItem(QString::fromWCharArray(varName.pwszVal), QString::fromWCharArray(pwszID));

            CoTaskMemFree(pwszID);
            pwszID = nullptr;
            PropVariantClear(&varName);
            pProps->Release();
            pProps = nullptr;
            pEndpoint->Release();
            pEndpoint = nullptr;
        }
    }

    if(pProps)
    {
        pProps->Release();
        pProps = nullptr;
    }

    if(pwszID)
    {
        CoTaskMemFree(pwszID);
        pwszID = nullptr;
    }

    if(pEndpoint)
    {
        pEndpoint->Release();
        pEndpoint = nullptr;
    }

    if(pEndpoints)
    {
        pEndpoints->Release();
        pEndpoints = nullptr;
    }

    if(pEnumerator)
    {
       pEnumerator->Release();
       pEnumerator = nullptr;
    }
}
