/***************************************************************************
 *   Copyright (C) 2009-2021 by Ilya Kotov                                 *
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

#include <QFile>
#include <QFileInfo>
#include <QBuffer>
#include <QMutexLocker>
#include <QCoreApplication>
#include <QImageReader>
#include "decoder.h"
#include "decoderfactory.h"
#include "abstractengine.h"
#include "inputsource.h"
#include "qmmpsettings.h"
#include "metadatamanager.h"

#include <memory>

#define COVER_CACHE_SIZE 100        // number of cached cover paths
#define COVER_CACHE_LIMIT 10        // number of cached cover pixmaps

MetaDataManager* MetaDataManager::m_instance = nullptr;

MetaDataManager::MetaDataManager()
    : m_cover_cache(COVER_CACHE_SIZE, COVER_CACHE_LIMIT, 1024)
{
    m_settings = QmmpSettings::instance();
}

MetaDataManager::~MetaDataManager()
{
    clearCoverCache();
}

QList<TrackInfo *> MetaDataManager::createPlayList(const QString &path, TrackInfo::Parts parts, QStringList *ignoredPaths) const
{
    QList <TrackInfo *> list;
    DecoderFactory *fact = nullptr;
    EngineFactory *efact = nullptr;
    QStringList dummyList;
    if(!ignoredPaths)
        ignoredPaths = &dummyList;

    if (!path.contains("://")) //local file
    {
        if(!QFile::exists(path))
            return list;

        if(!(fact = Decoder::findByFilePath(path, m_settings->determineFileTypeByContent())))
            efact = AbstractEngine::findByFilePath(path);
    }
    else
    {
        QString scheme = path.section("://",0,0);
        if(InputSource::findByUrl(path))
        {
            list << new TrackInfo(path);
        }
        else
        {
            for(DecoderFactory *f : Decoder::factories())
            {
                if(f->properties().protocols.contains(scheme) && Decoder::isEnabled(f))
                {
                    fact = f;
                    break;
                }
            }
        }
    }

    if(fact)
        list = fact->createPlayList(path, parts, ignoredPaths);
    else if(efact)
        list = efact->createPlayList(path, parts, ignoredPaths);

    for(TrackInfo *info : qAsConst(list))
    {
        if(info->value(Qmmp::DECODER).isEmpty() && (fact || efact))
            info->setValue(Qmmp::DECODER, fact ? fact->properties().shortName : efact->properties().shortName);
        if(info->value(Qmmp::FILE_SIZE).isEmpty() && !path.contains("://"))
            info->setValue(Qmmp::FILE_SIZE, QFileInfo(path).size());
    }
    return list;
}

MetaDataModel* MetaDataManager::createMetaDataModel(const QString &path, bool readOnly) const
{
    DecoderFactory *fact = nullptr;
    EngineFactory *efact = nullptr;

    if (!path.contains("://")) //local file
    {
        if(!QFile::exists(path))
            return nullptr;
        else if((fact = Decoder::findByFilePath(path, m_settings->determineFileTypeByContent())))
            return fact->createMetaDataModel(path, readOnly);
        else if((efact = AbstractEngine::findByFilePath(path)))
            return efact->createMetaDataModel(path, readOnly);
        return nullptr;
    }
    else
    {
        QString scheme = path.section("://",0,0);
        MetaDataModel *model = nullptr;
        if((fact = Decoder::findByProtocol(scheme)))
        {
            return fact->createMetaDataModel(path, readOnly);
        }
        for(EngineFactory *efact : AbstractEngine::enabledFactories())
        {
            if(efact->properties().protocols.contains(scheme))
                model = efact->createMetaDataModel(path, readOnly);
            if(model)
                return model;
        }
    }
    return nullptr;
}

QStringList MetaDataManager::filters() const
{
    QStringList filters;
    for(const DecoderFactory *fact : Decoder::enabledFactories())
    {
        if (!fact->properties().filters.isEmpty())
            filters << fact->properties().description + " (" + fact->properties().filters.join(" ") + ")";
    }
    for(const EngineFactory *fact : AbstractEngine::enabledFactories())
    {
        if (!fact->properties().filters.isEmpty())
            filters << fact->properties().description + " (" + fact->properties().filters.join(" ") + ")";
    }
    return filters;
}

QStringList MetaDataManager::nameFilters() const
{
    QStringList filters = Decoder::nameFilters();
    filters << AbstractEngine::nameFilters();
    if(m_settings->determineFileTypeByContent())
        filters << "*";
    filters.removeDuplicates();
    return filters;
}

QStringList MetaDataManager::protocols() const
{
    QStringList p;
    p << InputSource::protocols();
    p << Decoder::protocols();
    p << AbstractEngine::protocols();
    p.removeDuplicates();
    return p;
}

QList<QRegularExpression> MetaDataManager::regExps() const
{
    return InputSource::regExps();
}

bool MetaDataManager::supports(const QString &fileName) const
{
    if (!fileName.contains("://")) //local file
    {
        if (!QFile::exists(fileName))
            return false;
        if(Decoder::findByFilePath(fileName))
            return true;
        else if(AbstractEngine::findByFilePath(fileName))
            return true;
        return false;
    }
    return false;
}

QPixmap MetaDataManager::getCover(const QString &url) const
{
    return m_cover_cache.getCover(url);
}

QString MetaDataManager::getCoverPath(const QString &url) const
{
    return m_cover_cache.getCoverPath(url);
}

QString MetaDataManager::findCoverFile(const QString &fileName) const
{
    if(!m_settings->useCoverFiles())
        return QString();

    if(!QFile::exists(fileName))
    {
        return QString();
    }

    QFileInfoList l = findCoverFiles(QFileInfo(fileName).absoluteDir(), m_settings->coverSearchDepth());
    return l.isEmpty() ? QString() : l.at(0).filePath();
}

QFileInfoList MetaDataManager::findCoverFiles(QDir dir, int depth) const
{
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Name);
    QFileInfoList file_list = dir.entryInfoList(m_settings->coverNameFilters());

    const auto fileListCopy = file_list; //avoid container modification
    for(const QFileInfo &i : qAsConst(fileListCopy))
    {
        if(QDir::match(m_settings->coverNameFilters(false), i.fileName()))
            file_list.removeAll(i);

        if(QImageReader::imageFormat(i.filePath()).isEmpty()) //remove unsupported image formats
            file_list.removeAll(i);
    }
    if(!depth || !file_list.isEmpty())
        return file_list;
    depth--;
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Name);
    const QFileInfoList dir_info_list = dir.entryInfoList();
    for(const QFileInfo &i : qAsConst(dir_info_list))
    {
        file_list << findCoverFiles(QDir(i.absoluteFilePath()), depth);
    }
    return file_list;
}

void MetaDataManager::clearCoverCache()
{
    m_cover_cache.clear();
}

void MetaDataManager::prepareForAnotherThread()
{
    //this hack should load all required plugins
    InputSource::enabledFactories();
    Decoder::enabledFactories();
    AbstractEngine::enabledFactories();
}

bool MetaDataManager::hasMatch(const QList<QRegularExpression> &regExps, const QString &path)
{
    for(const QRegularExpression &re : qAsConst(regExps))
    {
        if(re.match(path).hasMatch())
            return true;
    }
    return false;
}

MetaDataManager *MetaDataManager::instance()
{
    if(!m_instance)
    {
        m_instance = new MetaDataManager();
        qAddPostRoutine(&MetaDataManager::destroy);
    }
    return m_instance;
}

void MetaDataManager::destroy()
{
    if(m_instance)
        delete m_instance;
    m_instance = nullptr;
}

MetaDataManager::CoverCache::CoverCache(int size, int limit, int maxCoverSize)
    : m_maxCoverSize{maxCoverSize}
    , m_lookup(size)
    , m_cache(limit)
{}

MetaDataManager::CoverCache::Cover* MetaDataManager::CoverCache::createCover(const QString &url)
{
    auto manager = MetaDataManager::instance();
    auto cover = std::make_unique<Cover>();
    auto& pixmap = cover->pixmap;
    auto& path = cover->path;

    if (!url.contains("://") && QmmpSettings::instance()->useCoverFiles())
        path = manager->findCoverFile(url);

    if (path.isEmpty())
    {
        auto model = std::unique_ptr<MetaDataModel>(manager->createMetaDataModel(url, true));
        if (model)
        {
            path = model->coverPath();
            pixmap = model->cover();
        }
    }

    if (pixmap.isNull() && !path.isEmpty())
        pixmap = QPixmap(path);

    if (pixmap.width() > m_maxCoverSize || pixmap.height() > m_maxCoverSize)
        pixmap = pixmap.scaled(m_maxCoverSize, m_maxCoverSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    return cover.release();
}

MetaDataManager::CoverCache::Cover* MetaDataManager::CoverCache::findCover(const QString &url)
{
    const int hash = qHash(url);
    if (auto key = m_lookup.object(hash); key) {
        if (auto cover = m_cache.object(*key); cover)
            return cover;
    }

    auto cover = createCover(url);
    auto key = new qint64(cover->pixmap.cacheKey());
    if (m_cache.insert(*key, cover)) {
        m_lookup.insert(hash, key);
        return cover;
    }

    return nullptr;
}

QPixmap MetaDataManager::CoverCache::getCover(const QString &url)
{
    QMutexLocker locker(&m_mutex);
    auto cover = findCover(url);
    return cover ? cover->pixmap : QPixmap();
}

QString MetaDataManager::CoverCache::getCoverPath(const QString &url)
{
    QMutexLocker locker(&m_mutex);
    auto cover = findCover(url);
    return cover ? cover->path : QString();
}

void MetaDataManager::CoverCache::clear()
{
    QMutexLocker locker(&m_mutex);
    m_lookup.clear();
    m_cache.clear();
}
