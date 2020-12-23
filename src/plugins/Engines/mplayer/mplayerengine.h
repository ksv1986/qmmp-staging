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

#ifndef MPLAYERENGINE_H
#define MPLAYERENGINE_H

#include <QQueue>
#include <QString>
#include <QProcess>
#include <qmmp/statehandler.h>
#include <qmmp/abstractengine.h>

class Output;
class QIDevice;
class QMenu;
class QProcess;
class TrackInfo;
class InputSource;

class MplayerInfo
{
public:
    static TrackInfo *createTrackInfo(const QString &path);
    static const QStringList &filters();
    static bool supports(const QString &path);
};

class MplayerEngine : public AbstractEngine
{
    Q_OBJECT
public:
    explicit MplayerEngine(QObject *parent);
    virtual ~MplayerEngine();

    // Engine API
    bool play() override;
    bool enqueue(InputSource *source) override;
    bool initialize();
    void seek(qint64) override;
    void stop() override;
    void pause() override;

private slots:
    void setMuted(bool muted);
    void readStdOut();
    void onError(QProcess::ProcessError error);
    void onStateChanged(QProcess::ProcessState state);
    void startMplayerProcess();

private:
    QStringList m_args;
    QProcess *m_process = nullptr;
    int m_bitrate = 0;
    int m_samplerate = 0;
    int m_channels = 0;
    int m_bitsPerSample = 0;
    bool m_user_stop = false;
    qint64 m_currentTime = 0;
    qint64 m_length = 0;
    QQueue <InputSource*> m_sources;
    InputSource *m_source = nullptr;
};


#endif // MPLAYERENGINE_H
