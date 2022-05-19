/***************************************************************************
 *   Copyright (C) 2013-2022 by Ilya Kotov                                 *
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
#include <QByteArray>
#include <QDBusMetaType>
#include "udisksdevice.h"

UDisksDevice::UDisksDevice(QDBusObjectPath o, QObject *parent) : QObject(parent)
{
    m_block_interface = new QDBusInterface("org.freedesktop.UDisks2", o.path(),
                                           "org.freedesktop.UDisks2.Block", QDBusConnection::systemBus(),
                                           this);

    QDBusObjectPath drive_object = property("Drive").value<QDBusObjectPath>();

    QDBusConnection::systemBus().connect("org.freedesktop.UDisks2", o.path(),
                                         "org.freedesktop.DBus.Properties","PropertiesChanged",
                                         this, SIGNAL(changed()));

    m_drive_interface = new QDBusInterface("org.freedesktop.UDisks2", drive_object.path(),
                                           "org.freedesktop.UDisks2.Drive", QDBusConnection::systemBus(),
                                           this);
    m_path = o;
}

UDisksDevice::~UDisksDevice()
{
}

QVariant UDisksDevice::property(const QString &key) const
{
    return m_block_interface->property(key.toLatin1().data());
}

bool UDisksDevice::isRemovable() const
{
    return m_drive_interface->property("Removable").toBool();
}

bool UDisksDevice::isMediaRemovable() const
{
    return m_drive_interface->property("MediaRemovable").toBool();
}

bool UDisksDevice::isAudio() const
{
    return m_drive_interface->property("OpticalNumAudioTracks").toInt() > 0;
}

bool UDisksDevice::isMounted() const
{
    return !mountPoints().isEmpty();
}

bool UDisksDevice::isOptical() const
{
    return m_drive_interface->property("Optical").toBool();
}

QStringList UDisksDevice::mountPoints() const
{
    QStringList points;
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", m_path.path(),
                                                          "org.freedesktop.DBus.Properties", "Get");

    QList<QVariant> arguments;
    arguments << "org.freedesktop.UDisks2.Filesystem" << "MountPoints";
    message.setArguments(arguments);

    QDBusMessage reply = QDBusConnection::systemBus().call(message);

    const QList<QVariant> args = reply.arguments();

    for(const QVariant &arg : qAsConst(args))
    {
        QByteArrayList list;
        QDBusArgument a = arg.value<QDBusVariant>().variant().value<QDBusArgument>();
        if(a.currentType() != QDBusArgument::ArrayType)
            continue;
        a >> list;

        for(const QByteArray &p : qAsConst(list))
            points.append(p);
    }
    return points;
}

QString UDisksDevice::deviceFile() const
{
    return QString::fromLatin1(m_block_interface->property("Device").toByteArray());
}

QDBusObjectPath UDisksDevice::objectPath() const
{
    return m_path;
}
