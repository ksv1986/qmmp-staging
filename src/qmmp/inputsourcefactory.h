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

#ifndef INPUTSOURCEFACTORY_H
#define INPUTSOURCEFACTORY_H

#include <QObject>
#include <QRegularExpression>
#include <QList>
#include <QStringList>
#include "qmmp_export.h"

class InputSource;

/*! @brief Structure to store transport plugin properies.
 * @author Ilya Kotov <forkotov02@ya.ru>
 */
struct QMMP_EXPORT InputSourceProperties
{
    QString name;                      /*!< Transport plugin full name */
    QString shortName;                 /*!< Transport plugin name for internal usage */
    QList<QRegularExpression> regExps; /*!< A list of regular expressions for supported URLs (has highest priority). */
    QStringList protocols;             /*!< A list of supported protocols. */
    bool hasAbout = false;             /*!< Should be \b true if plugin has about dialog, otherwise \b false */
    bool hasSettings = false;          /*!< Should be \b true if plugin has settings dialog, otherwise \b false */
};


/*! @brief Transport plugin interface.
 * @author Ilya Kotov <forkotov02@ya.ru>
 */
class InputSourceFactory
{
public:
    /*!
     * Returns transport plugin properties.
     */
    virtual InputSourceProperties properties() const = 0;
    /*!
     * Creates transport provided by plugin.
     * @param url URL of the stream.
     * @param parent Parent object.
     */
    virtual InputSource *create(const QString &url, QObject *parent = nullptr) = 0;
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

Q_DECLARE_INTERFACE(InputSourceFactory, "InputSourceFactory/1.0")

#endif // INPUTSOURCEFACTORY_H
