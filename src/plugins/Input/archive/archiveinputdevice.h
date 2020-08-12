/***************************************************************************
 *   Copyright (C) 2016-2020 by Ilya Kotov                                 *
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

#ifndef ARCHIVEINPUTDEVICE_H
#define ARCHIVEINPUTDEVICE_H

#include <QIODevice>
#include <QBuffer>
#include <archive.h>
#include <archive_entry.h>

class ArchiveInputDevice : public QIODevice
{
    Q_OBJECT
public:
    explicit ArchiveInputDevice(const QString &url, QObject *parent = nullptr);
    explicit ArchiveInputDevice(struct archive *a, struct archive_entry *e, QObject *parent = nullptr);
    virtual ~ArchiveInputDevice();

    bool seek(qint64 pos) override;
    qint64 size() const override;

protected:
    virtual qint64 readData(char *data, qint64 maxSize) override;
    virtual qint64 writeData(const char *, qint64) override;

private:
    struct archive *m_archive = nullptr;
    struct archive_entry *m_entry = nullptr;
    QBuffer m_buffer;
    bool m_close_libarchive = true;
};

#endif // ARCHIVEINPUTDEVICE_H
