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

#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QRegularExpression>
#include <qmmp/soundcore.h>
#include <qmmpui/metadataformatter.h>
#include <qmmpui/playlistmanager.h>
#include "qsuistatusbar.h"

QSUiStatusBar::QSUiStatusBar(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);

    for(int i = StatusLabel; i <= TimeLabel; ++i)
    {
        QLabel *label = new QLabel;
        m_labels.insert(LabelType(i), label);
        layout->addWidget(label);

        if(i != TimeLabel)
        {
            QFrame *sep = new QFrame(this);
            sep->setFrameStyle(QFrame::VLine | QFrame::Raised);
            m_separators.insert(LabelType(i), sep);
            layout->addWidget(sep);
        }
    }
    m_labels[BitrateLabel]->setAlignment(Qt::AlignRight);
    m_labels[TimeLabel]->setAlignment(Qt::AlignRight);
    layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));
    setLayout(layout);

    m_core = SoundCore::instance();
    m_pl_manager = PlayListManager::instance();
    connect(m_core, SIGNAL(stateChanged(Qmmp::State)), SLOT(onStateChanged(Qmmp::State)));
    connect(m_core, SIGNAL(bufferingProgress(int)), SLOT(onBufferingProgress(int)));
    connect(m_core, SIGNAL(audioParametersChanged(AudioParameters)), SLOT(onAudioParametersChanged(AudioParameters)));
    connect(m_core, SIGNAL(bitrateChanged(int)), SLOT(onBitrateChanged(int)));
    connect(m_core, SIGNAL(elapsedChanged(qint64)), SLOT(onElapsedChanged(qint64)));
    onStateChanged(m_core->state());
}

void QSUiStatusBar::updatePlayListStatus()
{
    int tracks = m_pl_manager->currentPlayList()->trackCount();
    qint64 duration = m_pl_manager->currentPlayList()->totalDuration();
    m_labels[TrackCountLabel]->setText(tr("tracks: %1").arg(tracks));
    m_labels[TotalTimeLabel]->setText(tr("total time: %1").arg(MetaDataFormatter::formatDuration(duration, false)));
}

void QSUiStatusBar::onStateChanged(Qmmp::State state)
{
    if(state == Qmmp::Playing || state == Qmmp::Paused)
    {        
        for(QLabel *label : m_labels.values())
            label->show();

        for(QFrame *sep : m_separators.values())
            sep->show();

        m_labels[BitrateLabel]->setMinimumWidth(0);
        m_labels[BitrateLabel]->clear();
        m_labels[TimeLabel]->setMinimumWidth(0);
        m_labels[TimeLabel]->clear();

        m_labels[StatusLabel]->setText(QString("<b>%1</b>").arg(state == Qmmp::Playing ? tr("Playing") : tr("Paused")));
        onAudioParametersChanged(m_core->audioParameters());
        onElapsedChanged(m_core->elapsed());
        onBitrateChanged(m_core->bitrate());
        updatePlayListStatus();
    }
    else if(state == Qmmp::Buffering)
    {
        for(int i = SampleRateLabel; i <= TimeLabel; ++i)
            m_labels[LabelType(i)]->hide();

        for(QFrame *sep : m_separators.values())
            sep->hide();

        m_labels[StatusLabel]->show();
        m_labels[StatusLabel]->setText(tr("Buffering"));
    }
    else if(state == Qmmp::Stopped)
    {
        for(int i = StatusLabel; i <= TimeLabel; ++i)
        {
            LabelType t = LabelType(i);
            if(t == StatusLabel || t == TrackCountLabel)
            {
                m_labels[t]->show();
                m_separators[t]->show();
            }
            else if(t == TotalTimeLabel)
            {
                m_labels[t]->show();
                m_separators[t]->hide();
            }
            else
            {
                m_labels[t]->hide();
                if(t != TimeLabel)
                    m_separators[t]->hide();
            }
        }

        m_labels[StatusLabel]->setText(QString("<b>%1</b>").arg(tr("Stopped")));
        updatePlayListStatus();
    }
    else
    {
        for(int i = SampleRateLabel; i <= TimeLabel; ++i)
            m_labels[LabelType(i)]->hide();

        for(QFrame *sep : m_separators.values())
            sep->hide();

        m_labels[StatusLabel]->show();
        m_labels[StatusLabel]->setText(QString("<b>%1</b>").arg(tr("Error")));
        updatePlayListStatus();
    }
}

void QSUiStatusBar::onBufferingProgress(int percent)
{
    if(m_core->state() == Qmmp::Buffering)
        m_labels[StatusLabel]->setText(tr("Buffering: %1%").arg(percent));
}

void QSUiStatusBar::onAudioParametersChanged(const AudioParameters &ap)
{
    m_labels[SampleSizeLabel]->setText(tr("%1 bits").arg(ap.validBitsPerSample()));
    if(ap.channels() == 1)
        m_labels[ChannelsLabel]->setText(tr("mono"));
    else if(ap.channels() == 2)
        m_labels[ChannelsLabel]->setText(tr("stereo"));
    else
        m_labels[ChannelsLabel]->setText(tr("%n channels", "", ap.channels()));
    m_labels[SampleRateLabel]->setText(tr("%1 Hz").arg(ap.sampleRate()));
}

void QSUiStatusBar::onBitrateChanged(int bitrate)
{
    QString text = tr("%1 kbps").arg(bitrate);
    QLabel *label = m_labels[BitrateLabel];
    if(text.size() > label->text().size()) //label width tuning to avoid text jumping
    {
        QString tmp = text;
        tmp.replace(QRegularExpression("\\d"), "4");
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
        int width = label->fontMetrics().horizontalAdvance(tmp);
#else
        int width = label->fontMetrics().width(tmp);
#endif
        label->setMinimumWidth(width);
    }
    label->setText(text);
}

void QSUiStatusBar::onElapsedChanged(qint64 elapsed)
{
    QString elapsedText = MetaDataFormatter::formatDuration(elapsed, false);
    QString plDurationText;
    QLabel *label = m_labels[TimeLabel];
    if(m_core->duration() > 1000)
    {
        plDurationText.append("/");
        plDurationText.append(MetaDataFormatter::formatDuration(m_core->duration()));
    }
    if((elapsedText.size() + plDurationText.size()) != label->text().size()) //label width tuning to avoid text jumping
    {
        QString tmp = elapsedText;
        tmp.replace(QRegularExpression("\\d"), "4");
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
        int width = label->fontMetrics().horizontalAdvance(tmp + plDurationText);
#else
        int width = label->fontMetrics().width(tmp + plDurationText);
#endif
        label->setMinimumWidth(width);
    }
    label->setText(elapsedText + plDurationText);
}
