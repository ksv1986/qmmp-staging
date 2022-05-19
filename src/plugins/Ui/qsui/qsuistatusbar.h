/***************************************************************************
 *   Copyright (C) 2020 by Ilya Kotov                                      *
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

#ifndef QSUISTATUSBAR_H
#define QSUISTATUSBAR_H

#include <QWidget>
#include <QHash>
#include <qmmp/audioparameters.h>
#include <qmmp/qmmp.h>

class QLabel;
class QFrame;
class SoundCore;
class PlayListManager;

class QSUiStatusBar : public QWidget
{
    Q_OBJECT
public:
    explicit QSUiStatusBar(QWidget *parent = nullptr);

    void updatePlayListStatus();

private slots:
    void onStateChanged(Qmmp::State state);
    void onBufferingProgress(int percent);
    void onAudioParametersChanged(const AudioParameters &ap);
    void onBitrateChanged(int bitrate);
    void onElapsedChanged(qint64 elapsed);

private:
    enum LabelType
    {
        StatusLabel = 0,
        SampleSizeLabel,
        ChannelsLabel,
        SampleRateLabel,
        TrackCountLabel,
        TotalTimeLabel,
        BitrateLabel,
        TimeLabel
    };

    QHash<LabelType, QLabel*> m_labels;
    QHash<LabelType, QFrame*> m_separators;
    SoundCore *m_core;
    PlayListManager *m_pl_manager;
};

#endif // QSUISTATUSBAR_H
