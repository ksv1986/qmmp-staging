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
#ifndef ANALYZER_H
#define ANALYZER_H

#include <QWidget>
#include <qmmp/visual.h>

class QTimer;
class QMenu;
class QAction;
class QActionGroup;
class QPainter;
class QPaintEvent;
class QHideEvent;
class QShowEvent;

class Analyzer : public Visual
{
    Q_OBJECT

public:
    explicit Analyzer( QWidget *parent = nullptr);
    virtual ~Analyzer();

public slots:
    void start() override;
    void stop() override;

private slots:
    void timeout();
    void toggleFullScreen();
    void readSettings();
    void writeSettings();

private:
    void clear();
    virtual void hideEvent (QHideEvent *) override;
    virtual void showEvent (QShowEvent *) override;
    virtual void closeEvent (QCloseEvent *) override;
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *e) override;
    void process();
    void draw(QPainter *p);
    void createMenu();
    QTimer *m_timer;
    double *m_intern_vis_data = nullptr;
    double *m_peaks = nullptr;
    int *m_x_scale = nullptr;
    double m_peaks_falloff;
    double m_analyzer_falloff;
    bool m_show_peaks;
    float m_left_buffer[QMMP_VISUAL_NODE_SIZE];
    float m_right_buffer[QMMP_VISUAL_NODE_SIZE];
    int m_cols = 0, m_rows = 0;
    bool m_update = false;
    bool m_running = false;
    //colors
    QColor m_color1;
    QColor m_color2;
    QColor m_color3;
    QColor m_bgColor;
    QColor m_peakColor;
    QSize m_cell_size;
    QMenu *m_menu;
    QAction *m_peaksAction;
    QActionGroup *m_fpsGroup;
    QActionGroup *m_analyzerFalloffGroup;
    QActionGroup *m_peaksFalloffGroup;
};


#endif
