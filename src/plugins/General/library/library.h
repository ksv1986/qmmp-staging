/***************************************************************************
 *   Copyright (C) 2020 by Ilya Kotov                                      *
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

#ifndef LIBRARY_H
#define LIBRARY_H

#include <QObject>
#include <QElapsedTimer>
#include <QPointer>
#include <qmmp/trackinfo.h>
#include <qmmp/qmmp.h>

#define CONNECTION_NAME "qmmp_library"

class SoundCore;
//class HistoryWindow;

class Library : public QObject
{
    Q_OBJECT
public:
    explicit Library(QObject *parent = nullptr);
    ~Library();

private slots:
    void showLibraryWindow();

private:
    bool createTables();
    //void saveTrack();

    //QPointer<HistoryWindow> m_historyWindow;

};

#endif // LIBRARY_H
