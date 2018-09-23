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

#ifndef HISTORYWINDOW_H
#define HISTORYWINDOW_H

#include <QWidget>
#include <QSqlDatabase>
#include <QListWidgetItem>
#include <QMap>
#include <qmmpui/metadataformatter.h>

namespace Ui {
class HistoryWindow;
}

class HistoryWindow : public QWidget
{
    Q_OBJECT
public:
    explicit HistoryWindow(QSqlDatabase db, QWidget *parent = 0);
    ~HistoryWindow();

private slots:
    void on_executeButton_clicked();
    void on_lastWeekButton_clicked();
    void on_lastMonthButton_clicked();
    void on_fromButton_clicked();
    void on_toButton_clicked();

private:
    void loadHistory();
    void loadDistribution();
    void loadTopSongs();
    void loadTopArtists();
    void loadTopGenres();
    void readSettings();
    void closeEvent(QCloseEvent *);

    Ui::HistoryWindow *m_ui;
    QSqlDatabase m_db;
    MetaDataFormatter m_formatter;
};

#endif // HISTORYWINDOW_H
