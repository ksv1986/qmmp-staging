/***************************************************************************
 *   Copyright (C) 2010-2019 by Ilya Kotov                                 *
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
#include <QMap>
#include <qmmp/soundcore.h>
#include <qmmpui/metadataformatter.h>
#include "statusoption.h"


StatusOption::StatusOption()
{
    registerOption(STATUS, "--status", tr("Print playback status"));
    registerOption(NOW_PLAYING, "--nowplaying", tr("Print formatted track name (example: qmmp --nowplaying \"%t - %a\")"),
                   QStringList() << "fmt");
    registerOption(NOW_PLAYING_SYNTAX, "--nowplaying-syntax", tr("Print --nowplaying syntax"));
}

QString StatusOption::shortName() const
{
    return QLatin1String("StatusOption");
}

QString StatusOption::translation() const
{
    return QLatin1String(":/status_plugin_");
}

QString StatusOption::executeCommand(int id, const QStringList &args)
{
    SoundCore *core = SoundCore::instance();
    QString out;
    switch (id)
    {
    case STATUS:
    {
        QMap<int, QString> state_names;
        state_names.insert(Qmmp::Playing, "[playing]");
        state_names.insert(Qmmp::Paused, "[paused]");
        state_names.insert(Qmmp::Stopped, "[stopped]");
        state_names.insert(Qmmp::Buffering, "[buffering]");
        state_names.insert(Qmmp::NormalError, "[error]");
        state_names.insert(Qmmp::FatalError, "[error]");
        out += state_names[core->state()];

        if(core->state() == Qmmp::Playing || core->state() == Qmmp::Paused)
        {
            out += " ";
            out += genProgressBar() + "\n";
            out += "ARTIST = %p\n";
            out += "ALBUMARTIST = %aa\n";
            out += "TITLE = %t\n";
            out += "ALBUM = %a\n";
            out += "COMMENT = %c\n";
            out += "GENRE = %g\n";
            out += "YEAR = %y\n";
            out += "TRACK = %n\n";
            out += "FILE = %f";
            MetaDataFormatter formatter(out);
            out = formatter.format(core->trackInfo());
        }
        out += "\n";
    }
        break;
    case NOW_PLAYING:
    {
        QString t = args.join(" ");
        MetaDataFormatter formatter(t);
        out = formatter.format(core->trackInfo());
        out += "\n";
    }
        break;
    case NOW_PLAYING_SYNTAX:
    {
        out += tr("Syntax:") + "\n";
        out += tr("%p - artist") + "\n";
        out += tr("%a - album") + "\n";
        out += tr("%aa - album artist") + "\n";
        out += tr("%t - title") + "\n";
        out += tr("%n - track") + "\n";
        out += tr("%NN - 2-digit track") + "\n";
        out += tr("%g - genre") + "\n";
        out += tr("%c - comment") + "\n";
        out += tr("%C - composer") + "\n";
        out += tr("%D - disc number") + "\n";
        out += tr("%f - file name") + "\n";
        out += tr("%F - full path") + "\n";
        out += tr("%y - year") + "\n";
        out += tr("%l - duration") + "\n";
        out += tr("%{bitrate} - bitrate") + "\n";
        out += tr("%{samplerate} - sample rate") + "\n";
        out += tr("%{channels} - number of channels") + "\n";
        out += tr("%{samplesize} - bits per sample") + "\n";
        out += tr("%{format} - format name") + "\n";
        out += tr("%{decoder} - decoder name") + "\n";
        out += tr("%{filesize} - file size") + "\n";
        out += tr("%if(A&B&C,D,E) - condition") + "\n";
        out += tr("%dir(n) - directory name located on n levels above");
    }
        break;
    default:
        ;
    }

    return out;
}

QString StatusOption::genProgressBar()
{
    SoundCore *core = SoundCore::instance();
    QString totalTime = QString("%1:%2").arg(core->duration()/60000)
            .arg(core->duration()%60000/1000, 2, 10, QChar('0'));
    QString currentTime = QString("%1:%2").arg(core->elapsed()/60000)
            .arg(core->elapsed()%60000/1000, 2, 10, QChar('0'));
    QString out = currentTime;
    if(core->duration())
    {
        out.clear();
        int played_count = 22 * (double)core->elapsed()/core->duration();
        for(int i = 0; i < played_count; ++i)
            out += "=";
        out += "#";
        for(int i = played_count; i < 22; ++i)
            out += "-";
        out += " " + currentTime + "/" +totalTime;
    }
    return out;
}
