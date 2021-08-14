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
#ifndef ARCHIVEREADER_H
#define ARCHIVEREADER_H

#include <QObject>
#include <QByteArray>

class QProcess;

/**
	@author Ilya Kotov <forkotov02@ya.ru>
*/
class ArchiveReader : public QObject
{
Q_OBJECT
public:
    explicit ArchiveReader(QObject *parent = nullptr);

    ~ArchiveReader();

    bool isSupported(const QString &path);
    QByteArray unpack(const QString &path);

private:
    QProcess *m_process;
    QByteArray unzip(const QString &path);
    QByteArray gunzip(const QString &path);
    QByteArray bunzip2(const QString &path);

};

#endif
