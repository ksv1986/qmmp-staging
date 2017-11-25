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

#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <QListView>
#include <QVBoxLayout>
#include <QSettings>
#include <QAction>
#include <QApplication>
#include <QLineEdit>
#include <qmmp/metadatamanager.h>
#include <qmmpui/playlistmanager.h>
#include <qmmpui/filedialog.h>
#include <qmmpui/playlistmanager.h>
#include <qmmp/qmmp.h>
#include "elidinglabel.h"
#include "filesystembrowser.h"

class FileSystemFilterProxyModel : public QSortFilterProxyModel
{
public:
    FileSystemFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent) {}

protected:
    virtual bool filterAcceptsRow(
            int source_row, const QModelIndex &source_parent) const{
        QFileSystemModel *sm = qobject_cast<QFileSystemModel*>(sourceModel());
        if (source_parent == sm->index(sm->rootPath())) {
            return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
        }
        return true;
    }
};

FileSystemBrowser::FileSystemBrowser(QWidget *parent) :
    QWidget(parent)
{
    m_update = false;

    m_listView = new QListView(this);
    m_listView->setFrameStyle(QFrame::NoFrame);
    m_listView->setDragEnabled(true);
    m_listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    connect(m_listView, SIGNAL(activated(QModelIndex)), SLOT(onListViewActivated(QModelIndex)));

    m_label = new Utils::ElidingLabel(this);
    m_label->setContentsMargins(5,5,5,0);
    m_label->setMargin(0);

    m_filterLineEdit = new QLineEdit(this);
    m_filterLineEdit->setContentsMargins(5,5,5,0);
    m_filterLineEdit->setVisible(false);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(m_label);
    layout->addWidget(m_filterLineEdit);
    layout->addWidget(m_listView);
    setLayout(layout);

    m_fileSystemModel = new QFileSystemModel(this);
    m_fileSystemModel->setReadOnly(true);
    m_fileSystemModel->setNameFilterDisables(false);
    m_fileSystemModel->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDot);

    m_proxyModel = new  FileSystemFilterProxyModel(this);
    m_proxyModel->setDynamicSortFilter(true);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxyModel->setSourceModel(m_fileSystemModel);
    m_listView->setModel(m_proxyModel);

    setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction *addToPlaylistAction = new QAction(tr("Add to Playlist"), this);
    addAction(addToPlaylistAction);
    QAction *selectDirAction = new QAction(tr("Change Directory"), this);
    addAction(selectDirAction);
    addAction(m_showFilterAction = new QAction(tr("Quick Search"), this));
    m_showFilterAction->setCheckable(true);

    connect(selectDirAction, SIGNAL(triggered()), SLOT(selectDirectory()));
    connect(addToPlaylistAction, SIGNAL(triggered()), SLOT(addToPlayList()));
    connect(m_showFilterAction, SIGNAL(toggled(bool)), m_filterLineEdit, SLOT(setVisible(bool)));
    connect(m_showFilterAction, SIGNAL(triggered()), m_filterLineEdit, SLOT(clear()));
    connect(m_filterLineEdit, SIGNAL(textChanged(QString)), SLOT(onFilterLineEditTextChanged(QString)));

    readSettings();
}

FileSystemBrowser::~FileSystemBrowser()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Simple");
    settings.setValue("fsbrowser_current_dir", m_fileSystemModel->rootDirectory().canonicalPath());
    settings.setValue("fsbrowser_quick_search", m_showFilterAction->isChecked());
    settings.endGroup();
}

void FileSystemBrowser::readSettings()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Simple");
    if(!m_update)
    {
        m_update = true;
        setCurrentDirectory(settings.value("fsbrowser_current_dir", QDir::homePath()).toString());
        m_showFilterAction->setChecked(settings.value("fsbrowser_quick_search", false).toBool());
    }
    settings.endGroup();
    m_fileSystemModel->setNameFilters(MetaDataManager::instance()->nameFilters());
}

void FileSystemBrowser::onListViewActivated(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    QModelIndex sourceIndex = m_proxyModel->mapToSource(index);

    QString name = m_fileSystemModel->fileName(sourceIndex);

    if(name == "..")
    {
        setCurrentDirectory(m_fileSystemModel->fileInfo(sourceIndex).absoluteFilePath());
    }
    else if(m_fileSystemModel->isDir(sourceIndex))
    {
        QFileInfo info = m_fileSystemModel->fileInfo(sourceIndex);
        if(info.isExecutable() && info.isReadable())
            setCurrentDirectory(m_fileSystemModel->filePath(sourceIndex));
    }
}

void FileSystemBrowser::addToPlayList()
{
    foreach (QModelIndex index, m_listView->selectionModel()->selectedIndexes())
    {
        if(!index.isValid())
            continue;

        QModelIndex sourceIndex = m_proxyModel->mapToSource(index);
        QString name = m_fileSystemModel->fileName(sourceIndex);
        if(name == "..")
            continue;
        PlayListManager::instance()->selectedPlayList()->add(m_fileSystemModel->filePath(sourceIndex));
    }
}

void FileSystemBrowser::selectDirectory()
{
    QString dir = FileDialog::getExistingDirectory(qApp->activeWindow(),
                                                   tr("Select Directory"), m_fileSystemModel->rootDirectory().canonicalPath());
    if(!dir.isEmpty())
        setCurrentDirectory(dir);
}

void FileSystemBrowser::onFilterLineEditTextChanged(const QString &str)
{
    m_proxyModel->setFilterFixedString(str);
}

void FileSystemBrowser::setCurrentDirectory(const QString &path)
{
    if(path.isEmpty())
        return;

    m_filterLineEdit->clear();

    QModelIndex index = m_fileSystemModel->setRootPath(QDir(path).exists() ? path : QDir::homePath());
    if(index.isValid())
    {
        m_listView->setRootIndex(m_proxyModel->mapFromSource(index));
        m_label->setText(QDir(m_fileSystemModel->rootPath()).dirName());
    }
    else
        m_label->clear();
}
