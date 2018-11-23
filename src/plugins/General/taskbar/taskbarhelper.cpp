/***************************************************************************
 *   Copyright (C) 2018 by Ilya Kotov                                      *
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

#include <QTimer>
#include <QApplication>
#include <QMainWindow>
#include <QWinTaskbarButton>
#include <QWinTaskbarProgress>
#include <QtDebug>
#include <qmmp/soundcore.h>
#include <qmmpui/uihelper.h>
#include <qmmpui/playlistmanager.h>
#include <qmmpui/playlistitem.h>
#include <qmmpui/mediaplayer.h>
#include "taskbarhelper.h"

TaskbarHelper::TaskbarHelper(QObject *parent) : QObject(parent)
{
    SoundCore *core = SoundCore::instance();
    connect(core, &SoundCore::stateChanged, this, &TaskbarHelper::onStateChanged);
    connect(core, &SoundCore::elapsedChanged, this, &TaskbarHelper::onElapsedChanged);
    QTimer::singleShot(0, this, &TaskbarHelper::init);
}

TaskbarHelper::~TaskbarHelper()
{
    if(!m_button.isNull())
        m_button->deleteLater();
}

void TaskbarHelper::init()
{
    if(!m_button.isNull())
        return;

    //find main window add create QWinTaskbarButton object
    for(QWidget *w : qApp->topLevelWidgets())
    {
        QMainWindow *mw = nullptr;
        if((mw = qobject_cast<QMainWindow *>(w)))
        {
            mw->winId(); //load window handle
            if(mw->windowHandle())
            {
                m_button = new QWinTaskbarButton(mw);
                m_button->setWindow(mw->windowHandle());
                onStateChanged(SoundCore::instance()->state());
            }
            break;
        }
    }
}

void TaskbarHelper::onStateChanged(Qmmp::State state)
{
    if(!m_button.isNull())
    {
        QWinTaskbarProgress *progress = m_button->progress();
        SoundCore *core = SoundCore::instance();
        switch (state)
        {
        case Qmmp::Playing:
            progress->show();
            progress->setRange(0, core->duration());
            progress->setValue(core->elapsed());
            progress->resume();
            break;
        case Qmmp::Paused:
            progress->show();
            progress->pause();
            break;
        default:
            progress->setValue(0);
            progress->hide();
        }
    }
}

void TaskbarHelper::onElapsedChanged(qint64 elapsed)
{
    if(!m_button.isNull())
    {
        QWinTaskbarProgress *progress = m_button->progress();
        progress->setValue(elapsed);
    }
}
