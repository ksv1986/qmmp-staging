/***************************************************************************
 *   Copyright (C) 2020-2021 by Ilya Kotov                                 *
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
#include <QStringList>
#include <QThread>
#include <atomic>
#include <qmmp/trackinfo.h>
#include <qmmp/qmmp.h>

class QFileInfo;
class QAction;
class SoundCore;
class PlayListTrack;
class LibraryWidget;

class Library : public QThread
{
    Q_OBJECT
public:
    explicit Library(QPointer<LibraryWidget> *libraryWidget, QObject *parent = nullptr);
    ~Library();

    QAction *showAction() const;

private slots:
    void showLibraryWindow();

private slots:
    void startDirectoryScanning();

private:
    void run() override;
    bool createTables();
    void addTrack(TrackInfo *track, const QString &filePath);
    QByteArray serializeAudioInfo(const QMap<Qmmp::TrackProperty, QString> &properties);
    bool scanDirectories(const QStringList &paths);
    void addDirectory(const QString &s);
    void removeMissingFiles(const QStringList &paths);
    bool checkFile(const QFileInfo &info);
    void removeIgnoredTracks(QList<TrackInfo *> *tracks, const QStringList &ignoredPaths);

    QStringList m_filters, m_dirs;
    std::atomic_bool m_stopped = ATOMIC_VAR_INIT(false);
    QPointer<LibraryWidget> *m_libraryWidget;
    QAction *m_showAction;
};

#endif // LIBRARY_H
