/***************************************************************************
 *   Copyright (C) 2018 by Ilya Kotov                                      *
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

#include <QRegExp>
#include "trackinfo.h"

TrackInfo::TrackInfo(const QString &path)
{
    m_path = path;
    m_duration = 0;
    m_parts = NoParts;
    m_metaData.insert(Qmmp::URL, path);
}

TrackInfo::TrackInfo(const TrackInfo &other)
{
    *this = other;
}

TrackInfo::~TrackInfo()
{}

TrackInfo &TrackInfo::operator=(const TrackInfo &info)
{
    setDuration(info.duration());
    setValues(info.metaData());
    setValues(info.properties());
    setValues(info.replayGainInfo());
    setPath(info.path());
    setParts(info.parts());
    return *this;
}

bool TrackInfo::operator==(const TrackInfo &info) const
{
    return m_metaData == info.metaData () &&
            m_properties == info.properties() &&
            m_replayGainInfo == info.replayGainInfo() &&
            m_parts == info.parts();
}

bool TrackInfo::operator!=(const TrackInfo &info) const
{
    return !operator==(info);
}

qint64 TrackInfo::duration () const
{
    return m_duration;
}

bool TrackInfo::isEmpty() const
{
    return m_metaData.isEmpty() && m_properties.isEmpty() && m_replayGainInfo.isEmpty() && m_path.isEmpty();
}

const QString TrackInfo::path() const
{
    return m_path;
}

const QString &TrackInfo::value(Qmmp::MetaData key) const
{
    return m_metaData.find(key).value();
}

const QString &TrackInfo::value(Qmmp::TrackProperty key) const
{
    return m_properties.find(key).value();
}

const double &TrackInfo::value(Qmmp::ReplayGainKey key) const
{
    return m_replayGainInfo.find(key).value();
}

const QMap<Qmmp::MetaData, QString> &TrackInfo::metaData() const
{
    return m_metaData;
}

const QMap<Qmmp::TrackProperty, QString> &TrackInfo::properties() const
{
    return m_properties;
}

const QMap<Qmmp::ReplayGainKey, double> &TrackInfo::replayGainInfo() const
{
    return m_replayGainInfo;
}

TrackInfo::Parts TrackInfo::parts() const
{
    return m_parts;
}

void TrackInfo::setDuration(qint64 duration)
{
    m_duration = duration;
}

void TrackInfo::setValue(Qmmp::MetaData key, const QVariant &value)
{
    QString strValue = value.toString();
    if(strValue.isEmpty() || strValue == "0")
        return;

    m_parts |= MetaData;
    m_metaData[key] = strValue;
}

void TrackInfo::setValue(Qmmp::TrackProperty key, const QVariant &value)
{
    QString strValue = value.toString();
    if(strValue.isEmpty() || strValue == "0")
        return;

    m_parts |= Properties;
    m_properties[key] = strValue;
}

void TrackInfo::setValue(Qmmp::ReplayGainKey key, double value)
{
    if(!qFuzzyIsNull(value))
        return;

    m_parts |= ReplayGainInfo;
    m_replayGainInfo[key] = value;
}

void TrackInfo::setValue(Qmmp::ReplayGainKey key, const QString &value)
{
    QString str = value;
    str.remove(QRegExp("[\\sA-Za-z]"));
    str = str.trimmed();
    bool ok = false;
    double v = str.toDouble(&ok);
    if(ok)
        setValue(key, v);
}

void TrackInfo::setValues(const QMap<Qmmp::MetaData, QString> &metaData)
{
    m_parts |= MetaData;
    m_metaData = metaData;
}

void TrackInfo::setValues(const QMap<Qmmp::TrackProperty, QString> &properties)
{
    m_parts |= Properties;
    m_properties = properties;
}

void TrackInfo::setValues(const QMap<Qmmp::ReplayGainKey, double> &replayGainInfo)
{
    m_parts |= ReplayGainInfo;
    m_replayGainInfo = replayGainInfo;
}

void TrackInfo::updateValues(const QMap<Qmmp::MetaData, QString> &metaData)
{
    foreach (Qmmp::MetaData key, metaData.keys())
        setValue(key, metaData[key]);
}

void TrackInfo::updateValues(const QMap<Qmmp::TrackProperty, QString> &properties)
{
    foreach (Qmmp::TrackProperty key, properties.keys())
        setValue(key, properties[key]);
}

void TrackInfo::updateValues(const QMap<Qmmp::ReplayGainKey, double> &replayGainInfo)
{
    foreach (Qmmp::ReplayGainKey key, replayGainInfo.keys())
        setValue(key, replayGainInfo[key]);
}

void TrackInfo::setPath(const QString &path)
{
    m_path = path;
    m_metaData.insert(Qmmp::URL, path);
}

void TrackInfo::setParts(Parts parts)
{
    m_parts = parts;
}

void TrackInfo::clear(Parts parts)
{
    if(parts & MetaData)
        m_metaData.clear();
    if(parts & Properties)
        m_properties.clear();
    if(parts & ReplayGainInfo)
        m_replayGainInfo.clear();
    m_parts &= ~parts;
}
