/***************************************************************************
 *   Copyright (C) 2017 by Ilya Kotov                                      *
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

#include "dateinputdialog.h"
#include "ui_dateinputdialog.h"

DateInputDialog::DateInputDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::DateInputDialog)
{
    m_ui->setupUi(this);
}

DateInputDialog::~DateInputDialog()
{
    delete m_ui;
}

QDate DateInputDialog::selectedDate() const
{
    return m_ui->calendarWidget->selectedDate();
}

void DateInputDialog::setSelectedDate(const QDate &date)
{
    m_ui->calendarWidget->setSelectedDate(date);
}
