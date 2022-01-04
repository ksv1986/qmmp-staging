/***************************************************************************
 *   Copyright (C) 2007-2021 by Ilya Kotov                                 *
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
#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QMainWindow>
#include <QPoint>
#include "pixmapwidget.h"
#include "playlist.h"

class MainWindow;
class QMouseEvent;
class Skin;
class Button;
class SymbolDisplay;
class TitleBarControl;
class ShadedVisual;
class TimeIndicatorModel;

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class TitleBar : public PixmapWidget
{
Q_OBJECT
public:
    explicit TitleBar(TimeIndicatorModel *model, QWidget *parent = nullptr);

    ~TitleBar();

    void setActive(bool);

private slots:
    void onModelChanged();
    void updateSkin();
    void showMainMenu();
    void shade();

private:
    Skin *m_skin;
    QPoint m_pos;
    MainWindow *m_mw;
    Button *m_menu;
    Button *m_minimize;
    Button *m_shade;
    Button *m_shade2 = nullptr;
    Button *m_close;
    SymbolDisplay *m_currentTime = nullptr;
    QString formatTime (int);
    bool m_shaded = false;
    bool m_align = false;
    TitleBarControl *m_control = nullptr;
    ShadedVisual *m_visual = nullptr;
    TimeIndicatorModel *m_model;
    void updatePositions();

protected:
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseDoubleClickEvent(QMouseEvent*) override;
};



#endif
