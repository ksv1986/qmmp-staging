/***************************************************************************
 *   Copyright (C) 2008-2019 by Ilya Kotov                                 *
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
    static QStringList filters();
};

class MplayerEngine : public AbstractEngine
{
    Q_OBJECT
public:
    MplayerEngine(QObject *parent);
    virtual ~MplayerEngine();

    // Engine API
    bool play() override;
    bool enqueue(InputSource *source) override;
    bool initialize();
    void seek(qint64) override;
    void stop() override;
    void pause() override;
    void setMuted(bool muted) override;

private slots:
    void readStdOut();
    void onError(QProcess::ProcessError error);
    void startMplayerProcess();

private:
    QStringList m_args;
    QProcess *m_process;
    int m_bitrate;
    int m_samplerate;
    int m_channels;
    int m_bitsPerSample;
    bool m_muted;
    bool m_user_stop;
    qint64 m_currentTime;
    qint64 m_length;
    QQueue <InputSource*> m_sources;
    InputSource *m_source;
};


#endif // MPLAYERENGINE_H
