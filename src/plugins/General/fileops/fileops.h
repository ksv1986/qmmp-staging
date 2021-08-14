/***************************************************************************
 *   Copyright (C) 2009-2021 by Ilya Kotov                                 *
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
#ifndef FILEOPS_H
#define FILEOPS_H

#include <qmmpui/general.h>
#include <qmmp/qmmp.h>

class QAction;
class SoundCore;
class PlayListTrack;
class MetaDataFormatter;
class PlayListModel;

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/

class FileOps : public QObject
{
    Q_OBJECT
public:
    FileOps(QObject *parent = nullptr);

    ~FileOps();

    enum ActionType
    {
        COPY = 0,
        RENAME,
        REMOVE,
        MOVE,
        EXECUTE
    };

private slots:
    void execAction();

private:
    void copy(const QList<PlayListTrack*> &tracks, const QString &dest, const MetaDataFormatter *formatter);
    void rename(const QList<PlayListTrack *> &tracks, const MetaDataFormatter *formatter, PlayListModel *model);
    void move(const QList<PlayListTrack*> &tracks, const QString &dest, const MetaDataFormatter *formatter, PlayListModel *model);
    void execute(const QList<PlayListTrack*> &tracks, const MetaDataFormatter *formatter, PlayListModel *model);
    bool isValid(PlayListTrack *track) const;
};

#endif
