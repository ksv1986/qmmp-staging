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
#ifndef EQWIDGET_H
#define EQWIDGET_H

#include "pixmapwidget.h"
#include <qmmp/output.h>

/**
   @author Ilya Kotov <forkotov02@ya.ru>
*/

class QMenu;
class Skin;
class EqTitleBar;
class EqSlider;
class ToggleButton;
class EQGraph;
class Button;
class EQPreset;
class PlayListTrack;
class SoundCore;

class EqWidget : public PixmapWidget
{
    Q_OBJECT
public:
    explicit EqWidget(QWidget *parent = nullptr);

    ~EqWidget();

    /*!
    * necessary for auto-load presets
    */
    void loadPreset(const QString &name);
    void setMimimalMode(bool b = true);

    void writeSettings();

signals:
    void closed();

private slots:
    void updateSkin();
    void readEq();
    void writeEq();
    void showPresetsMenu();
    void reset();
    void showEditor();
    void savePreset();
    void saveAutoPreset();
    void setPreset(EQPreset*);
    void deletePreset(EQPreset*);
    void importWinampEQF();

private:
    void updatePositions();
    void readSettings();
    void createActions();
    void updateMask();
    EQPreset *findPreset(const QString &name);
    //events
    void keyPressEvent (QKeyEvent *) override;
    void changeEvent(QEvent*) override;
    void closeEvent(QCloseEvent*) override;
#ifdef QMMP_WS_X11
    bool event (QEvent *event) override;
#endif

    Skin *m_skin;
    EqTitleBar *m_titleBar;
    EqSlider *m_preamp;
    Button *m_presetButton;
    QList<EqSlider*> m_sliders;
    ToggleButton *m_on;
    ToggleButton *m_autoButton;
    EQGraph *m_eqg;
    QMenu *m_presetsMenu;
    QList<EQPreset*> m_presets;
    QList<EQPreset*> m_autoPresets;
    bool m_shaded;
};

#endif
