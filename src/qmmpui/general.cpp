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

#include <QList>
#include <QDir>
#include <QDialog>
#include <qmmp/qmmp.h>
#include "uihelper.h"
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
            if(general)
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

QStringList General::enabledWidgets()
{
    QStringList out;
    for(const GeneralFactory *f : General::enabledFactories())
    {
        for(const WidgetDescription &desc : f->properties().widgets)
            out << QString("%1_%2").arg(f->properties().shortName).arg(desc.id);
    }

    return out;
}

WidgetDescription General::widgetDescription(const QString &id)
{
    for(const GeneralFactory *f : General::enabledFactories())
    {
        for(const WidgetDescription &desc : f->properties().widgets)
        {
            if(id == QString("%1_%2").arg(f->properties().shortName).arg(desc.id))
              return desc;
        }
    }

    return { -1, QString(), Qt::LeftDockWidgetArea, Qt::NoDockWidgetArea };
}

QWidget *General::createWidget(const QString &id, QWidget *parent)
{
    for(GeneralFactory *f : General::enabledFactories())
    {
        for(const WidgetDescription &desc : f->properties().widgets)
        {
            if(id == QString("%1_%2").arg(f->properties().shortName).arg(desc.id))
              return f->createWidget(desc.id, parent);
        }
    }

    return nullptr;
}

QString General::file(const GeneralFactory *factory)
{
    loadPlugins();
    auto it = std::find_if(m_cache->cbegin(), m_cache->cend(),
                           [factory] (QmmpUiPluginCache *item){ return item->shortName() == factory->properties().shortName; } );
    return it == m_cache->cend() ? QString() : (*it)->file();
}

void General::setEnabled(GeneralFactory *factory, bool enable)
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
        if(general)
            m_generals->insert(factory, general);

        for(const WidgetDescription &d : factory->properties().widgets)
            UiHelper::instance()->addWidget(QString("%1_%2").arg(factory->properties().shortName).arg(d.id));
    }
    else
    {
        for(const WidgetDescription &d : factory->properties().widgets)
            UiHelper::instance()->removeWidget(QString("%1_%2").arg(factory->properties().shortName).arg(d.id));

        if(m_generals->value(factory))
            delete m_generals->take(factory);
    }
}

void General::showSettings(GeneralFactory *factory, QWidget *parentWidget)
{
    QDialog *dialog = factory->createConfigDialog(parentWidget);
    if (!dialog)
        return;

    if (m_generals && dialog->exec() == QDialog::Accepted)
    {
        if(m_generals->keys().contains(factory))
            delete m_generals->take(factory);

        QObject *general = factory->create(m_parent);
        if(general)
            m_generals->insert(factory, general);

        for(const WidgetDescription &d : factory->properties().widgets)
            UiHelper::instance()->updateWidget(QString("%1_%2").arg(factory->properties().shortName).arg(d.id));
    }
    dialog->deleteLater();
}

bool General::isEnabled(const GeneralFactory *factory)
{
    loadPlugins();
    return m_enabledNames.contains(factory->properties().shortName);
}
