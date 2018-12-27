/***************************************************************************
 *   Copyright (C) 2011-2019 by Ilya Kotov                                 *
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

#include <QtPlugin>
#include <QLocale>
#include <qmmp/soundcore.h>
#include <qmmpui/playlistmanager.h>
#include <qmmpui/metadataformatter.h>
#include <qmmpui/mediaplayer.h>
#include <qmmpui/qmmpuisettings.h>
#include <qmmpui/commandlinemanager.h>
#include "playlistoption.h"

void PlayListOption::registerOprions()
{
    registerOption(PL_HELP, "--pl-help", tr("Show playlist manipulation commands"));
    registerOption(PL_LIST, "--pl-list", tr("List all available playlists"));
    registerOption(PL_DUMP, "--pl-dump", tr("Show playlist content"), QStringList() << "id");
    registerOption(PL_PLAY, "--pl-play", tr("Play track <track> in playlist <id>"), QStringList() << "id" << "track");
    registerOption(PL_CLEAR, "--pl-clear",  tr("Clear playlist"), QStringList() << "id");
    registerOption(PL_REPEATE_TOGGLE, "--pl-repeat-toggle", tr("Toggle playlist repeat"));
    registerOption(PL_SHUFFLE_TOGGLE, "--pl-shuffle-toggle", tr("Toggle playlist shuffle"));
    registerOption(PL_STATE, "--pl-state", tr("Show playlist options"));

    setOptionFlags(PL_HELP, NO_START);
    setOptionFlags(PL_LIST, HIDDEN_FROM_HELP);
    setOptionFlags(PL_DUMP, HIDDEN_FROM_HELP);
    setOptionFlags(PL_PLAY, HIDDEN_FROM_HELP);
    setOptionFlags(PL_CLEAR, HIDDEN_FROM_HELP);
    setOptionFlags(PL_REPEATE_TOGGLE, HIDDEN_FROM_HELP);
    setOptionFlags(PL_SHUFFLE_TOGGLE, HIDDEN_FROM_HELP);
    setOptionFlags(PL_STATE, HIDDEN_FROM_HELP);
}

QString PlayListOption::shortName() const
{
    return QLatin1String("PlayListOption");
}

QString PlayListOption::translation() const
{
    return QLatin1String(":/playlist_plugin_");
}

QString PlayListOption::executeCommand(int id, const QStringList &args)
{
    QString out;
    PlayListManager *pl_manager = PlayListManager::instance();
    MediaPlayer *player = MediaPlayer::instance();
    QmmpUiSettings *ui_settings = QmmpUiSettings::instance();


    switch (id)
    {
    case PL_HELP:
    {
        QStringList list = QStringList()
                << helpString(PL_LIST)
                << helpString(PL_DUMP)
                << helpString(PL_PLAY)
                << helpString(PL_CLEAR)
                << helpString(PL_REPEATE_TOGGLE)
                << helpString(PL_SHUFFLE_TOGGLE)
                << helpString(PL_STATE);

        foreach (QString line, list)
            out += CommandLineManager::formatHelpString(line) + "\n";
    }
        break;
    case PL_LIST:
    {
        QStringList names = pl_manager->playListNames();
        for(int i = 0; i <  names.count(); ++i)
        {
            if(i == pl_manager->currentPlayListIndex())
                out += QString("%1. %2 [*]\n").arg(i+1).arg(names.at(i));
            else
                out += QString("%1. %2\n").arg(i+1).arg(names.at(i));
        }
    }
        break;
    case PL_DUMP:
    {
        MetaDataFormatter formatter("%p%if(%p&%t, - ,)%t%if(%p,,%if(%t,,%f))%if(%l, - %l,)");
        int id = args.isEmpty() ? pl_manager->currentPlayListIndex() : args.at(0).toInt() - 1;
        PlayListModel *model = pl_manager->playListAt(id);
        if(!model)
            return tr("Invalid playlist ID") + "\n";
        for(int i = 0; i < model->count(); ++i)
        {
            PlayListTrack *track = model->track(i);
            if(!track)
                continue;
            out += QString("%1. %2").arg(model->indexOfTrack(i) + 1).arg(formatter.format(track));
            if(i == model->currentIndex())
                out += " [*]";
            out += "\n";
        }
    }
        break;
    case PL_PLAY:
    {
        if(args.count() > 2 || args.isEmpty())
            return tr("Invalid number of arguments") + "\n";

        int pl_id = (args.count() == 1) ? pl_manager->currentPlayListIndex() : args.at(0).toInt() - 1;
        int track_number = (args.count() == 1) ? args.at(0).toInt() - 1 : args.at(1).toInt() - 1;
        PlayListModel *model = pl_manager->playListAt(pl_id);
        if(!model)
            return tr("Invalid playlist ID") + "\n";

        PlayListTrack *track = model->findTrack(track_number);
        if(!track)
            return tr("Invalid track ID") + "\n";
        player->stop();
        pl_manager->activatePlayList(model);
        pl_manager->selectPlayList(model);
        model->setCurrent(track);
        player->play();
    }
        break;
    case PL_CLEAR:
    {
        int id = args.isEmpty() ? pl_manager->currentPlayListIndex() : args.at(0).toInt() - 1;
        PlayListModel *model = pl_manager->playListAt(id);
        if(!model)
            return tr("Invalid playlist ID") + "\n";
        model->clear();
    }
        break;
    case PL_REPEATE_TOGGLE:
        ui_settings->setRepeatableList(!ui_settings->isRepeatableList());
        break;
    case PL_SHUFFLE_TOGGLE:
        ui_settings->setShuffle(!ui_settings->isShuffle());
        break;
    case PL_STATE:
        out += "SHUFFLE:             " + boolToText(ui_settings->isShuffle()) + "\n";
        out += "REPEAT PLAYLIST:     " + boolToText(ui_settings->isRepeatableList()) + "\n";
        out += "REPEAT TRACK:        " + boolToText(ui_settings->isRepeatableTrack()) + "\n";
        out += "NO PLAYLIST ADVANCE: " + boolToText(ui_settings->isNoPlayListAdvance()) + "\n";
        break;
    default:
        ;
    }
    return out;
}

QString PlayListOption::boolToText(bool enabled)
{
    return QString(enabled ? "[+]" : "[-]");
}
