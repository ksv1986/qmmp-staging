/***************************************************************************
 *   Copyright (C) 2011-2020 by Ilya Kotov                                 *
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

#include <QDir>
#include <qmmp/qmmp.h>
#include "qmmpuiplugincache_p.h"
#include "uiloader.h"

#ifndef QMMP_DEFAULT_UI
#define QMMP_DEFAULT_UI "skinned"
#endif

QList<QmmpUiPluginCache*> *UiLoader::m_cache = nullptr;

void UiLoader::loadPlugins()
{
    if (m_cache)
        return;

    m_cache = new QList<QmmpUiPluginCache*>;
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    for(const QString &filePath : Qmmp::findPlugins("Ui"))
    {
        QmmpUiPluginCache *item = new QmmpUiPluginCache(filePath, &settings);
        if(item->hasError())
        {
            delete item;
            continue;
        }
        m_cache->append(item);
    }
}

QList<UiFactory *> UiLoader::factories()
{
    loadPlugins();
    QList<UiFactory *> list;
    for(QmmpUiPluginCache *item : qAsConst(*m_cache))
    {
        if(item->uiFactory())
            list.append(item->uiFactory());
    }
    return list;
}

QStringList UiLoader::names()
{
    QStringList out;
    loadPlugins();
    for(const QmmpUiPluginCache *item : qAsConst(*m_cache))
    {
        out << item->shortName();
    }
    return out;
}

QString UiLoader::file(UiFactory *factory)
{
    loadPlugins();
    for(const QmmpUiPluginCache *item : qAsConst(*m_cache))
    {
        if(item->shortName() == factory->properties().shortName)
            return item->file();
    }
    return QString();
}

void UiLoader::select(UiFactory* factory)
{
    select(factory->properties().shortName);
}

void UiLoader::select(const QString &name)
{
    loadPlugins();
    for(const QmmpUiPluginCache *item : qAsConst(*m_cache))
    {
        if(item->shortName() == name)
        {
            QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
            settings.setValue ("Ui/current_plugin", name);
            break;
        }
    }
}

UiFactory *UiLoader::selected()
{
    loadPlugins();
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
#ifdef Q_OS_UNIX
    QString defaultUi = QMMP_DEFAULT_UI;
    if(defaultUi == QLatin1String("skinned") && qgetenv("XDG_SESSION_TYPE") == "wayland")
        defaultUi = "qsui";
    QString name = settings.value("Ui/current_plugin", defaultUi).toString();
#else
    QString name = settings.value("Ui/current_plugin", QMMP_DEFAULT_UI).toString();
#endif
    for(QmmpUiPluginCache *item : qAsConst(*m_cache))
    {
        if(item->shortName() == name && item->uiFactory())
            return item->uiFactory();
    }
    if (!m_cache->isEmpty())
        return m_cache->at(0)->uiFactory();
    return nullptr;
}
