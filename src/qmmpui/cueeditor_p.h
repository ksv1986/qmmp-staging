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
#include <qmmp/cueparser.h>
#include <qmmp/trackinfo.h>

namespace Ui {
class CueEditor;
}

class MetaDataModel;

class CueEditor : public QWidget
{
    Q_OBJECT

public:
    explicit CueEditor(MetaDataModel *model, const TrackInfo &info, QWidget *parent = nullptr);
    ~CueEditor();

    void save();
    bool isEditable() const;
    int trackCount() const;

private slots:
    void on_loadButton_clicked();
    void on_deleteButton_clicked();
    void on_saveAsButton_clicked();

private:
    Ui::CueEditor *m_ui;
    MetaDataModel *m_model;
    QString m_lastDir;
    bool m_editable;
    TrackInfo m_info;
    CueParser m_parser;
};

#endif // CUEEDITOR_P_H
