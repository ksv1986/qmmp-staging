/***************************************************************************
 *   Copyright (C) 2019-2021 by Ilya Kotov                                 *
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

#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <qmmp/soundcore.h>
#include "sleepinhibitor.h"

SleepInhibitor::SleepInhibitor(QObject *parent) : QObject(parent)
{
    m_interface = new QDBusInterface(QLatin1String("org.freedesktop.login1"),
                                     QLatin1String("/org/freedesktop/login1"),
                                     QLatin1String("org.freedesktop.login1.Manager"),
                                     QDBusConnection::systemBus(), this);
    connect(SoundCore::instance(), SIGNAL(stateChanged(Qmmp::State)), SLOT(onStateChanged(Qmmp::State)));
}

SleepInhibitor::~SleepInhibitor()
{}

void SleepInhibitor::onStateChanged(Qmmp::State state)
{
    if(state == Qmmp::Playing)
        inhibit();
    else if(state != Qmmp::Buffering)
        uninhibit();
}

void SleepInhibitor::inhibit()
{
    if(m_lock)
        return;

    static const QString method = QStringLiteral("Inhibit");
    static const QString what = QStringLiteral("sleep:idle");
    static const QString who  = QStringLiteral("qmmp");
    static const QString why  = QStringLiteral("Playing audio");
    static const QString mode = QStringLiteral("block");
    QDBusReply<QDBusUnixFileDescriptor> fd = m_interface->call(method, what, who, why, mode);
    m_lock.reset(new QDBusUnixFileDescriptor(fd));
}

void SleepInhibitor::uninhibit()
{
    m_lock.reset(nullptr);
}
