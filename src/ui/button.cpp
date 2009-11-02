/***************************************************************************
 *   Copyright (C) 2007-2008 by Ilya Kotov                                 *
 *   forkotov02@hotmail.ru                                                 *
 *                                                                         *
 *   Based on Promoe, an XMMS2 Client                                      *
 *   Copyright (C) 2005-2006 by XMMS2 Team                                 *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "button.h"
#include "skin.h"
#include <QMouseEvent>

Button::Button (QWidget *parent, uint normal, uint pressed, uint cursor)
        : PixmapWidget (parent)
{
    name_normal = normal;
    name_pressed = pressed;
    name_cursor = cursor;
    skin = Skin::instance();
    setON (FALSE);
    setCursor (skin->getCursor (name_cursor));
    connect (skin, SIGNAL (skinChanged()), this, SLOT (updateSkin()));
}


Button::~Button()
{}

void Button::updateSkin()
{
    setPixmap (skin->getButton (name_normal));
    setCursor (skin->getCursor (name_cursor));
}

void Button::setON (bool on)
{
    if (on)
        setPixmap (skin->getButton (name_pressed));
    else
        setPixmap (skin->getButton (name_normal));
}
void Button::mousePressEvent (QMouseEvent *e)
{
    setON (TRUE);
    m_cursorin = TRUE;
    QWidget::mousePressEvent(e);
}

void Button::mouseReleaseEvent (QMouseEvent*)
{
    setON (FALSE);
    if (m_cursorin)
        emit clicked();
}

void Button::mouseMoveEvent (QMouseEvent *e)
{
    if ( !m_cursorin && rect().contains(e->pos()) ) {
        m_cursorin = TRUE;
        setON (TRUE);
    } else if ( m_cursorin && !rect().contains(e->pos()) ) {
        m_cursorin = FALSE;
        setON (FALSE);
    }
}
