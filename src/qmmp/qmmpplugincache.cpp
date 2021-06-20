/***************************************************************************
 *   Copyright (C) 2013-2021 by Ilya Kotov                                 *
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

#include <QStringList>
#include <QDateTime>
#include <QFileInfo>
#include <QPluginLoader>
#include <QApplication>
#include <QTranslator>
#include "decoderfactory.h"
#include "outputfactory.h"
#include "enginefactory.h"
#include "effectfactory.h"
#include "inputsourcefactory.h"
#include "qmmpplugincache_p.h"

QmmpPluginCache::QmmpPluginCache(const QString &file, QSettings *settings)
{
    bool update = false;
    QFileInfo info(file);
    m_path = info.QFileInfo::canonicalFilePath();

    settings->beginGroup("PluginCache");
    QString key = m_path;
#ifndef Q_OS_WIN
    key.remove(0,1);
#endif
    if(settings->allKeys().contains(key))

    {
        QStringList values = settings->value(m_path).toStringList();
        if(values.count() != 5)
            update = true;
        else
        {
            m_shortName = values.at(0);
            m_priority = values.at(1).toInt();
            m_filters = values.at(2).split(";");
            m_contentTypes = values.at(3).split(";");
            update = (info.lastModified().toString(Qt::ISODate) != values.at(4));
        }
    }
    else
        update = true;


    if(update)
    {
        if(DecoderFactory *factory = decoderFactory())
        {
            m_shortName = factory->properties().shortName;
            m_priority = factory->properties().priority;
            m_filters = factory->properties().filters;
            m_contentTypes = factory->properties().contentTypes;
        }
        else if(OutputFactory *factory = outputFactory())
        {
            m_shortName = factory->properties().shortName;
            m_priority = 0;
        }
        else if(EngineFactory *factory = engineFactory())
        {
            m_shortName = factory->properties().shortName;
            m_priority = 0;
            m_filters = factory->properties().filters;
            m_contentTypes = factory->properties().contentTypes;
        }
        else if(EffectFactory *factory = effectFactory())
        {
            m_shortName = factory->properties().shortName;
            m_priority = factory->properties().priority;
        }
        else if(InputSourceFactory *factory = inputSourceFactory())
        {
            m_shortName = factory->properties().shortName;
            m_priority = 0;
        }
        else
        {
            qWarning("QmmpPluginCache: unknown plugin type: %s", qPrintable(m_path));
            m_error = true;
        }

        if (!m_error)
        {
            QStringList values;
            values << m_shortName;
            values << QString::number(m_priority);
            values << m_filters.join(";");
            values << m_contentTypes.join(";");
            values << info.lastModified().toString(Qt::ISODate);
            settings->setValue(m_path, values);
            qDebug("QmmpPluginCache: added cache item \"%s=%s\"",
                   qPrintable(info.fileName()), qPrintable(values.join(",")));
        }
    }
    settings->endGroup();
}

const QString QmmpPluginCache::shortName() const
{
    return m_shortName;
}

const QString QmmpPluginCache::file() const
{
    return m_path;
}

const QStringList &QmmpPluginCache::filters() const
{
    return m_filters;
}

const QStringList &QmmpPluginCache::contentTypes() const
{
    return m_contentTypes;
}

int QmmpPluginCache::priority() const
{
    return m_priority;
}

DecoderFactory *QmmpPluginCache::decoderFactory()
{
    if(!m_decoderFactory)
    {
        m_decoderFactory = qobject_cast<DecoderFactory *> (instance());
        if(m_decoderFactory)
            loadTranslation(m_decoderFactory->translation());
    }
    return m_decoderFactory;
}

OutputFactory *QmmpPluginCache::outputFactory()
{
    if(!m_outputFactory)
    {
        m_outputFactory = qobject_cast<OutputFactory *> (instance());
        if(m_outputFactory)
            loadTranslation(m_outputFactory->translation());
    }
    return m_outputFactory;
}

EngineFactory *QmmpPluginCache::engineFactory()
{
    if(!m_engineFactory)
    {
        m_engineFactory = qobject_cast<EngineFactory *> (instance());
        if(m_engineFactory)
            loadTranslation(m_engineFactory->translation());
    }
    return m_engineFactory;
}

EffectFactory *QmmpPluginCache::effectFactory()
{
    if(!m_effectFactory)
    {
        m_effectFactory = qobject_cast<EffectFactory *> (instance());
        if(m_effectFactory)
            loadTranslation(m_effectFactory->translation());
    }
    return m_effectFactory;
}

InputSourceFactory *QmmpPluginCache::inputSourceFactory()
{
    if(!m_inputSourceFactory)
    {
        m_inputSourceFactory = qobject_cast<InputSourceFactory *> (instance());
        if(m_inputSourceFactory)
            loadTranslation(m_inputSourceFactory->translation());
    }
    return m_inputSourceFactory;
}

bool QmmpPluginCache::hasError() const
{
    return m_error;
}

QObject *QmmpPluginCache::instance()
{
    if(m_error)
        return nullptr;
    if(m_instance)
        return m_instance;
    QPluginLoader loader(m_path);
    m_instance = loader.instance();
    if (loader.isLoaded())
        qDebug("QmmpPluginCache: loaded plugin %s", qPrintable(QFileInfo(m_path).fileName()));
    else
    {
        m_error = true;
        qWarning("QmmpPluginCache: error: %s", qPrintable(loader.errorString ()));
    }
    return m_instance;
}

void QmmpPluginCache::loadTranslation(const QString &translation)
{
    if(!translation.isEmpty())
    {
        QTranslator *translator = new QTranslator(qApp);
        if(translator->load(translation + Qmmp::systemLanguageID()))
            qApp->installTranslator(translator);
        else
            delete translator;
    }
}

void QmmpPluginCache::cleanup(QSettings *settings)
{
    settings->beginGroup("PluginCache");

    for(const QString &key : settings->allKeys())
    {
#ifdef Q_OS_WIN
        if(!QFile::exists(key))
#else
        if(!QFile::exists("/" + key))
#endif
        {
            settings->remove(key);
            qDebug("QmmpPluginCache: removed key %s", qPrintable(key));
        }
    }
    settings->endGroup();
}
