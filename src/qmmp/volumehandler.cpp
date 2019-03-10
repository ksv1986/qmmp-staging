/***************************************************************************
 *   Copyright (C) 2008-2016 by Ilya Kotov                                 *
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

#include <QTimer>
#include <QDir>
#include <QSettings>
#include "qmmpsettings.h"
#include "output.h"
#include "softwarevolume_p.h"
#include "volumehandler.h"

VolumeHandler::VolumeHandler(QObject *parent) : QObject(parent)
{
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), SLOT(checkVolume()));
    reload();
}

VolumeHandler::~VolumeHandler()
{
    if(m_volume)
        delete m_volume;
}

void VolumeHandler::setVolume(int left, int right)
{
    VolumeSettings v;
    v.left = qBound(0,left,100);
    v.right = qBound(0,right,100);
    m_volume->setVolume(v);
    checkVolume();
}

void VolumeHandler::changeVolume(int delta)
{
    setVolume(qBound(0, volume() + delta, 100));
}

void VolumeHandler::setVolume(int volume)
{
    volume = qBound(0, volume, 100);
    setVolume(volume-qMax(balance(),0)*volume/100,
              volume+qMin(balance(),0)*volume/100);
}

void VolumeHandler::setBalance(int balance)
{
    balance = qBound(-100, balance, 100);
    setVolume(volume()-qMax(balance,0)*volume()/100,
              volume()+qMin(balance,0)*volume()/100);
}

void VolumeHandler::setMuted(bool muted)
{
    if(m_muted != muted)
    {
        m_volume->setMuted(muted);
        checkVolume();
    }
}

int VolumeHandler::left() const
{
    return m_settings.left;
}

int VolumeHandler::right() const
{
    return m_settings.right;
}

int VolumeHandler::volume() const
{
    return qMax(m_settings.right, m_settings.left);
}

int VolumeHandler::balance() const
{
    int v = volume();
    return v > 0 ? (m_settings.right - m_settings.left) * 100 / v : 0;
}

bool VolumeHandler::isMuted() const
{
    return m_muted;
}

void VolumeHandler::checkVolume()
{
    VolumeSettings v = m_volume->volume();
    bool muted = m_volume->isMuted();

    v.left = qBound(0, v.left, 100);
    v.right = qBound(0, v.right, 100);
    if(m_muted != muted || (m_prev_block && !signalsBlocked ()))
    {
        m_muted = muted;
        emit mutedChanged(m_muted);
    }

    if (m_settings != v) //volume has been changed
    {
        m_settings = v;
        emit volumeChanged(v.left, v.right);
        emit volumeChanged(volume());
        emit balanceChanged(balance());
    }
    else if(m_prev_block && !signalsBlocked ()) //signals have been unblocked
    {
        emit volumeChanged(v.left, v.right);
        emit volumeChanged(volume());
        emit balanceChanged(balance());
    }
    m_prev_block = signalsBlocked ();
}

void VolumeHandler::reload()
{
    m_timer->stop();
    bool restore = false;
    if(m_volume)
    {
        restore = true;
        delete m_volume;
        m_volume = nullptr;
    }

    if(!QmmpSettings::instance()->useSoftVolume() && Output::currentFactory())
        m_volume = Output::currentFactory()->createVolume();

    if(m_volume)
    {
        if(restore)
            m_volume->setMuted(m_muted);

        if(m_volume->flags() & Volume::HasNotifySignal)
        {
            checkVolume();
            connect(m_volume, SIGNAL(changed()), SLOT(checkVolume()));
        }
        else
        {
            m_timer->start(150); // fallback to polling if change notification is not available.
        }
    }
    else
    {
        if(restore)
            m_volume->setMuted(m_muted);

        m_volume = new SoftwareVolume;
        blockSignals(true);
        checkVolume();
        blockSignals(false);
        QTimer::singleShot(125, this, SLOT(checkVolume()));
    }
}
