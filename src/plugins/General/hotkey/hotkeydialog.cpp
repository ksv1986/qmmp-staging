/***************************************************************************
 *   Copyright (C) 2009-2018 by Ilya Kotov                                 *
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

#include <QKeyEvent>
#include "hotkeymanager.h"
#include "hotkeydialog.h"

HotkeyDialog::HotkeyDialog(quint32 key, quint32 mod, QWidget *parent)
        : QDialog(parent)
{
    m_ui.setupUi(this);
    m_key = key;
    m_modifiers = mod;
    m_ui.keyLineEdit->setText(HotkeyManager::getKeyString(m_key, m_modifiers));
}

HotkeyDialog::~HotkeyDialog()
{}

void HotkeyDialog::keyPressEvent (QKeyEvent *event)
{
    m_key = HotkeyManager::keycodeToKeysym(event->nativeScanCode ());
    m_modifiers = event->nativeModifiers ();
    foreach(long mask_mod, HotkeyManager::ignModifiersList())
        m_modifiers &= ~mask_mod; //remove ignored modifiers (num lock, caps lock, etc)

    m_ui.keyLineEdit->setText(HotkeyManager::getKeyString(m_key, m_modifiers));
    QDialog::keyPressEvent(event);
}

void HotkeyDialog::showEvent(QShowEvent *)
{
    grabKeyboard();
}

quint32 HotkeyDialog::nativeModifiers () const
{
    return m_modifiers;
}

quint32 HotkeyDialog::keySym () const
{
    return m_key;
}

void HotkeyDialog::accept()
{
    if (m_ui.keyLineEdit->text().isEmpty()) //clear key & modifiers
    {
        m_key = 0;
        m_modifiers = 0;
    }
    releaseKeyboard();
    QDialog::accept();
}

void HotkeyDialog::reject()
{
    releaseKeyboard();
    QDialog::reject();
}
