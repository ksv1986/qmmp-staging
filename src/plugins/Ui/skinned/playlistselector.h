/***************************************************************************
 *   Copyright (C) 2009-2020 by Ilya Kotov                                 *
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

#ifndef PLAYLISTSELECTOR_H
#define PLAYLISTSELECTOR_H

#include <QWidget>
#include <QColor>
#include <QPixmap>

class QFontMetrics;
class QFont;
class QMouseEvent;
class QMenu;
class PlayListManager;
class PlayList;

/**
   @author Ilya Kotov <forkotov02@ya.ru>
*/
class PlayListSelector : public QWidget
{
Q_OBJECT
public:
    PlayListSelector(PlayListManager *manager, QWidget *parent = nullptr);
    ~PlayListSelector();
    void readSettings();

private slots:
    void updateTabs();

private:
    enum BUTTON
    {
        BUTTON_UNKNOWN = -1,
        BUTTON_NEW_PL,
        BUTTON_LEFT,
        BUTTON_RIGHT
    };

    void paintEvent(QPaintEvent *) override;
    void mousePressEvent (QMouseEvent *e) override;
    void mouseReleaseEvent (QMouseEvent *e) override;
    void mouseDoubleClickEvent (QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void resizeEvent (QResizeEvent *) override;
    void updateOffsets();
    void drawButtons();
    void updateScrollers();
    QRect firstVisible();
    QRect lastVisible();
    int findPlayList(QPoint pos);
    int findButton(QPoint pos);

    PlayListManager *m_pl_manager = nullptr;
    QFontMetrics *m_metrics = nullptr;
    QFont m_font;
    QMenu *m_menu;
    bool m_scrollable = false;
    QList <QRect> m_rects;
    QList <QRect> m_extra_rects;
    QColor m_normal, m_current, m_normal_bg, m_selected_bg, m_selected_text, m_current_bg;
    QPixmap m_pixmap;
    bool m_show_new_pl_button;
    int m_offset = 0, m_offset_max = 0, m_press_offset = -1;
    bool m_moving = false;
    QPoint m_mouse_pos;
    QString m_pl_separator;
    QString m_pl_button;
    int m_pressed_button = BUTTON_UNKNOWN;


};

#endif // PLAYLISTSELECTOR_H
