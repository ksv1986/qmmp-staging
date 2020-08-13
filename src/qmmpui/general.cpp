/***************************************************************************
 *   Copyright (C) 2008-2020 by Ilya Kotov                                 *
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

#include <QList>
#include <QDir>
#include <QDialog>
#include <qmmp/qmmp.h>
#include "qmmpuiplugincache_p.h"
#include "general.h"

QList<QmmpUiPluginCache*> *General::m_cache = nullptr;
QStringList General::m_enabledNames;
QHash <GeneralFactory*, QObject*> *General::m_generals = nullptr;
QObject *General::m_parent = nullptr;

void General::loadPlugins()
{
    if (m_cache)
        return;

    m_cache = new QList<QmmpUiPluginCache*>;
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    for(const QString &filePath : Qmmp::findPlugins("General"))
    {
        QmmpUiPluginCache *item = new QmmpUiPluginCache(filePath, &settings);
        if(item->hasError())
        {
            delete item;
            continue;
        }
        m_cache->append(item);
    }
    m_enabledNames = settings.value("General/enabled_plugins").toStringList();
    QmmpUiPluginCache::cleanup(&settings);
}

void General::create(QObject *parent)
{
    if(m_generals)
        return;
    m_generals = new QHash <GeneralFactory*, QObject*>();
    m_parent = parent;
    loadPlugins();
    for(QmmpUiPluginCache* item : qAsConst(*m_cache))
    {
        if(!m_enabledNames.contains(item->shortName()))
            continue;
        GeneralFactory *factory = item->generalFactory();
        if (factory)
        {
            QObject *general = factory->create(parent);
            m_generals->insert(factory, general);
        }
    }
}

QList<GeneralFactory *> General::factories()
{
    loadPlugins();
    QList<GeneralFactory *> list;
    for(QmmpUiPluginCache *item : qAsConst(*m_cache))
    {
        if(item->generalFactory())
            list.append(item->generalFactory());
    }
    return list;
}

QList<GeneralFactory *> General::enabledFactories()
{
    loadPlugins();
    QList<GeneralFactory *> list;
    for(QmmpUiPluginCache *item : qAsConst(*m_cache))
    {
        if(!m_enabledNames.contains(item->shortName()))
            continue;
        if(item->generalFactory())
            list.append(item->generalFactory());
    }
    return list;
}

QString General::file(const GeneralFactory *factory)
{
    loadPlugins();
    auto it = std::find_if(m_cache->cbegin(), m_cache->cend(),
                           [factory] (QmmpUiPluginCache *item){ return item->shortName() == factory->properties().shortName; } );
    return it == m_cache->cend() ? QString() : (*it)->file();
}

void General::setEnabled(GeneralFactory* factory, bool enable)
{
    loadPlugins();
    if (!factories().contains(factory))
        return;

    if(enable == isEnabled(factory))
        return;

    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);

    if (enable)
        m_enabledNames << factory->properties().shortName;
    else
        m_enabledNames.removeAll(factory->properties().shortName);
    m_enabledNames.removeDuplicates();
    settings.setValue("General/enabled_plugins", m_enabledNames);

    if(!m_generals)
        return;

    if (enable == m_generals->keys().contains(factory))
        return;
    if (enable)
    {
        QObject *general = factory->create(m_parent);
        m_generals->insert(factory, general);
    }
    else
    {
        delete m_generals->value(factory);
        m_generals->remove(factory);
    }
}

void General::showSettings(GeneralFactory* factory, QWidget* parentWidget)
{
    QDialog *dialog = factory->createConfigDialog(parentWidget);
    if (!dialog)
        return;

    if (m_generals && dialog->exec() == QDialog::Accepted && m_generals->keys().contains(factory))
    {
        delete m_generals->value(factory);
        QObject *general = factory->create(m_parent);
        m_generals->insert(factory, general);
    }
    dialog->deleteLater();
}

bool General::isEnabled(const GeneralFactory *factory)
{
    loadPlugins();
    return m_enabledNames.contains(factory->properties().shortName);
}
