/***************************************************************************
 *   Copyright (C) 2018 by Ilya Kotov                                      *
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

#ifndef EDITSTREAMDIALOG_H
#define EDITSTREAMDIALOG_H

#include <QDialog>
#include <QMap>

namespace Ui {
class EditStreamDialog;
}

class EditStreamDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditStreamDialog(QWidget *parent = 0);
    ~EditStreamDialog();

    enum Key
    {
        URL = 0,
        NAME,
        GENRE,
        BITRATE,
        TYPE
    };

public slots:
    void accept();

    void setValues(const QMap<Key, QString> &values);
    const QMap<Key, QString> &values() const;

private slots:
    void validate();

private:
    Ui::EditStreamDialog *m_ui;
    QMap<Key, QString> m_values;
};

#endif // EDITSTREAMDIALOG_H
