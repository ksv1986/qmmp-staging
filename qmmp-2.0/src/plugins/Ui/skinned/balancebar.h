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
#ifndef BALANCEBAR_H
#define BALANCEBAR_H

#include "pixmapwidget.h"

class Skin;

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class BalanceBar : public PixmapWidget
{
Q_OBJECT
public:
    BalanceBar(QWidget *parent = nullptr);

    ~BalanceBar();

    int value() {return m_value; }

public slots:
    void setValue(int);
    void setMax(int);

signals:
    void sliderMoved (int);
    void sliderPressed();
    void sliderReleased();

private slots:
    void updateSkin();

private:
    Skin *m_skin;
    bool m_moving = false;
    int press_pos;
    int m_max = 100, m_min = -100, m_pos, m_value = 0, m_old = 0;
    QPixmap m_pixmap;
    int convert(int);   // value = convert(position);
    void draw(bool pressed = true);

protected:
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
};

#endif
