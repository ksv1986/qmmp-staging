/***************************************************************************
 *   Copyright (C) 2017-2019 by Ilya Kotov                                 *
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
#include <QPixmap>
#include <QPainter>
#include <QPaintEvent>
#include <QAction>
#include <QSettings>
#include <QStandardPaths>
#include <QVariant>
#include <QFileInfo>
#include <qmmp/qmmp.h>
#include "filedialog.h"
#include "coverviewer_p.h"

CoverViewer::CoverViewer(QWidget *parent)
        : QWidget(parent)
{
    QAction *saveAsAction = new QAction(tr("&Save As..."), this);
    connect(saveAsAction, SIGNAL(triggered()), SLOT(saveAs()));
    addAction(saveAsAction);
    setContextMenuPolicy(Qt::ActionsContextMenu);
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_lastDir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    m_lastDir = settings.value("CoverEditor/last_dir", m_lastDir).toString();
}

CoverViewer::~CoverViewer()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("CoverEditor/last_dir", m_lastDir);
}

void CoverViewer::setPixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;
    update();
}

bool CoverViewer::hasPixmap() const
{
    return !m_pixmap.isNull();
}

const QPixmap &CoverViewer::pixmap() const
{
    return m_pixmap;
}

void CoverViewer::saveAs()
{
    QString path = FileDialog::getSaveFileName(this, tr("Save Cover As"),
                                               m_lastDir + "/cover.jpg",
                                               tr("Images") +" (*.png *.jpg)");

    if (!path.isEmpty())
    {
        m_lastDir = QFileInfo(path).absoluteDir().path();
        m_pixmap.save(path);
    }
}

void CoverViewer::load()
{
    QString path = FileDialog::getOpenFileName(this, tr("Open Image"),
                                               m_lastDir,
                                               tr("Images") +" (*.png *.jpg)");
    if(!path.isEmpty())
    {
        m_lastDir = QFileInfo(path).absoluteDir().path();
        m_pixmap.load(path);
        if(m_pixmap.width() > 512)
            m_pixmap = m_pixmap.scaled(512, 512, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    update();
}

void CoverViewer::clear()
{
    m_pixmap = QPixmap();
    update();
}

void CoverViewer::paintEvent(QPaintEvent *)
{
    if(!m_pixmap.isNull())
    {
        QPainter paint(this);
        QPixmap pixmap = m_pixmap.scaled(size().width() - 10, size().height() - 10, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        paint.drawPixmap((width() - pixmap.width()) / 2, (height() - pixmap.height()) / 2, pixmap);
    }
}
