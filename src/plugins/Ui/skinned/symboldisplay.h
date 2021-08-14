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
#ifndef SYMBOLDISPLAY_H
#define SYMBOLDISPLAY_H

#include <QPixmap>
#include "pixmapwidget.h"

class Skin;

/**
   @author Vladimir Kuznetsov <vovanec@gmail.com>
 */
class SymbolDisplay : public PixmapWidget
{
    Q_OBJECT
public:
    explicit SymbolDisplay(QWidget *parent = nullptr, int digits = 3);

    virtual ~SymbolDisplay();

    void setAlignment(Qt::Alignment a);
    Qt::Alignment alignment() const;

public slots:
    void display(const QString&);
    void display(int);

private slots:
    void draw();

private:
    Skin* m_skin;
    QString m_text;
    Qt::Alignment m_alignment = Qt::AlignRight;
    int m_digits;
    int m_max = 0;
};

#endif
