/***************************************************************************
 *   Copyright (C) 2012-2019 by Ilya Kotov                                 *
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
#ifndef QSUIANALYZER_H
#define QSUIANALYZER_H

#include <QWidget>
#include <QResizeEvent>
#include <qmmp/visual.h>

class QTimer;
class QMenu;
class QActionGroup;
class QLabel;


class QSUiAnalyzer : public Visual
{
    Q_OBJECT
public:
    QSUiAnalyzer(QWidget *parent = nullptr);
    virtual ~QSUiAnalyzer();

    void setCover(const QPixmap &pixmap);
    void clear();
    void clearCover();
    QSize sizeHint() const override;

public slots:
    void start() override;
    void stop() override;
    void readSettings();

private slots:
    void timeout();
    void writeSettings();

private:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void hideEvent(QHideEvent *) override;
    void showEvent(QShowEvent *) override;
    void resizeEvent(QResizeEvent *) override;
    void process();
    void draw(QPainter *p);
    void createMenu();
    void updateCover();

    QPixmap m_cover;
    QTimer *m_timer;
    double *m_intern_vis_data = nullptr;
    double *m_peaks = nullptr;
    int *m_x_scale = nullptr;
    double m_peaks_falloff;
    double m_analyzer_falloff;
    bool m_show_peaks;
    bool m_show_cover = false;
    float m_buffer[QMMP_VISUAL_NODE_SIZE];
    int m_cols = 0, m_rows = 0;
    int m_offset = 0;
    bool m_update = false;
    QLabel *m_pixLabel;
    //colors
    QColor m_color1;
    QColor m_color2;
    QColor m_color3;
    QColor m_bgColor;
    QColor m_peakColor;
    QSize m_cell_size;
    QMenu *m_menu;
    QAction *m_peaksAction;
    QAction *m_coverAction;
    QActionGroup *m_fpsGroup;
    QActionGroup *m_analyzerFalloffGroup;
    QActionGroup *m_peaksFalloffGroup;
    bool m_running = false;
};


#endif //QSUIANALYZER_H
