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
#ifndef SHADEDVISUAL_H
#define SHADEDVISUAL_H

#include <QPainter>
#include <qmmp/visual.h>
#include <qmmp/buffer.h>

class QTimer;
class QPixmap;
class Skin;

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class ShadedVisual : public Visual
{
    Q_OBJECT
public:
    explicit ShadedVisual(QWidget *parent = nullptr);

    ~ShadedVisual();

public slots:
    void timeout();
    void start() override;
    void stop() override;

private slots:
    void updateSkin();

private:
    void clear();
    void paintEvent (QPaintEvent *) override;
    void hideEvent (QHideEvent *) override;
    void showEvent (QShowEvent *) override;
    void process ();
    void draw (QPainter *);

    Skin *m_skin;
    QTimer *m_timer;
    QPixmap m_pixmap;
    float m_left_buffer[QMMP_VISUAL_NODE_SIZE];
    float m_right_buffer[QMMP_VISUAL_NODE_SIZE];
    double m_l, m_r;
    int m_ratio;
    bool m_running = false;

};

#endif
