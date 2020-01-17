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

#ifndef QSUIWAVEFORMSEEKBAR_H
#define QSUIWAVEFORMSEEKBAR_H

#include <QWidget>
#include <QThread>
#include <QMutex>
#include <qmmp/audioparameters.h>
#include <qmmp/qmmp.h>

class SoundCore;
class Decoder;
class InputSource;
class QSUIWaveformScanner;

class QSUIWaveformSeekBar : public QWidget
{
    Q_OBJECT
public:
    explicit QSUIWaveformSeekBar(QWidget *parent = nullptr);
    QSize sizeHint() const override;

private slots:
    void onStateChanged(Qmmp::State state);
    void onScanFinished();
    void onElapsedChanged(qint64 elapsed);

private:
    void paintEvent(QPaintEvent *e) override;

    SoundCore *m_core;
    QSUIWaveformScanner *m_scanner = nullptr;
    QList<int> m_data;
    int m_channels = 0;
    qint64 m_elapsed = 0;
    qint64 m_duration = 0;
};

class QSUIWaveformScanner : public QThread
{
    Q_OBJECT
public:
    explicit QSUIWaveformScanner(QObject *parent);
    ~QSUIWaveformScanner();
    bool scan(const QString &path);
    void stop();
    const QList<int> &data() const;
    const AudioParameters &audioParameters() const;

private:
    void run() override;

    bool m_user_stop = false;
    Decoder *m_decoder = nullptr;
    InputSource *m_input = nullptr;
    QMutex m_mutex;
    QList<int> m_data;
    AudioParameters m_ap;
};

#endif // QSUIWAVEFORMSEEKBAR_H
