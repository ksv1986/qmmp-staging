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
#ifndef TIMEINDICATOR_H
#define TIMEINDICATOR_H

#include "pixmapwidget.h"

class QMouseEvent;

class Skin;


/** Class TimeIndicatorModel
 * @author Thomas Perl <m@thp.io>
 *
 * Encapsulates state and settings for the time indicator
 */
class TimeIndicatorModel : public QObject {
    Q_OBJECT
public:
    explicit TimeIndicatorModel(QObject *parent = nullptr);
    ~TimeIndicatorModel();

    inline int position() const { return m_position; }
    inline int duration() const { return m_duration; }
    inline bool elapsed() const { return m_elapsed; }
    inline bool visible() const { return m_visible; }

    void setPosition(int position);
    void setDuration(int duration);
    void setElapsed(bool elapsed);
    void setVisible(bool visible);

    int displayTime();

public slots:
    void toggleElapsed();

signals:
    void changed();

private:
    void readSettings();
    void writeSettings();

    int m_position = 0;
    int m_duration = 0;
    bool m_elapsed = true;
    bool m_visible = false;
};


/** Class TimeIndicator
 *  @author Vladimir Kuznetsov <vovanec@gmail.com>
 *
 *  Represents time indicator in the main display. Can show elapsed
 *  and rest time of song (mouse press on indicator changes mode)
 */
class TimeIndicator : public PixmapWidget
{
    Q_OBJECT
public:
    explicit TimeIndicator(TimeIndicatorModel *model, QWidget *parent = nullptr);
    ~TimeIndicator();

protected:
    virtual void mousePressEvent(QMouseEvent*) override;
    virtual void mouseMoveEvent(QMouseEvent*) override;
    virtual void mouseReleaseEvent(QMouseEvent*) override;

private slots:
    void modelChanged();
    void updateSkin();

private:
    TimeIndicatorModel *m_model;
    QPixmap m_pixmap;
    Skin *m_skin;
};

#endif
