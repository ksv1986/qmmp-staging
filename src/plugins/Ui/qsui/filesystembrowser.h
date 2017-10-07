/***************************************************************************
 *   Copyright (C) 2013-2017 by Ilya Kotov                                 *
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

#ifndef FILESYSTEMBROWSER_H
#define FILESYSTEMBROWSER_H

#include <QWidget>

class QFileSystemModel;
class QModelIndex;
class QListView;
class QAction;
class QLineEdit;
class FileSystemFilterProxyModel;

namespace Utils {
   class ElidingLabel;
}


/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class FileSystemBrowser : public QWidget
{
    Q_OBJECT
public:
    explicit FileSystemBrowser(QWidget *parent = 0);
    ~FileSystemBrowser();

    void readSettings();

private slots:
    void onListViewActivated(const QModelIndex &index);
    void addToPlayList();
    void selectDirectory();
    void onFilterLineEditTextChanged(const QString &str);

private:
    void setCurrentDirectory(const QString &path);
    bool m_update;
    Utils::ElidingLabel *m_label;
    QFileSystemModel *m_fileSystemModel;
    QListView *m_listView;
    FileSystemFilterProxyModel  *m_proxyModel;
    QLineEdit *m_filterLineEdit;
    QAction *m_showFilterAction;

};

#endif // FILESYSTEMBROWSER_H
