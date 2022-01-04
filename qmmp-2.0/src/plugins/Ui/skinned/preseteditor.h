/***************************************************************************
 *   Copyright (C) 2006-2021 by Ilya Kotov                                 *
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
#ifndef PRESETEDITOR_H
#define PRESETEDITOR_H

#include <QDialog>

#include "ui_preseteditor.h"

/**
	@author Ilya Kotov <forkotov02@ya.ru>
*/

class EQPreset;

class PresetEditor : public QDialog
{
Q_OBJECT
public:
    PresetEditor(QWidget *parent = nullptr);

    ~PresetEditor();

    void addPresets(const QList<EQPreset*>&);
    void addAutoPresets(const QList<EQPreset*>&);

signals:
    void presetLoaded(EQPreset*);
    void presetDeleted(EQPreset*);

private slots:
    void loadPreset();
    void deletePreset();

private:
    Ui::PresetEditor m_ui;

};

#endif
