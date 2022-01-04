/***************************************************************************
 *   Copyright (C) 2011-2021 by Ilya Kotov                                 *
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

#ifndef QSUISETTINGS_H
#define QSUISETTINGS_H

#include <QWidget>
#include <QFileInfo>
#include "ui_qsuisettings.h"

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class QSUiSettings : public QWidget
{
    Q_OBJECT
public:
    explicit QSUiSettings(QWidget *parent = nullptr);
    virtual ~QSUiSettings();
    void writeSettings();


private slots:
    void on_plFontButton_clicked();
    void on_columnFontButton_clicked();
    void on_tabsFontButton_clicked();
    void on_popupTemplateButton_clicked();
    void on_customizeToolBarButton_clicked();
    void on_resetFontsButton_clicked();
    void on_resetColorsButton_clicked();
    void addWindowTitleString(const QString &str);

private:
    void showEvent(QShowEvent *) override;
    void loadFonts();
    void createActions();
    void readSettings();

    Ui::QSUISettings m_ui;
};

#endif // QSUISETTINGS_H
