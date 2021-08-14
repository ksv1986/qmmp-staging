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
#ifndef BUTTON_H
#define BUTTON_H

#include "pixmapwidget.h"

class Skin;

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class Button : public PixmapWidget
{
Q_OBJECT
public:
    Button(QWidget *parent, uint normal, uint pressed, uint cursor);
    ~Button();

signals:
    void clicked();

private slots:
    void updateSkin();

private:
    Skin *m_skin;
    //bool m_cursorin;
    bool m_pressed = false;
    void setON(bool);
    uint m_name_normal, m_name_pressed, m_name_cursor;

protected:
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
};

#endif
