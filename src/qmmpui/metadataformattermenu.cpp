/***************************************************************************
 *   Copyright (C) 2017-2018 by Ilya Kotov                                 *
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

#include "metadataformattermenu.h"

MetaDataFormatterMenu::MetaDataFormatterMenu(Type type, QWidget *parent) :
    QMenu(parent)
{
    addAction(tr("Artist"))->setData("%p");
    addAction(tr("Album"))->setData("%a");
    addAction(tr("Album Artist"))->setData("%aa");
    if(type == TITLE_MENU)
    {
        addAction(tr("Title"))->setData("%t");
        addAction(tr("Track Number"))->setData("%n");
        addAction(tr("Two-digit Track Number"))->setData("%NN");
    }
    addAction(tr("Genre"))->setData("%g");
    addAction(tr("Comment"))->setData("%c");
    addAction(tr("Composer"))->setData("%C");
    addAction(tr("Disc Number"))->setData("%D");
    addAction(tr("Year"))->setData("%y");
    if(type == TITLE_MENU)
    {
        addAction(tr("Duration"))->setData("%l");
        addAction(tr("File Name"))->setData("%f");
        addAction(tr("File Path"))->setData("%F");
        addAction(tr("Artist - Title"))->setData("%if(%p,%p - %t,%t)");
        addAction(tr("Condition"))->setData("%if(%p&%t,%p - %t,%f)");
    }
    else if(type == GROUP_MENU)
    {
        addAction(tr("Artist - [Year] Album"))->setData("%p%if(%p&%a, - %if(%y,[%y] ,),)%a");
    }
    addAction(tr("Parent Directory Name"))->setData("%dir(0)");
    addSeparator();
    addAction(tr("Bitrate"))->setData("%{bitrate}");
    addAction(tr("Sample Rate"))->setData("%{samplerate}");
    addAction(tr("Number of Channels"))->setData("%{channels}");
    addAction(tr("Sample Size"))->setData("%{samplesize}");
    addAction(tr("Format"))->setData("%{format}");
    addAction(tr("Decoder"))->setData("%{decoder}");
    if(type == TITLE_MENU)
    {
        addAction(tr("File Size"))->setData("%{filesize}");
    }

    connect(this, SIGNAL(triggered (QAction *)), SLOT(onActionTriggered(QAction*)));
}

void MetaDataFormatterMenu::onActionTriggered(QAction *action)
{
    emit patternSelected(action->data().toString());
}
