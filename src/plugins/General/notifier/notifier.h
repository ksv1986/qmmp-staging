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
#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <QPointer>
#include <QStringList>
#include <qmmpui/general.h>
#include <qmmp/qmmp.h>

#ifdef X11_FOUND
typedef struct _XDisplay Display;
#endif

class PopupWidget;
class SoundCore;

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/

class Notifier : public QObject
{
Q_OBJECT
public:
    Notifier(QObject *parent = nullptr);

    virtual~Notifier();

private slots:
    void showMetaData();
    void showVolume(int, int);
    void setState(Qmmp::State state);

private:
    void removePsiTuneFiles();
    bool hasFullscreenWindow() const;
#ifdef X11_FOUND
    static Display* display();
    static bool isPlatformX11();
#endif
    QPointer<PopupWidget> m_popupWidget;
    bool m_desktop, m_showVolume;
    bool m_psi;
    int m_l = -1, m_r = -1;
    bool m_isPaused = false;
    bool m_resumeNotification;
    bool m_disableForFullScreen;
    SoundCore *m_core;
    QStringList m_psiTuneFiles;

};

#endif
