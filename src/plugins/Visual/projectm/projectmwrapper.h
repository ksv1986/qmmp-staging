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

#ifndef PROJECTMWRAPPER_H
#define PROJECTMWRAPPER_H

#include <QObject>
#include <libprojectM/projectM.hpp>

class ProjectMWrapper : public QObject, public projectM
{
    Q_OBJECT
public:
    explicit ProjectMWrapper(std::string config_file, int flags, QObject *parent = nullptr);
    explicit ProjectMWrapper(Settings settings, int flags, QObject *parent = nullptr);

    virtual ~ProjectMWrapper();

signals:
    void currentPresetChanged(int index) const;

public slots:
    void selectPreset(int index);

private:
#ifdef PROJECTM_31
    void presetSwitchedEvent(bool isHardCut, size_t index) const override;
#else
    void presetSwitchedEvent(bool isHardCut, unsigned int index) const override;
#endif


};

#endif // PROJECTMWRAPPER_H
