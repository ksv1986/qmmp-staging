/***************************************************************************
 *   Copyright (C) 2017 by Ilya Kotov                                      *
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

#include <QApplication>
#include <QMouseEvent>
#include "progressbaritemdelegate.h"

ProgressBarItemDelegate::ProgressBarItemDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

void ProgressBarItemDelegate::paint(QPainter *painter,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    if (hasProgressBar(index))
    {
        QStyleOptionViewItem opt = option;

        initStyleOption(&opt, index);

        QStyleOptionProgressBar progressBarOption;
        progressBarOption.rect = option.rect;
        progressBarOption.invertedAppearance = false;
        progressBarOption.bottomToTop = false;
        progressBarOption.text = index.data(ProgressBarValueRole).toString();
        progressBarOption.minimum = 0;
        progressBarOption.maximum = index.data(ProgressBarMaxRole).toInt();
        progressBarOption.progress = index.data(ProgressBarValueRole).toInt();
        progressBarOption.textVisible = true;
        progressBarOption.palette = opt.palette;
        qApp->style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);
    }
    else
    {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize ProgressBarItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    if (hasProgressBar(index))
    {
        int buttonHeight = qApp->style()->pixelMetric(QStyle::PM_ExclusiveIndicatorHeight, &option);
        size.setHeight(qMax(size.height(), buttonHeight));
    }
    return size;
}

bool ProgressBarItemDelegate::hasProgressBar(const QModelIndex &index) const
{
    return index.data(ProgressBarRole).toBool();
}
