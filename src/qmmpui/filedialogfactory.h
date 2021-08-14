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
#ifndef FILEDIALOGFACTORY_H
#define FILEDIALOGFACTORY_H

#include "qmmpui_export.h"

class QObject;
class FileDialog;
class QWidget;
class QString;

/*! @brief Structure to store file dialog plugin properties.
 */
struct FileDialogProperties
{
    bool hasAbout = false;/*!< Should be \b true if the file dialog plugin has about dialog,
                           * otherwise should be \b false */
    QString name;         /*!< File dialog plugin full name */
    QString shortName;    /*!< File dialog short name for internal usage */
    bool modal = false;   /*!<  Should be \b true if the file dialog doesn't support nonmodal mode,
                           * otherwise should be \b false */
};
/*! @brief File dialog plugin interface.
 * @author Vladimir Kuznetsov <vovanec@gmail.com>
 */
class QMMPUI_EXPORT FileDialogFactory
{
public:
    /*!
     * Object destructor.
     */
    virtual ~FileDialogFactory() {}
    /*!
     * Creates file dialog object.
     */
    virtual FileDialog* create() = 0;
    /*!
     * Returns file dialog plugin properties.
     */
    virtual FileDialogProperties properties() const = 0;
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

Q_DECLARE_INTERFACE(FileDialogFactory, "FileDialogFactory/1.0")

#endif
