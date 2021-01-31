/***************************************************************************
 *   Copyright (C) 2021 by Ilya Kotov                                      *
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

#ifndef CUEEDITOR_P_H
#define CUEEDITOR_P_H

#include <QWidget>

namespace Ui {
class CueEditor;
}

class MetaDataModel;

class CueEditor : public QWidget
{
    Q_OBJECT

public:
    explicit CueEditor(MetaDataModel *model, QWidget *parent = nullptr);
    ~CueEditor();

    void save();

private slots:
    void on_loadButton_clicked();
    void on_deleteButton_clicked();
    void on_saveAsButton_clicked();

private:
    Ui::CueEditor *m_ui;
    MetaDataModel *m_model;
    QString m_lastDir;
};

#endif // CUEEDITOR_P_H
