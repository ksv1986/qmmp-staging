/***************************************************************************
 *   Copyright (C) 2011-2018 by Ilya Kotov                                  *
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
#ifndef UIFACTORY_H
#define UIFACTORY_H

#include "qmmpui_export.h"

class QObject;
class QDialog;
class QString;
class QWidget;
class Control;
class General;

/*! @brief Helper class to store user interface plugin properies.
 */
class QMMPUI_EXPORT UiProperties
{
public:
    /*!
     * Constructor
     */
    UiProperties()
    {
        hasAbout = false;
    }
    QString name;        /*!< File dialog plugin full name */
    QString shortName;   /*!< File dialog short name for internal usage */
    bool hasAbout;       /*!< Should be \b true if plugin has about dialog, otherwise returns \b false */
};
/*! @brief User interface plugin interface.
 * @author Ilya Kotov <forkotov02@ya.ru>
 */
class UiFactory
{
public:
    /*!
     * Object destructor.
     */
    virtual ~UiFactory() {}
    /*!
     * Returns user interface plugin properties.
     */
    virtual UiProperties properties() const = 0;
    /*!
     * Creates user interface instance.
     */
    virtual QObject *create() = 0;
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

Q_DECLARE_INTERFACE(UiFactory, "UiFactory/1.0")
#endif
