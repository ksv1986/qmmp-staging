/***************************************************************************
 *   Copyright (C) 2010-2021 by Ilya Kotov                                 *
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
#include "shortcutdialog.h"

ShortcutDialog::ShortcutDialog(const QString &key, QWidget *parent)
        : QDialog(parent)
{
    m_ui.setupUi(this);
    m_ui.keyLineEdit->setText(key);

    //buttons should not catch keys
    for(QAbstractButton *button : m_ui.buttonBox->buttons())
        button->setFocusPolicy(Qt::NoFocus);
}

ShortcutDialog::~ShortcutDialog()
{
}

void ShortcutDialog::keyPressEvent (QKeyEvent *event)
{
    int key = event->key();
    switch (key)
    {
    case Qt::Key_Shift:
    case Qt::Key_Control:
    case Qt::Key_Meta:
    case Qt::Key_Alt:
    case Qt::Key_AltGr:
    case Qt::Key_Super_L:
    case Qt::Key_Super_R:
    case Qt::Key_Menu:
    case 0:
    case Qt::Key_unknown:
        m_ui.keyLineEdit->clear();
        QWidget::keyPressEvent(event);
        return;
    }
    QKeySequence seq(event->keyCombination());
    m_ui.keyLineEdit->setText(seq.toString());
    QWidget::keyPressEvent(event);
}

const QString ShortcutDialog::key()
{
    return m_ui.keyLineEdit->text();
}
