/***************************************************************************
 *   Copyright (C) 2018 by Ilya Kotov                                      *
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
#include <QStorageInfo>
#include <QActionGroup>
#include <QtDebug>
#include <QStyle>
#include <QSettings>
#include <qmmpui/playlistmanager.h>
#include <windows.h>
#include <dbt.h>
#include <qmmpui/uihelper.h>
#include "removablehelper.h"

RemovableHelper::RemovableHelper(QObject *parent): QObject(parent)
{
    qApp->installNativeEventFilter(this);
    m_actions = new QActionGroup(this);
    connect(m_actions,SIGNAL(triggered(QAction *)), SLOT(processAction(QAction *)));
    //load settings
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("rdetect");
    m_detectCDA = settings.value("cda", true).toBool();
    m_detectRemovable = settings.value("removable", true).toBool();
    m_addTracks = false; //do not load tracks on startup
    m_addFiles = false;
    //find existing devices
    updateActions();
    //load remaining settings
    m_addTracks = settings.value("add_tracks", false).toBool();
    m_removeTracks = settings.value("remove_tracks", false).toBool();
    m_addFiles = settings.value("add_files", false).toBool();
    m_removeFiles = settings.value("remove_files", false).toBool();
    settings.endGroup();

}

RemovableHelper::~RemovableHelper()
{
    qApp->removeNativeEventFilter(this);
}

bool RemovableHelper::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(result);

    if(eventType == "windows_generic_MSG")
    {
        MSG *msg = static_cast<MSG *>(message);
        if(msg->message == WM_DEVICECHANGE && msg->wParam == DBT_DEVICEARRIVAL)
        {
            PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)msg->lParam;

            if(lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME)
            {
                updateActions();
                return true;
            }
        }
        else if(msg->message == WM_DEVICECHANGE && msg->wParam == DBT_DEVICEREMOVECOMPLETE)
        {
            PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)msg->lParam;

            if(lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME)
            {
                updateActions();
                return true;
            }
        }
    }
    return false;
}

void RemovableHelper::processAction(QAction *action)
{
    qDebug("RemovableHelper: action triggered: %s", qPrintable(action->data().toString()));
    QString path = action->data().toString();
    PlayListManager::instance()->selectedPlayList()->add(path);
}

void RemovableHelper::updateActions()
{
    QList<QStorageInfo> volumes = QStorageInfo::mountedVolumes();

    foreach(const QStorageInfo &storage, volumes)
    {
        if(!storage.isValid() || !storage.isReady())
            continue;

        QString dev_path;

        qDebug() << storage.fileSystemType();

        if(m_detectRemovable && storage.bytesTotal() < 40000000000LL &&
                (storage.fileSystemType() == "NTFS" ||
                 storage.fileSystemType() == "FAT32" ||
                 storage.fileSystemType() == "CDFS" ||
                 storage.fileSystemType() == "UDF"))
        {
            dev_path = storage.rootPath();
        }
        else
            continue;

        if(!findAction(dev_path))
        {
            QAction *action = new QAction(this);
            QString actionText;
            /*if (device->isAudio())
            {
                actionText = QString(tr("Add CD \"%1\"")).arg(storage.displayName());
            }
            else*/
            {
                actionText = QString(tr("Add Volume \"%1\"")).arg(storage.displayName());
            }

            if(storage.fileSystemType() == "CDFS")
                action->setIcon(qApp->style()->standardIcon(QStyle::SP_DriveCDIcon));
            else if(storage.fileSystemType() == "UDF")
                action->setIcon(qApp->style()->standardIcon(QStyle::SP_DriveDVDIcon));
            else
                action->setIcon(qApp->style()->standardIcon(QStyle::SP_DriveHDIcon));
            qDebug("RemovableHelper: added menu item: \"%s\"", qPrintable(dev_path));

            action->setText(actionText);
            action->setData(dev_path);
            m_actions->addAction(action);
            UiHelper::instance()->addAction(action, UiHelper::ADD_MENU);
            addPath(dev_path);
        }
    }
    // remove action if device is unmounted/removed
    foreach(QAction *action, m_actions->actions())
    {
        bool found = false;

        foreach(const QStorageInfo &storage, volumes)
        {
            if(storage.rootPath() == action->data().toString())
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            qDebug("RemovableHelper: removed menu item: \"%s\"", qPrintable(action->data().toString()));
            m_actions->removeAction(action);
            UiHelper::instance()->removeAction(action);
            removePath(action->data().toString());
            action->deleteLater();
        }
    }
}

QAction *RemovableHelper::findAction(const QString &dev_path)
{
    foreach(QAction *action, m_actions->actions())
    {
        if (action->data().toString() == dev_path)
            return action;
    }
    return nullptr;
}

void RemovableHelper::addPath(const QString &path)
{
    PlayListModel *model = PlayListManager::instance()->selectedPlayList();

    foreach(PlayListItem *item, model->items()) // Is it already exist?
    {
        if(item->isGroup())
            continue;
        if (dynamic_cast<PlayListTrack *>(item)->path().startsWith(path))
            return;
    }

    if (path.startsWith("cdda://") && m_addTracks)
    {
        PlayListManager::instance()->selectedPlayList()->add(path);
        return;
    }
    else if (!path.startsWith("cdda://") && m_addFiles)
        PlayListManager::instance()->selectedPlayList()->add(path);
}

void RemovableHelper::removePath(const QString &path)
{
    if ((path.startsWith("cdda://") && !m_removeTracks) ||
            (!path.startsWith("cdda://") && !m_removeFiles)) //process settings
        return;

    PlayListModel *model = PlayListManager::instance()->selectedPlayList();

    int i = 0;
    while (model->count() > 0 && i < model->count())
    {
        if (model->isTrack(i) && model->track(i)->path().startsWith(path))
            model->removeTrack(i);
        else
            ++i;
    }
}

