/***************************************************************************
 *   Copyright (C) 2009 by Artur Guzik                                     *
 *   a.guzik88@gmail.com                                                   *
 *                                                                         *
 *   Copyright (C) 2009-2015 by Ilya Kotov                                 *
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

#include <QtGlobal>
#ifdef Q_WS_X11

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QPixmap>
#include <QFileInfo>
#include <QApplication>
#include <QDesktopWidget>
#include <QSpacerItem>
#include <QProgressBar>
#include <QSettings>
#include <qmmp/soundcore.h>
#include <qmmp/metadatamanager.h>
#include <qmmpui/metadataformatter.h>
#include "coverwidget.h"
#include "statusiconpopupwidget.h"

StatusIconPopupWidget::StatusIconPopupWidget(QWidget * parent)
        : QFrame(parent)
{
    setWindowFlags(Qt::X11BypassWindowManagerHint |
                   Qt::WindowStaysOnTopHint | Qt::ToolTip | Qt::FramelessWindowHint);
    setFrameStyle(QFrame::NoFrame | QFrame::Plain);
    setAttribute(Qt::WA_QuitOnClose, false);

    m_hLayout = new QHBoxLayout();
    m_vLayout = new QVBoxLayout();

    m_cover = new CoverWidget(this);
    m_hLayout->addWidget(m_cover);

    m_textLabel = new QLabel(this);
    m_vLayout->addWidget(m_textLabel);

    m_spacer = new QSpacerItem(20,0,QSizePolicy::Expanding,QSizePolicy::Expanding);
    m_vLayout->addItem(m_spacer);

    m_bar = new TimeBar(this);
    m_vLayout->addWidget(m_bar);

    m_hLayout->addLayout(m_vLayout);
    setLayout(m_hLayout);

    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);

    m_bar->setMinimumWidth(110);

    connect(m_timer,SIGNAL(timeout()),SLOT(deleteLater()));
    connect(SoundCore::instance(),SIGNAL(elapsedChanged(qint64)),this,SLOT(updateTime(qint64)));

    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Tray");
    m_timer->setInterval(settings.value("tooltip_delay",2000).toInt());
    setWindowOpacity(1.0 - settings.value("tooltip_transparency",0).toInt()/100.0);
    int size = settings.value("tooltip_cover_size",100).toInt();
    m_cover->setFixedSize(size,size);
    m_showProgress = settings.value("tooltip_progress",true).toBool();
    settings.endGroup();
}

StatusIconPopupWidget::~StatusIconPopupWidget()
{
}

void StatusIconPopupWidget::mousePressEvent(QMouseEvent *)
{
    deleteLater();
}

void StatusIconPopupWidget::updateMetaData(const QString &message)
{
    m_timer->stop();
    SoundCore *core = SoundCore::instance();
    m_textLabel->setText(message);
    if(core->state() == Qmmp::Playing || core->state() == Qmmp::Paused)
    {
        QPixmap cover = MetaDataManager::instance()->getCover(core->metaData(Qmmp::URL));
        m_cover->show();
        m_bar->show();
        if(cover.isNull())
            m_cover->setPixmap(QPixmap(":/empty_cover.png"));
        else
            m_cover->setPixmap(cover);
        updateTime(core->elapsed());
        m_bar->setVisible(m_showProgress);
    }
    else
    {
        m_cover->hide();
        m_bar->hide();
    }
    m_textLabel->setText(message);
    qApp->processEvents();
    resize(sizeHint());
    qApp->processEvents();
    if(isVisible())
        updatePosition(m_lastTrayX,m_lastTrayY);
    m_timer->start();
}

void StatusIconPopupWidget::updateTime(qint64 elapsed)
{
    m_bar->setMaximum(SoundCore::instance()->totalTime()/1000);
    m_bar->setValue(elapsed/1000);
    m_bar->update();
}

void StatusIconPopupWidget::updatePosition(int trayx, int trayy)
{
    QRect screenGeometry = QApplication::desktop()->availableGeometry();
    int xpos = 0;
    int ypos = 0;

    xpos = screenGeometry.x() + trayx -5;
    if(xpos + width() > screenGeometry.width())
        xpos = screenGeometry.width() - width() -5;

    if(trayy < screenGeometry.y()) //tray on top of screen
    {
        ypos = screenGeometry.y() + 5;
    }

    if(trayy > screenGeometry.y()) //tray on bottom
    {
        ypos = screenGeometry.y() + screenGeometry.height() - height() -5;
    }
    move(xpos,ypos);
    return;
}

void StatusIconPopupWidget::showInfo(int x, int y, const QString &message)
{
    m_timer->stop();
    m_lastTrayX = x;
    m_lastTrayY = y;
    updateMetaData(message);
    qApp->processEvents();
    updatePosition(x,y);
    qApp->processEvents();
    show();
    m_timer->start();
}

TimeBar::TimeBar(QWidget *parent) : QProgressBar(parent)
{}

QString TimeBar::text() const
{
    return QString("%1:%2").arg(value()/60,2,10,QChar('0')).arg(value()%60,2,10,QChar('0'));
}

#endif
