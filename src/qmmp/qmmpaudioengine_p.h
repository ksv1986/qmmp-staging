/***************************************************************************
 *   Copyright (C) 2009-2021 by Ilya Kotov                                 *
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

#ifndef QMMPAUDIOENGINE_P_H
#define QMMPAUDIOENGINE_P_H

#include <QQueue>
#include <QHash>
#include <atomic>
#include <QSharedPointer>
#include "abstractengine.h"
#include "trackinfo.h"
#include "audioparameters.h"

class QIODevice;
class OutputWriter;
class Effect;
class DecoderFactory;
class StateHandler;
class Decoder;
class InputSource;
class EffectFactory;
class ReplayGain;
class QmmpSettings;
class AudioConverter;
class Dithering;

/*! @internal
 * @author Ilya Kotov <forkotov02@ya.ru>
 */
class QmmpAudioEngine : public AbstractEngine
{
Q_OBJECT
public:
    QmmpAudioEngine(QObject *parent);
    ~QmmpAudioEngine();

    bool play() override;
    bool enqueue(InputSource *source) override;
    void seek(qint64 time) override;
    void stop() override;
    void pause() override;
    void addEffect(EffectFactory *factory);
    void removeEffect(EffectFactory *factory);

    static QmmpAudioEngine *instance();

private slots:
    void finish();
    void updateReplayGainSettings();
    void updateAudioSettings();
    void updateEqSettings();

private:
    void run() override;
    void reset();
    void clearDecoders();
    void flush(bool = false);
    void addOffset();
    qint64 produceSound(unsigned char *data, qint64 size, quint32 brate);
    void attachMetaData(Decoder *decoder, DecoderFactory *factory, InputSource *source);
    OutputWriter *createOutput();
    void prepareEffects(Decoder *d);

    DecoderFactory *m_factory = nullptr;
    QList<Effect*> m_effects;
    QList<Effect*> m_blockedEffects;
    OutputWriter *m_output = nullptr;

    std::atomic_bool m_done, m_finish, m_user_stop;
    uint m_bks = 0, m_sample_size = 0;
    qint64 m_seekTime;
    quint64 m_output_at, m_output_size = 0;
    int m_bitrate;
    unsigned char *m_output_buf = nullptr;
    Decoder *m_decoder = nullptr;
    QQueue <Decoder*> m_decoders;
    QHash <Decoder*, InputSource*> m_inputs;
    AudioParameters m_ap;
    bool m_next;
    QSharedPointer<TrackInfo> m_trackInfo;
    static QmmpAudioEngine *m_instance;
    ReplayGain *m_replayGain = nullptr;
    QmmpSettings *m_settings;
    AudioConverter *m_converter;
    Dithering *m_dithering = nullptr;
};

#endif // QMMPAUDIOENGINE_P_H
