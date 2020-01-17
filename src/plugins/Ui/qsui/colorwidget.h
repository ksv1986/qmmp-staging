/***************************************************************************
 *   Copyright (C) 2005-2020 by Ilya Kotov                                 *
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
#ifndef COLORWIDGET_H
#define COLORWIDGET_H

#include <QFrame>
#include <QPaintEvent>
#include <QColorDialog>

/**
@author Ilya Kotov
*/
class ColorWidget : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(QColorDialog::ColorDialogOptions options READ options WRITE setOptions)
public:
    ColorWidget(QWidget *parent = nullptr);
    ~ColorWidget();

    QColorDialog::ColorDialogOptions options() const;
    void setOptions(QColorDialog::ColorDialogOptions options);

    const QString colorName() const;

public slots:
    void setColor(QString);

private:
    void mousePressEvent(QMouseEvent *) override;
    QString m_colorName;
    QColorDialog::ColorDialogOptions m_options = QColorDialog::ColorDialogOptions();


};

#endif
