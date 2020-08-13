/***************************************************************************
 *   Copyright (C) 2013-2020 by Ilya Kotov                                 *
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

#ifndef QMMPUIPLUGINCACHE_P_H
#define QMMPUIPLUGINCACHE_P_H

#include <QString>
#include <QObject>
#include <QSettings>

class GeneralFactory;
class UiFactory;
class FileDialogFactory;

/*! @internal
 * @author Ilya Kotov <forkotov02@ya.ru>
 */
class QmmpUiPluginCache
{
public:
    QmmpUiPluginCache(const QString &file, QSettings *settings);
    explicit QmmpUiPluginCache(QObject *instance);

    const QString shortName() const;
    const QString file() const;
    int priority() const;
    bool hasError() const;

    GeneralFactory *generalFactory();
    UiFactory *uiFactory();
    FileDialogFactory *fileDialogFactory();

    static void cleanup(QSettings *settings);

private:
    QObject *instance();
    void loadTranslation(const QString &translation);
    QString m_path;
    QString m_shortName;
    bool m_error = false;
    QObject *m_instance = nullptr;
    GeneralFactory *m_generalFactory = nullptr;
    UiFactory *m_uiFactory = nullptr;
    FileDialogFactory *m_fileDialogFactory = nullptr;
    int m_priority = 0;
};

#endif // QMMPUIPLUGINCACHE_P_H
