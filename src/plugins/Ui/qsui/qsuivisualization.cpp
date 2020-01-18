/***************************************************************************
 *   Copyright (C) 2012-2020 by Ilya Kotov                                 *
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
#include <QTimer>
#include <QSettings>
#include <QPainter>
#include <QMenu>
#include <QActionGroup>
#include <QLabel>
#include <qmmp/qmmp.h>
#include <qmmp/buffer.h>
#include <math.h>
#include <stdlib.h>
#include "fft.h"
#include "inlines.h"
#include "qsuivisualization.h"

QSUIVisualization::QSUIVisualization(QWidget *parent) : Visual (parent)
{
    m_pixLabel = new QLabel(this);
    createMenu();

    m_timer = new QTimer (this);
    connect(m_timer, SIGNAL (timeout()), this, SLOT (timeout()));

    readSettings();
    clear();
}

QSUIVisualization::~QSUIVisualization()
{
    if(m_drawer)
        delete m_drawer;
}

void QSUIVisualization::clear()
{
    if(m_drawer)
    {
        m_drawer->clear();
        update();
    }
}

void QSUIVisualization::clearCover()
{
    m_cover = QPixmap();
    updateCover();
    update();
}

QSize QSUIVisualization::sizeHint() const
{
    return QSize(200, 100);
}

void QSUIVisualization::setCover(const QPixmap &pixmap)
{
    m_cover = pixmap;
    updateCover();
}

void QSUIVisualization::timeout()
{
    if(takeData(m_buffer))
    {
        process();
        update();
    }
}

void QSUIVisualization::paintEvent (QPaintEvent * e)
{
    if(m_drawer)
    {
        QPainter painter (this);
        painter.fillRect(e->rect(),m_bgColor);
        m_drawer->draw(&painter, m_offset);
    }
}

void QSUIVisualization::hideEvent (QHideEvent *)
{
    m_timer->stop();
}

void QSUIVisualization::showEvent (QShowEvent *)
{
    if(m_running)
        m_timer->start();
}

void QSUIVisualization::resizeEvent(QResizeEvent *)
{
    updateCover();
}

void QSUIVisualization::process()
{
    if(m_drawer)
        m_drawer->process(m_buffer, width() - m_offset, height());
}

void QSUIVisualization::createMenu()
{
    m_menu = new QMenu (this);
    connect(m_menu, SIGNAL(triggered (QAction *)),SLOT(writeSettings()));
    connect(m_menu, SIGNAL(triggered (QAction *)),SLOT(readSettings()));

    m_coverAction = m_menu->addAction(tr("Cover"));
    m_coverAction->setCheckable(true);

    QMenu *visMode = m_menu->addMenu(tr("Visualization Mode"));
    m_visModeGroup = new QActionGroup(this);
    m_visModeGroup->setExclusive(true);
    m_visModeGroup->addAction(tr("Analyzer"))->setData("analyzer");
    m_visModeGroup->addAction(tr("Scope"))->setData("scope");
    for(QAction *act : m_visModeGroup->actions())
    {
        act->setCheckable(true);
        visMode->addAction(act);
    }

    QMenu *analyzerMode = m_menu->addMenu(tr("Analyzer Mode"));
    m_analyzerTypeGroup = new QActionGroup(this);
    m_analyzerTypeGroup->addAction(tr("Cells"))->setData("cells");
    m_analyzerTypeGroup->addAction(tr("Lines"))->setData("lines");

    for(QAction *act : m_analyzerTypeGroup->actions())
    {
        act->setCheckable(true);
        analyzerMode->addAction(act);
    }
    analyzerMode->addSeparator ();
    m_peaksAction = analyzerMode->addAction(tr("Peaks"));
    m_peaksAction->setCheckable(true);

    QMenu *refreshRate = m_menu->addMenu(tr("Refresh Rate"));
    m_fpsGroup = new QActionGroup(this);
    m_fpsGroup->setExclusive(true);
    m_fpsGroup->addAction(tr("50 fps"))->setData(50);
    m_fpsGroup->addAction(tr("25 fps"))->setData(25);
    m_fpsGroup->addAction(tr("10 fps"))->setData(10);
    m_fpsGroup->addAction(tr("5 fps"))->setData(5);
    for(QAction *act : m_fpsGroup->actions())
    {
        act->setCheckable(true);
        refreshRate->addAction(act);
    }

    QMenu *analyzerFalloff = m_menu->addMenu(tr("Analyzer Falloff"));
    m_analyzerFalloffGroup = new QActionGroup(this);
    m_analyzerFalloffGroup->setExclusive(true);
    m_analyzerFalloffGroup->addAction(tr("Slowest"))->setData(1.2);
    m_analyzerFalloffGroup->addAction(tr("Slow"))->setData(1.8);
    m_analyzerFalloffGroup->addAction(tr("Medium"))->setData(2.2);
    m_analyzerFalloffGroup->addAction(tr("Fast"))->setData(2.4);
    m_analyzerFalloffGroup->addAction(tr("Fastest"))->setData(2.8);
    for(QAction *act : m_analyzerFalloffGroup->actions())
    {
        act->setCheckable(true);
        analyzerFalloff->addAction(act);
    }

    QMenu *peaksFalloff = m_menu->addMenu(tr("Peaks Falloff"));
    m_peaksFalloffGroup = new QActionGroup(this);
    m_peaksFalloffGroup->setExclusive(true);
    m_peaksFalloffGroup->addAction(tr("Slowest"))->setData(0.05);
    m_peaksFalloffGroup->addAction(tr("Slow"))->setData(0.1);
    m_peaksFalloffGroup->addAction(tr("Medium"))->setData(0.2);
    m_peaksFalloffGroup->addAction(tr("Fast"))->setData(0.4);
    m_peaksFalloffGroup->addAction(tr("Fastest"))->setData(0.8);
    for(QAction *act : m_peaksFalloffGroup->actions())
    {
        act->setCheckable(true);
        peaksFalloff->addAction(act);
    }
    update();
}

void QSUIVisualization::updateCover()
{
    if(m_show_cover && !m_cover.isNull())
    {
        m_offset = height();
        m_pixLabel->setGeometry(10,10, height() - 20, height() - 20);
        m_pixLabel->setPixmap(m_cover.scaled(m_pixLabel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        m_pixLabel->show();
    }
    else
    {
        m_offset = 0;
        m_pixLabel->hide();
    }
}

void QSUIVisualization::mousePressEvent (QMouseEvent *e)
{
    if (e->button() == Qt::RightButton)
        m_menu->exec(e->globalPos());
}

void QSUIVisualization::readSettings()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Simple");
    //general settings
    m_show_cover = settings.value("vis_show_cover", true).toBool();
    m_timer->setInterval(1000 / settings.value("vis_refresh_rate", 25).toInt());
    QString visName = settings.value("vis_type","analyzer").toString();
    m_bgColor.setNamedColor(settings.value("vis_bg_color", "Black").toString());
    //analyzer settings
    double peaks_falloff = settings.value("vis_peaks_falloff", 0.2).toDouble();
    double analyzer_falloff = settings.value("vis_analyzer_falloff", 2.2).toDouble();
    bool show_peaks = settings.value("vis_show_peaks", true).toBool();
    QString analyzer_type = settings.value("vis_analyzer_type", "cells").toString();

    if(!m_update)
    {
        m_update = true;
        m_coverAction->setChecked(m_show_cover);
        m_peaksAction->setChecked(show_peaks);

        for(QAction *act : m_visModeGroup->actions())
        {
            if (visName == act->data().toString())
                act->setChecked(true);
        }

        for(QAction *act : m_fpsGroup->actions ())
        {
            if (m_timer->interval() == 1000 / act->data().toInt())
                act->setChecked(true);
        }
        for(QAction *act : m_peaksFalloffGroup->actions ())
        {
            if (peaks_falloff == act->data().toDouble())
                act->setChecked(true);
        }
        for(QAction *act : m_analyzerFalloffGroup->actions ())
        {
            if (analyzer_falloff == act->data().toDouble())
                act->setChecked(true);
        }
        for(QAction *act : m_analyzerTypeGroup->actions())
        {
            if(analyzer_type == act->data().toString())
                act->setChecked(true);
        }

    }
    updateCover();
    settings.endGroup();

    if(!m_drawer || m_drawer->name() != visName)
    {
        if(m_drawer)
            delete m_drawer;

        if(visName == "scope")
            m_drawer = new QSUiScope;
        else
            m_drawer = new QSUiAnalyzer;
    }
    m_drawer->readSettings();
    m_drawer->clear();
}

void QSUIVisualization::writeSettings()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Simple");

    QAction *act = m_fpsGroup->checkedAction ();
    settings.setValue("vis_refresh_rate", act ? act->data().toInt() : 25);
    act = m_peaksFalloffGroup->checkedAction ();
    settings.setValue("vis_peaks_falloff", act ? act->data().toDouble() : 0.2);
    act = m_analyzerFalloffGroup->checkedAction ();
    settings.setValue("vis_analyzer_falloff", act ? act->data().toDouble() : 2.2);
    settings.setValue("vis_show_peaks", m_peaksAction->isChecked());
    settings.setValue("vis_show_cover", m_coverAction->isChecked());
    act = m_visModeGroup->checkedAction();
    settings.setValue("vis_type", act ? act->data().toString() : "none");
    act = m_analyzerTypeGroup->checkedAction();
    settings.setValue("vis_analyzer_type", act ? act->data().toString() : "none");
    settings.endGroup();
}

void QSUIVisualization::start()
{
    m_running = true;
    if(isVisible())
        m_timer->start();
}

void QSUIVisualization::stop()
{
    m_running = false;
    m_timer->stop();
    clear();
}

QSUiScope::~QSUiScope()
{
    if(m_intern_vis_data)
        delete [] m_intern_vis_data;
}

QString QSUiScope::name() const
{
    return "scope";
}

void QSUiScope::process(float *buffer, int width, int height)
{
    int step = (QMMP_VISUAL_NODE_SIZE << 8) / width;
    int pos = 0;

    if(m_width != width )
    {
        m_width = width;

        if(m_intern_vis_data)
            delete [] m_intern_vis_data;

        m_intern_vis_data = new int[m_width]{ 0 };
    }

    if(m_height != height)
    {
        m_height = height;
        QLinearGradient gradient(0, 0, 0, m_height);
        gradient.setColorAt(0.1, m_color3);
        gradient.setColorAt(0.3, m_color2);
        gradient.setColorAt(0.5, m_color1);
        gradient.setColorAt(0.7, m_color2);
        gradient.setColorAt(0.9, m_color3);
        QBrush brush(gradient);
        m_pen.setWidthF(1.5);
        m_pen.setJoinStyle(Qt::RoundJoin);
        m_pen.setCapStyle(Qt::RoundCap);
        m_pen.setBrush(brush);
    }

    for(int i = 0; i < width - 4; ++i)
    {
        pos += step;
        m_intern_vis_data[i] = int(buffer[pos >> 8] * height / 2);
        m_intern_vis_data[i] = qBound(-height / 2, m_intern_vis_data[i], height / 2);
    }
}

void QSUiScope::draw(QPainter *p, int offset)
{
    p->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    for (int i = 0; i < m_width - 5; ++i)
    {
        int h1 = m_height / 2 - m_intern_vis_data[i];
        int h2 = m_height / 2 - m_intern_vis_data[i+1];
        p->setPen(m_pen);
        p->drawLine(i + offset, h1, offset + i + 1, h2);
    }
}

void QSUiScope::clear()
{
    m_width = 0;
    m_height = 0;
}

void QSUiScope::readSettings()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Simple");
    m_color1.setNamedColor(settings.value("vis_color1", "#BECBFF").toString());
    m_color2.setNamedColor(settings.value("vis_color2", "#BECBFF").toString());
    m_color3.setNamedColor(settings.value("vis_color3", "#BECBFF").toString());
    settings.endGroup();
}

QSUiVisualDrawer::~QSUiVisualDrawer()
{}

QSUiAnalyzer::~QSUiAnalyzer()
{
    if(m_intern_vis_data)
        delete [] m_intern_vis_data;
    if(m_peaks)
        delete [] m_peaks;
    if(m_x_scale)
        delete [] m_x_scale;
}

QString QSUiAnalyzer::name() const
{
    return "analyzer";
}

void QSUiAnalyzer::process(float *buffer, int width, int height)
{
    int rows = qMax((height - 2) / m_cell_size.height(),2);
    int cols = qMax((width - 2) / m_cell_size.width(),1);
    m_height = height;

    if(m_rows != rows || m_cols != cols)
    {
        m_rows = rows;
        m_cols = cols;
        if(m_peaks)
            delete [] m_peaks;
        if(m_intern_vis_data)
            delete [] m_intern_vis_data;
        if(m_x_scale)
            delete [] m_x_scale;
        m_peaks = new double[m_cols];
        m_intern_vis_data = new double[m_cols] { 0 };
        m_x_scale = new int[m_cols + 1] { 0 };

        for(int i = 0; i < m_cols + 1; ++i)
            m_x_scale[i] = pow(pow(255.0, 1.0 / m_cols), i);

        QLinearGradient gradient(0, 0, 0, m_height);
        gradient.setColorAt(0.33, m_color3);
        gradient.setColorAt(0.66, m_color2);
        gradient.setColorAt(1.0, m_color1);
        m_brush = QBrush(gradient);
    }
    short dest[256];

    calc_freq(dest, buffer);

    double y_scale = (double) 1.25 * m_rows / log(256);

    for (int i = 0; i < m_cols; i++)
    {
        short y = 0;
        int magnitude = 0;

        if(m_x_scale[i] == m_x_scale[i + 1])
        {
            y = dest[i];
        }
        for (int k = m_x_scale[i]; k < m_x_scale[i + 1]; k++)
        {
            y = qMax(dest[k], y);
        }

        y >>= 7; //256


        if (y)
        {
            magnitude = int(log (y) * y_scale);
            magnitude = qBound(0, magnitude, m_rows);
        }


        m_intern_vis_data[i] -= m_analyzer_falloff * m_rows / 15;
        m_intern_vis_data[i] = magnitude > m_intern_vis_data[i] ? magnitude : m_intern_vis_data[i];

        if (m_show_peaks)
        {
            m_peaks[i] -= m_peaks_falloff * m_rows / 15;
            m_peaks[i] = magnitude > m_peaks[i] ? magnitude : m_peaks[i];
        }
    }
}

void QSUiAnalyzer::draw(QPainter *p, int offset)
{
    for (int j = 0; j < m_cols; ++j)
    {
        int x = offset + j * m_cell_size.width() + 1;

        if(m_analyzerType == Cells)
        {
            for (int i = 1; i <= m_intern_vis_data[j]; ++i)
            {
                p->fillRect(x, m_height - (i - 1) * m_cell_size.height(),
                            m_cell_size.width() - 1, m_cell_size.height() - 4, m_brush);
            }
        }
        else if(m_analyzerType == Lines && m_intern_vis_data[j] > 0)
        {
            p->fillRect(x, m_height - 4, m_cell_size.width() - 1, -int(m_intern_vis_data[j] - 1)* m_cell_size.height() + 4, m_brush);
        }

        if (m_show_peaks && m_peaks[j] > 1)
        {
            p->fillRect(x, m_height - int(m_peaks[j] - 1) * m_cell_size.height(),
                        m_cell_size.width() - 1, m_cell_size.height() - 4, m_peakColor);
        }
    }
}

void QSUiAnalyzer::clear()
{
    m_rows = 0;
    m_cols = 0;
}

void QSUiAnalyzer::readSettings()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Simple");
    m_color1.setNamedColor(settings.value("vis_color1", "#BECBFF").toString());
    m_color2.setNamedColor(settings.value("vis_color2", "#BECBFF").toString());
    m_color3.setNamedColor(settings.value("vis_color3", "#BECBFF").toString());
    m_peakColor.setNamedColor(settings.value("vis_peak_color", "#DDDDDD").toString());
    m_cell_size =  QSize(14, 8);
    m_peaks_falloff = settings.value("vis_peaks_falloff", 0.2).toDouble();
    m_analyzer_falloff = settings.value("vis_analyzer_falloff", 2.2).toDouble();
    m_show_peaks = settings.value("vis_show_peaks", true).toBool();
    QString analyzer_type = settings.value("vis_analyzer_type", "cells").toString();
    if(analyzer_type == "lines")
        m_analyzerType = Lines;
    else
        m_analyzerType = Cells;

    settings.endGroup();
}
