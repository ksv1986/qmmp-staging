/***************************************************************************
 *   Copyright (C) 2017 by Ilya Kotov                                      *
 *   forkotov02@hotmail.ru                                                 *
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

#ifndef QSUIQUICKSEARCH_H
#define QSUIQUICKSEARCH_H

#include <QWidget>
#include <QList>

class QLineEdit;
class PlayListManager;

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class QSUIQuickSearch : public QWidget
{
    Q_OBJECT
public:
    explicit QSUIQuickSearch(QWidget *parent = 0);

private slots:
    void onFocusChanged(QWidget *old, QWidget *now);
    void onTextEdited(const QString &str);
    void onSearchUpClicked();
    void onSearchDownClicked();

private:
    void search(const QString &str);
    void updateSelection();
    QLineEdit *m_lineEdit;
    PlayListManager *m_manager;
    QList<int> m_indexes;

};

#endif // QSUIQUICKSEARCH_H
