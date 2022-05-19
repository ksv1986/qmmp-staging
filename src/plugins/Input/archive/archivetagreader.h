/***************************************************************************
 *   Copyright (C) 2016 by Ilya Kotov                                      *
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

#ifndef ARCHIVETAGREADER_H
#define ARCHIVETAGREADER_H

#include <QIODevice>
#include <QMap>
#include <qmmp/qmmp.h>
#include <taglib/fileref.h>
#include <taglib/audioproperties.h>

class IODeviceStream;

class ArchiveTagReader
{
public:
    ArchiveTagReader(QIODevice *input, const QString &url);

    ~ArchiveTagReader();

    const QMap<Qmmp::MetaData, QString>  metaData() const;
    TagLib::AudioProperties *audioProperties() const;

private:
    IODeviceStream *m_stream;
    TagLib::FileRef *m_file;
};

#endif // ARCHIVETAGREADER_H
