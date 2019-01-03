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

#ifndef COVEREDITOR_P_H
#define COVEREDITOR_P_H

#include <QWidget>
#include <QString>
#include <qmmp/metadatamodel.h>
#include "ui_covereditor.h"

class CoverViewer;

/**
   @internal
   @author Ilya Kotov <forkotov02@ya.ru>
*/

class CoverEditor : public QWidget
{
    Q_OBJECT
public:
    explicit CoverEditor(MetaDataModel *model, const QString &coverPath, QWidget *parent = nullptr);

    bool isEditable() const;
    void save();

private slots:
    void on_sourceComboBox_activated(int index);
    void on_loadButton_clicked();
    void on_deleteButton_clicked();
    void on_saveAsButton_clicked();

private:
    Ui::CoverEditor m_ui;
    MetaDataModel *m_model;
    CoverViewer *m_viewer;
    QString m_coverPath;
    bool m_editable;

};

#endif // COVEREDITOR_P_H
