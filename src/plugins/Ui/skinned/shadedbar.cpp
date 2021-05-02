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

#include <QMouseEvent>
#include <QPainter>
#include <QPainter>
#include <math.h>
#include "skin.h"
#include "shadedbar.h"

ShadedBar::ShadedBar(QWidget *parent, uint slider1, uint slider2, uint slider3) : QWidget(parent),
    m_slider1(slider1),
    m_slider2(slider2),
    m_slider3(slider3)
{
    m_skin = Skin::instance();
    m_ratio = m_skin->ratio();
    if(slider1 == Skin::EQ_VOLUME1)
        resize(m_ratio*97,m_ratio*7);
    else
        resize(m_ratio*42,m_ratio*7);
    connect(m_skin, SIGNAL(skinChanged()), this, SLOT(updateSkin()));
    draw();
}

ShadedBar::~ShadedBar()
{
}

void ShadedBar::mousePressEvent(QMouseEvent *e)
{
    m_moving = true;
    m_press_pos = e->position().x();
    if(m_pos < e->position().x() && e->position().x() < m_pos+3)
    {
        m_press_pos = e->position().x() - m_pos;
    }
    else
    {
        m_value = convert(qMax(qMin(width() - 3, qRound(e->position().x()) - 1), 0));
        m_press_pos = 1;
        if (m_value != m_old)
        {
            emit sliderMoved(m_value);
        }
    }
    draw();
}

void ShadedBar::mouseMoveEvent (QMouseEvent *e)
{
    if(m_moving)
    {
        int po = e->position().x();
        po = po - m_press_pos;

        if(0 <= po && po <= width() - 3)
        {
            m_value = convert(po);
            draw();
            emit sliderMoved(m_value);
        }
    }
}

void ShadedBar::mouseReleaseEvent(QMouseEvent*)
{
    m_moving = false;
    draw();
    m_old = m_value;
}

void ShadedBar::setValue(int v)
{
   if (m_moving || m_max == 0)
        return;
    m_value = v;
    draw();
}

void ShadedBar::setRange(int min, int max)
{
    m_max = max;
    m_min = min;
    draw();
}

void ShadedBar::updateSkin()
{
    m_ratio = m_skin->ratio();
    if(m_slider1 == Skin::EQ_VOLUME1)
        resize(m_ratio*97,m_ratio*7);
    else
        resize(m_ratio*42,m_ratio*7);
    draw();
}

void ShadedBar::draw()
{
    if (m_value <= m_min + (m_max - m_min)/3)
        m_pixmap = m_skin->getEqPart(m_slider1);
    else if(m_min + (m_max - m_min)/3 < m_value && m_value <= m_min + 2*(m_max - m_min)/3)
        m_pixmap = m_skin->getEqPart(m_slider2);
    else
        m_pixmap = m_skin->getEqPart(m_slider3);
    m_pos = int(ceil(double(m_value-m_min)*(width()-3*m_ratio)/(m_max-m_min)));
    update();
}

int ShadedBar::convert(int p)
{
    return int(ceil(double(m_max-m_min)*(p)/(width()-3)+m_min));
}

void ShadedBar::paintEvent(QPaintEvent*)
{
    QPainter paint(this);
    paint.drawPixmap(m_pos,0,m_pixmap);
}
