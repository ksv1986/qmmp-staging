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

#ifndef EMPTYINPUTSOURCE_P_H
#define EMPTYINPUTSOURCE_P_H

#include "inputsource.h"

/*! @internal
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class EmptyInputSource : public InputSource
{
Q_OBJECT
public:
    explicit EmptyInputSource(const QString &path, QObject *parent = nullptr);

    QIODevice *ioDevice() const override;
    bool initialize() override;
    bool isReady() const override;

private:
    bool m_ok = false;
};

#endif // EMPTYINPUTSOURCE_P_H
