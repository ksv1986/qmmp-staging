/***************************************************************************
 *   Copyright (C) 2006-2021 by Ilya Kotov                                 *
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

#ifndef DECODERFACTORY_H
#define DECODERFACTORY_H

#include <QObject>
#include <QString>
#include <QIODevice>
#include <QStringList>
#include "qmmp_export.h"
#include "decoder.h"
#include "metadatamodel.h"
#include "trackinfo.h"

/*! @brief Structure to store input plugin properties.
 * @author Ilya Kotov <forkotov02@ya.ru>
 */
struct DecoderProperties
{
    QString name;             /*!< Input plugin full name */
    QString shortName;        /*!< Input plugin short name for internal usage */
    QStringList filters;      /*!< File filters (example: "*.mp3") */
    QString description;      /*!< File filters description */
    QStringList contentTypes; /*!< Supported content types */
    QStringList protocols;    /*!< A list of supported protocols.
                               *   Should be empty if plugin uses stream input. */
    bool hasAbout = false;    /*!< Should be \b true if plugin has about dialog, otherwise \b false */
    bool hasSettings = false; /*!< Should be \b true if plugin has settings dialog, otherwise \b false */
    bool noInput = false;     /*!< Should be \b true if plugin has own input, otherwise \b false */
    int priority = 0;         /*!< Decoder priority. Decoders with lowest value will be used first */
};
/*! @brief Input plugin interface (decoder factory).
 * @author Ilya Kotov <forkotov02@ya.ru>
 */
class DecoderFactory
{
public:
    /*!
    * Object destructor.
    */
    virtual ~DecoderFactory() {}
    /*!
     * Returns \b true if plugin can decode data provided by \b d, otherwise returns \b false
     */
    virtual bool canDecode(QIODevice *d) const = 0;
    /*!
     * Returns decoder plugin properties.
     */
    virtual DecoderProperties properties() const = 0;
    /*!
     * Creates decoder object.
     * @param path File path
     * @param input Input data (if required)
     */
    virtual Decoder *create(const QString &path, QIODevice *input = nullptr) = 0;
    /*!
     * Extracts metadata and audio information from file \b path and returns a list of \b TrackInfo items.
     * One file may contain several playlist items (for example: cda disk or flac with embedded cue)
     * @param fileName File path.
     * @param parts parts of metadata which should be extracted from file.
     * @param ignoredPaths Pointer to a list of the files which should be ignored by the recursive search
     * (useful to exclude cue data files from playlist).
     */
    virtual QList<TrackInfo *> createPlayList(const QString &fileName, TrackInfo::Parts parts, QStringList *ignoredPaths) = 0;
    /*!
     * Creats metadata object, which provides full access to file tags.
     * @param path File path.
     * @param readOnly Open file in read-only mode (\b true - enabled, \b false - disable).
     * @return MetaDataModel pointer.
     */
    virtual MetaDataModel* createMetaDataModel(const QString &path, bool readOnly) = 0;
    /*!
     * Shows settings dialog.
     * @param parent Parent widget.
     */
    virtual void showSettings(QWidget *parent) = 0;
    /*!
     * Shows about dialog.
     * @param parent Parent widget.
     */
    virtual void showAbout(QWidget *parent) = 0;
    /*!
     * Returns translation file path without locale code and extension
     */
    virtual QString translation() const = 0;

};

Q_DECLARE_INTERFACE(DecoderFactory, "DecoderFactory/1.0")

#endif
