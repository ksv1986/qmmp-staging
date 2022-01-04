/***************************************************************************
 *   Copyright (C) 2008-2021 by Ilya Kotov                                 *
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
#ifndef CUEFILE_H
#define CUEFILE_H

#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>
#include <qmmp/qmmp.h>
#include <qmmp/trackinfo.h>
#include <qmmp/cueparser.h>


/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class CueFile : public CueParser
{
public:
    explicit CueFile(const QString &path);
    ~CueFile();

    QString cueFilePath() const;
    QString dataFilePath(int track) const;
    QStringList dataFilePaths() const;

private:
    QStringList splitLine(const QString &line);
    QString getDirtyPath(const QString &cue_path, const QString &path);
    QMap<QString, QString> m_dataFiles; //name, full path
    bool m_dirty;
    QString m_filePath;

};

#endif //CUEFILE_H
