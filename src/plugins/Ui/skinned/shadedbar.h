/***************************************************************************
 *   Copyright (C) 2007-2020 by Ilya Kotov                                 *
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
#ifndef SHADEDBAR_H
#define SHADEDBAR_H

#include <QWidget>

class QMouseEvent;
class QPaintEvent;
class Skin;

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class ShadedBar : public QWidget
{
    Q_OBJECT
public:
    ShadedBar(QWidget *parent = nullptr, uint slider1 = 0, uint slider2 = 0, uint slider3 = 0);

    ~ShadedBar();
    inline int value()
    {
        return m_value;
    }
    inline int isPressed()
    {
        return m_moving;
    }
    void setRange(int min, int max);

public slots:
    void setValue(int);

signals:
    void sliderMoved (int);

private slots:
    void updateSkin();

private:
    Skin *m_skin;
    bool m_moving = false;
    int m_press_pos = 0;
    int m_max = 100, m_min = 0, m_pos = 0, m_value = 0, m_old;
    QPixmap m_pixmap;
    int convert(int);   // value = convert(position);
    void draw();
    uint m_slider1, m_slider2, m_slider3;
    int m_ratio;

protected:
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void paintEvent(QPaintEvent*) override;
};

#endif
