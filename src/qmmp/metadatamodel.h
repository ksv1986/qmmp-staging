/***************************************************************************
 *   Copyright (C) 2009-2018 by Ilya Kotov                                 *
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

#ifndef METADATAMODEL_H
#define METADATAMODEL_H

#include <QHash>
#include <QList>
#include <QString>
#include <QCoreApplication>
#include <QPixmap>
#include <QVariant>
#include <QFlags>
#include "tagmodel.h"


class QMMP_EXPORT MetaDataItem
{
public:
    MetaDataItem(const QString &name, const QVariant &value, const QString &suffix = QString());

    const QString &name() const;
    void setName(const QString &name);
    const QVariant &value() const;
    void setValue(const QString &value);
    const QString &suffix() const;
    void setSuffix(const QString &suffix);

private:
    QString m_name, m_suffix;
    QVariant m_value;
};

/*! @brief The MetaDataModel is the base interface class of metadata access.
 * @author Ilya Kotov <forkotov02@ya.ru>
 */
class QMMP_EXPORT MetaDataModel
{
public:
    enum DialogHint
    {
        NO_HINTS = 0x0,
        IS_COVER_EDITABLE = 0x1,
        COMPLETE_PROPERTY_LIST = 0x2
    };
    Q_DECLARE_FLAGS(DialogHints, DialogHint)
    /*!
     * Constructor.
     */
    MetaDataModel(bool readOnly);
    /*!
     * Destructor.
     */
    virtual ~MetaDataModel();

    virtual QList<MetaDataItem> extraProperties() const;
    virtual QList<MetaDataItem> descriptions() const;
    /*!
     * Returns a list of available tags.
     * Subclass should reimplement this function. Default implementation returns empty array.
     */
    virtual QList<TagModel* > tags() const;
    /*!
     * Returns cover pixmap.
     * Subclass should reimplement this function. Default implementation returns empty pixmap.
     */
    virtual QPixmap cover() const;
    virtual void setCover(const QPixmap &pix) const;
    virtual void removeCover() const;
    /*!
     * Returns path to cover pixmap.
     */
    virtual QString coverPath() const;
    void setCover(const QPixmap &cover);
    bool isReadOnly() const;
    const DialogHints &dialogHints() const;

protected:
    void setDialogHints(const DialogHints &hints);
    void setReadOnly(bool readOnly);

private:
    bool m_readOnly;
    DialogHints m_dialogHints;
};

#endif // METADATAMODEL_H
