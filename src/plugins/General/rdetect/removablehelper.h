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
#ifndef REMOVABLEHELPER_H
#define REMOVABLEHELPER_H

#include <QObject>
#include <QAbstractNativeEventFilter>

class QActionGroup;
class QAction;

class RemovableHelper : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit RemovableHelper(QObject *parent = nullptr);
    ~RemovableHelper();

    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result);

private slots:
    void processAction(QAction *action);

private:
    void updateActions();
    QAction *findAction(const QString &dev_path);
    void addPath(const QString &path);
    void removePath(const QString &path);
    bool isAudioCd(const QString &path);

    QActionGroup *m_actions;
    bool m_detectCDA;
    bool m_detectRemovable;
    bool m_addTracks, m_removeTracks;
    bool m_addFiles, m_removeFiles;

};

#endif // REMOVABLEHELPER_H
