/***************************************************************************
 *   Copyright (C) 2009-2021 by Ilya Kotov                                 *
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
#ifndef HOTKEYDIALOG_H
#define HOTKEYDIALOG_H

#include <QDialog>

#include "ui_hotkeydialog.h"

class QKeyEvent;

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class HotkeyDialog : public QDialog
{
    Q_OBJECT
public:
    explicit HotkeyDialog(const QString &key, QWidget *parent = nullptr);

    ~HotkeyDialog();

    const QString key();

protected:
    virtual void keyPressEvent (QKeyEvent *event) override;

private:
    Ui::HotkeyDialog m_ui;

};

#endif
