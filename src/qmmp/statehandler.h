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
#ifndef STATEHANDLER_H
#define STATEHANDLER_H

#include <QObject>
#include <QMap>
#include <QHash>
#include <QRecursiveMutex>
#include "abstractengine.h"
#include "audioparameters.h"
#include "qmmp.h"

/*! @brief The StateHandler class allows one to track information about playback progress.
 * @author Ilya Kotov <forkotov02@ya.ru>
 */
class QMMP_EXPORT StateHandler : public QObject
{
    Q_OBJECT
public:
    /*!
     * Object constructor.
     * @param parent Parent object.
     */
    StateHandler(QObject *parent = nullptr);
    /*!
     * Destructor.
     */
    ~StateHandler();
    /*!
     * Sends information about playback progress.
     * @param elapsed Current time (in milliseconds).
     * @param bitrate Current bitrate (in kbps).
     */
    void dispatch(qint64 elapsed, int bitrate);
    /*!
     * Sends information about audio parameters.
     * @param p Output audio patameters.
     */
    void dispatch(const AudioParameters &p);
    /*!
     * Sends information about song length
     * @param duration track length in milliseconds
     */
    void dispatch(qint64 duration);
    /*!
     * Sends track information.
     * @param info track information.
     */
    bool dispatch(const TrackInfo &info);
    /*!
     * Sends stream information \b info
     */
    void dispatch(const QHash<QString, QString> &info);
    /*!
     * Sends playback state.
     */
    void dispatch(Qmmp::State state);
    /*!
     * Sends buffering progress.
     * @param percent Indicates the current percentage of buffering completed.
     */
    void dispatchBuffer(int percent);
    /*!
     * Returns the current time (in milliseconds).
     */
    qint64 elapsed() const;
    /*!
     * Returns duration in milliseconds
     */
    qint64 duration() const;
    /*!
     * Returns current bitrate (in kbps)
     */
    int bitrate() const;
    /*!
     * Returns output audio parameters.
     */
    AudioParameters audioParameters() const;
    /*!
     * Returns the current state.
     */
    Qmmp::State state() const;
    /*!
     * Sends next track request.
     */
    void sendNextTrackRequest();
    /*!
     * Sends playback finished event.
     */
    void sendFinished();
    /*!
     * Returns a pointer to the first created StateHandler instance.
     */
    static StateHandler* instance();

signals:
    /*!
     * Tracks elapesed time.
     * @param time New track position in milliseconds.
     */
    void elapsedChanged(qint64 time);
    /*!
     * Emitted when bitrate has changed.
     * @param bitrate New bitrate (in kbps)
     */
    void bitrateChanged(int bitrate);
    /*!
     * Emitted when audio parameters have changed.
     * @param p New audio parameters for output.
     */
    void audioParametersChanged(const AudioParameters &p);
     /*!
     * This signal is emitted when the stream reader fills it's buffer.
     * The argument \b progress indicates the current percentage of buffering completed.
     */
    void bufferingProgress(int progress);


private:
    qint64 m_elapsed;
    qint64 m_duration;
    bool m_sendAboutToFinish;
    int m_bitrate;
    static StateHandler* m_instance;
    TrackInfo m_info;
    QHash <QString, QString> m_streamInfo;
    Qmmp::State m_state;
    AudioParameters m_audioParameters;
    mutable QRecursiveMutex m_mutex;
};

#endif
