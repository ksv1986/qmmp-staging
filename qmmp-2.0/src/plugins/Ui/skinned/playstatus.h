/***************************************************************************
 *   Copyright (C) 2006-2015 by Ilya Kotov                                 *
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
#ifndef PLAYSTATUS_H
#define PLAYSTATUS_H

#include "pixmapwidget.h"

class Skin;

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class PlayStatus : public PixmapWidget
{
Q_OBJECT
public:
    PlayStatus(QWidget *parent = nullptr);

    ~PlayStatus();

    enum Type
    {
       PLAY,
       STOP,
       PAUSE,
    };

void setStatus(Type);

private slots:
    void updateSkin();

private:
   Skin *m_skin;
   Type m_status;
};

#endif
