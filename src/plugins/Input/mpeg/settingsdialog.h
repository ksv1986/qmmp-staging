/***************************************************************************
 *   Copyright (C) 2009-2019 by Ilya Kotov                                 *
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
#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include "ui_settingsdialog.h"

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class SettingsDialog : public QDialog
{
Q_OBJECT
public:
    SettingsDialog(bool using_rusxmms, QWidget *parent = nullptr);

    ~SettingsDialog();

    enum TagType {ID3v1 = 0, ID3v2, APE, Disabled};

public slots:
    void accept() override;

private:
    void findCodecs();
    Ui::SettingsDialog m_ui;
    QList<QTextCodec *> codecs;

};

#endif
