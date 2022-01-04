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
#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include <QObject>
#include <qmmp/soundcore.h>
#include "playlistmanager.h"
#include "qmmpui_export.h"

class QmmpUiSettings;
class QTimer;


/*! @brief The MediaPlayer class provides a simple way to use SoundCore and PlayListModel together.
 * @author Ilya Kotov <forkotov02@ya.ru>
 */
class QMMPUI_EXPORT MediaPlayer : public QObject
{
    Q_OBJECT
public:
    /*!
     * Object constructor,
     * @param parent Parent object.
     */
    MediaPlayer(QObject *parent = nullptr);
    /*!
     * Destructor
     */
    ~MediaPlayer();
    /*!
     * Returns a pointer to the object's instance.
     */
    static MediaPlayer* instance();
    /*!
     * Returns playlist manager pointer
     */
    PlayListManager *playListManager();

signals:
    /*!
     * Signal emitted when playlist has finished or playback has been stopped by user.
     * This signal should be used to reset/restore window title.
     */
    void playbackFinished();

public slots:
    /*!
     * Starts playback.
     */
    void play(qint64 offset = -1);
    /*!
     * Stops playback.
     */
    void stop();
    /*!
     * Sets next playlist item for playing.
     */
    void next();
    /*!
     * Sets previous playlist item for playing.
     */
    void previous();

private slots:
    void playNext();
    void updateNextUrl();
    void processState(Qmmp::State state);
    void updateMetaData();

private:
    QmmpUiSettings *m_settings;
    PlayListManager *m_pl_manager;
    SoundCore *m_core;
    static MediaPlayer* m_instance;
    int m_skips = 0;
    QString m_nextUrl;
    QTimer *m_finishTimer;
};

#endif
