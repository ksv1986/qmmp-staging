/***************************************************************************
 *   Copyright (C) 2015-2016 by Ilya Kotov                                 *
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

#ifndef PLAYLISTHEADER_H
#define PLAYLISTHEADER_H

#include <QWidget>
#include <QPixmap>

class QFontMetrics;
class QFont;
class QMouseEvent;
class QContextMenuEvent;
class QMenu;
class QAction;
class QStyleOptionHeader;
class PlayListHeaderModel;

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class PlayListHeader : public QWidget
{
    Q_OBJECT
public:
    explicit PlayListHeader(QWidget *parent = nullptr);
    ~PlayListHeader();

    void readSettings();
    void setNumberWidth(int width);
    void setScrollBarWidth(int width);
    int requiredHeight() const;
    QList<int> sizes() const;
    QList<int> alignment() const;
    int trackStateColumn() const;
    int maxScrollValue() const;
    int offset() const;
    bool hasAutoResizeColumn() const;

public slots:
    void scroll(int offset);
    void updateColumns();
    void showSortIndicator(int column, bool reverted);
    void hideSortIndicator();

private slots:
    void addColumn();
    void editColumn();
    void removeColumn();
    void setAutoResize(bool on);
    void showTrackState(bool yes);
    void setAlignment(QAction *action);
    void onColumnAdded(int index);
    void onColumnRemoved();

private:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    void contextMenuEvent(QContextMenuEvent *e) override;
    void paintEvent(QPaintEvent *) override;
    int findColumn(QPoint pos);
    void initStyleOption(QStyleOptionHeader *opt);
    int size(int index) const;
    void setSize(int index, int size);
    const QString name(int index) const;
    void adjustColumn(int index);
    int autoResizeColumn() const;
    void writeSettings();
    void showEvent(QShowEvent *) override;
    void hideEvent(QHideEvent *) override;

    enum DataKey
    {
        NAME = 0,
        SIZE,
        RECT, //geometry
        AUTO_RESIZE,
        TRACK_STATE,
        ALIGNMENT
    };

    QSize m_size_hint;
    QFontMetrics *m_metrics = nullptr;
    QMenu *m_menu, *m_alignmentMenu;
    QPoint m_pressed_pos;
    QPoint m_mouse_pos;
    PlayListHeaderModel *m_model;
    QAction *m_trackStateAction;
    QAction *m_autoResizeAction;
    bool m_reverted = false;
    bool m_auto_resize = false;
    int m_number_width = 0;
    int m_scrollbar_width = 0;
    int m_pressed_column;
    int m_old_size;
    int m_press_offset;
    int m_pl_padding = 0;
    int m_sorting_column = -1;

    int m_offset = 0;

    enum
    {
        NO_TASK = -1,
        RESIZE,
        MOVE,
        SORT
    } m_task = NO_TASK;


};

#endif // PLAYLISTHEADER_H
