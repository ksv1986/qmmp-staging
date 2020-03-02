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
#include <QPixmap>
#include <qmmp/audioparameters.h>
#include <qmmp/qmmp.h>

class QMenu;
class SoundCore;
class Decoder;
class InputSource;
class QSUiWaveformScanner;

/**
   @author Ilya Kotov <forkotov02@ya.ru>
*/
class QSUiWaveformSeekBar : public QWidget
{
    Q_OBJECT
public:
    explicit QSUiWaveformSeekBar(QWidget *parent = nullptr);
    QSize sizeHint() const override;
    void readSettings();

private slots:
    void onStateChanged(Qmmp::State state);
    void onScanFinished();
    void onDataChanged();
    void onElapsedChanged(qint64 elapsed);
    void writeSettings();

private:
    void paintEvent(QPaintEvent *e) override;
    void resizeEvent(QResizeEvent *) override;
    void showEvent(QShowEvent *) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void drawWaveform();
    void createMenu();

    SoundCore *m_core;
    QSUiWaveformScanner *m_scanner = nullptr;
    QMenu *m_menu;
    QAction *m_showTwoChannelsAction, *m_showRmsAction;
    QList<int> m_data;
    int m_channels = 0;
    qint64 m_elapsed = 0;
    qint64 m_duration = 0;
    QColor m_bgColor, m_rmsColor, m_waveFormColor, m_progressBar;
    QPixmap m_pixmap;
    int m_pressedPos = -1;
    bool m_update = false;
};

class QSUiWaveformScanner : public QThread
{
    Q_OBJECT
public:
    explicit QSUiWaveformScanner(QObject *parent);
    ~QSUiWaveformScanner();
    bool scan(const QString &path);
    void stop();
    const QList<int> &data() const;
    const AudioParameters &audioParameters() const;

signals:
    void dataChanged();

private:
    void run() override;

    bool m_user_stop = false;
    Decoder *m_decoder = nullptr;
    InputSource *m_input = nullptr;
    mutable QMutex m_mutex;
    QList<int> m_data;
    AudioParameters m_ap;
};

#endif // QSUIWAVEFORMSEEKBAR_H
