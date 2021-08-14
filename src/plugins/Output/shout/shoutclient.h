/***************************************************************************
 *   Copyright (C) 2017 by Ilya Kotov                                      *
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

#ifndef SHOUTCLIENT_H
#define SHOUTCLIENT_H

#include <QObject>
#include <shout/shout.h>

class QTimer;

class ShoutClient : public QObject
{
    Q_OBJECT
public:
    explicit ShoutClient(QObject *parent);
    ~ShoutClient();

    void readSettings();
    bool open();
    bool send(const unsigned char *data, int len);
    qint64 latency() const;
    void closeLater();

public slots:
    void close();

private:
    shout_t *m_shout_conn;
    QTimer *m_timer;

};

#endif // SHOUTCLIENT_H
