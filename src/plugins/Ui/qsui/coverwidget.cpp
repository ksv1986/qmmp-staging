/***************************************************************************
 *   Copyright (C) 2013-2022 by Ilya Kotov                                 *
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

#include <QAction>
#include <QDesktopServices>
#include <QFileInfo>
#include <QPainter>
#include <QPaintEvent>
#include <QUrl>
#include <qmmp/qmmp.h>
#include <qmmp/qmmpsettings.h>
#include <qmmpui/filedialog.h>
#include "coverwidget.h"

namespace {
    QPixmap defaultCover()
    {
        QmmpSettings *gs = QmmpSettings::instance();
        switch (gs->defaultCover()) {
        case QmmpSettings::DEFAULTCOVER_NONE:
            return QPixmap();
        case QmmpSettings::DEFAULTCOVER_CUSTOM:
            return QPixmap(gs->customCoverPath());
        case QmmpSettings::DEFAULTCOVER_BUILTIN:
            break;
        }
        return QPixmap(":/qsui/ui_no_cover.png");
    }
}

CoverWidget::CoverWidget(QWidget *parent)
        : QWidget(parent)
{
    setContextMenuPolicy(Qt::ActionsContextMenu);

    QAction *openLocation = new QAction(tr("Open the directory containing this file"), this);
    connect(openLocation, &QAction::triggered, this, &CoverWidget::openCoverLocation);
    addAction(openLocation);

    QAction *saveAsAction = new QAction(tr("&Save As..."), this);
    connect(saveAsAction, &QAction::triggered, this, &CoverWidget::saveAs);
    addAction(saveAsAction);

    m_pixmap = defaultCover();
}

void CoverWidget::setCover(const QPixmap &pixmap, const QString &path)
{
    m_pixmap = pixmap.isNull() ? defaultCover() : pixmap;
    m_path = path;
    update();
}

void CoverWidget::clearCover()
{
    setCover(QPixmap(), QString());
    update();
}

void CoverWidget::paintEvent(QPaintEvent *)
{
    if(!m_pixmap.isNull())
    {
        QPainter paint(this);
        QPixmap pixmap = m_pixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        paint.drawPixmap((width() - pixmap.width()) / 2, (height() - pixmap.height()) / 2, pixmap);
    }
}

void CoverWidget::saveAs()
{
    QString path = FileDialog::getSaveFileName(this, tr("Save Cover As"),
                                                 QDir::homePath() + "/cover.jpg",
                                                 tr("Images") +" (*.png *.jpg)");

    if (!path.isEmpty())
        m_pixmap.save(path);
}

void CoverWidget::openCoverLocation()
{
    if (!m_path.isEmpty())
        QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(m_path).absolutePath()));
}
