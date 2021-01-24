/***************************************************************************
 *   Copyright (C) 2011-2021 by Ilya Kotov                                 *
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

#include <stdio.h>
#include <QStringList>
#include <QStandardPaths>
#include <qmmp/inputsourcefactory.h>
#include <qmmp/decoderfactory.h>
#include <qmmp/metadatamanager.h>
#include <qmmp/audioconverter.h>
#include <qmmp/buffer.h>
#include <qmmpui/metadataformatter.h>
#include <QtEndian>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/mpegfile.h>
#include "converter.h"

Converter::Converter(QObject *parent) : QObject(parent), QRunnable()
{}

Converter::~Converter()
{
    qDebug("%s", Q_FUNC_INFO);
    if(m_decoder)
    {
        delete m_decoder;
        m_decoder = nullptr;
    }
    if(m_input)
    {
        delete m_input;
        m_input = nullptr;
    }
}

bool Converter::prepare(const QString &url, int row, const QVariantMap &preset)
{
    m_row = row;
    InputSource *source = InputSource::create(url, this);
    if(!source->initialize())
    {
        delete source;
        qWarning("Converter: Invalid url");
        return false;
    }

    if(source->ioDevice())
    {
        if(!source->ioDevice()->open(QIODevice::ReadOnly))
        {
            source->deleteLater();
            qWarning("Converter: cannot open input stream, error: %s",
                     qPrintable(source->ioDevice()->errorString()));
            return false;
        }
    }

    DecoderFactory *factory = nullptr;

    if(!source->path().contains("://"))
        factory = Decoder::findByFilePath(source->path());
    if(!factory)
        factory = Decoder::findByMime(source->contentType());
    if(!factory && source->ioDevice() && source->path().contains("://")) //ignore content of local files
        factory = Decoder::findByContent(source->ioDevice());
    if(!factory && source->path().contains("://"))
        factory = Decoder::findByProtocol(source->path().section("://",0,0));
    if(!factory)
    {
        qWarning("Converter: unsupported file format");
        source->deleteLater();
        return false;
    }
    qDebug("Converter: selected decoder: %s",qPrintable(factory->properties().shortName));
    if(factory->properties().noInput && source->ioDevice())
        source->ioDevice()->close();
    Decoder *decoder = factory->create(source->path(), source->ioDevice());
    if(!decoder->initialize())
    {
        qWarning("Converter: invalid file format");
        source->deleteLater();
        delete decoder;
        return false;
    }
    m_decoder = decoder;
    m_input = source;
    m_preset = preset;
    if(!decoder->totalTime())
        source->setOffset(-1);
    m_user_stop = false;
    return true;
}

void Converter::stop()
{
    m_mutex.lock();
    m_user_stop = true;
    m_mutex.unlock();
}

void Converter::run()
{
    qDebug("Converter: staring thread");
    if(m_user_stop)
    {
        emit message(m_row, tr("Cancelled"));
        emit finished(this);
        return;
    }

    AudioParameters ap = m_decoder->audioParameters();
    QString path = m_input->path();
    QString out_path = m_preset["out_dir"].toString();
    QString pattern = m_preset["file_name"].toString();

    QList<TrackInfo *> list = MetaDataManager::instance()->createPlayList(path);

    if(list.isEmpty() || out_path.isEmpty() || pattern.isEmpty())
    {
        qWarning("Converter: invalid parameters");
        emit message(m_row, tr("Error"));
        return;
    }

    TrackInfo info = *list.first();
    qDeleteAll(list);
    list.clear();
    MetaDataFormatter formatter(pattern);


    QString name = formatter.format(info);
    name.remove("'");
    name.remove("\"");
    name.remove(":");
    QString full_path = out_path + "/" + name + "." + m_preset["ext"].toString();

    if(QFile::exists(full_path))
    {
        if(m_preset["overwrite"].toBool()) //remove previous file
            QFile::remove(full_path);
        else
        {
            int i = 0;
            while(QFile::exists(full_path)) //create file with another name
            {
                ++i;
                full_path = out_path + "/" + name + QString("_%1.").arg(i) + m_preset["ext"].toString();
            }
        }
    }

    QString command = m_preset["command"].toString();
    command.replace("%o", "\"" + full_path + "\"");
    QString tmp_path = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/tmp.wav";
    bool use_file = command.contains("%i");
    command.replace("%i", "\"" + tmp_path + "\"");

    qDebug("Converter: starting task '%s'", qPrintable(m_preset["name"].toString()));
    emit message(m_row, tr("Converting"));

    char wave_header[] = { 0x52, 0x49, 0x46, 0x46, //"RIFF"
                           0x00, 0x00, 0x00, 0x00, //(file size) - 8
                           0x57, 0x41, 0x56, 0x45, //WAVE
                           0x66, 0x6d, 0x74, 0x20, //"fmt "
                           0x10, 0x00, 0x00, 0x00, //16 + extra format bytes
                           0x01, 0x00, 0x02, 0x00, //PCM/uncompressed, channels
                           0x00, 0x00, 0x00, 0x00, //sample rate
                           0x00, 0x00, 0x00, 0x00, //average bytes per second
                           0x04, 0x00, 0x10, 0x00, //block align, significant bits per sample
                           0x64, 0x61, 0x74, 0x61, //"data"
                           0x00, 0x00, 0x00, 0x00 }; //chunk size*/

    quint16 sample_size = m_preset["use_16bit"].toBool() ? 2 : ap.sampleSize();
    quint32 sample_rate = qToLittleEndian(ap.sampleRate());
    quint16 channels = qToLittleEndian((quint16)ap.channels());
    quint16 block_align = qToLittleEndian((quint16)sample_size * ap.channels());
    quint16 bps = qToLittleEndian((quint16)sample_size * 8);
    quint32 size = m_decoder->totalTime() * ap.sampleRate() * ap.channels() * sample_size / 1000;
    size = qToLittleEndian(size);

    memcpy(&wave_header[22], &channels, 2);
    memcpy(&wave_header[24], &sample_rate, 4);
    memcpy(&wave_header[32], &block_align, 2);
    memcpy(&wave_header[34], &bps, 2);
    memcpy(&wave_header[40], &size, 4);

    FILE *enc_pipe = use_file ? fopen(qPrintable(tmp_path), "w+b") : popen(qPrintable(command), "w");
    if(!enc_pipe)
    {
        qWarning("Converter: unable to open pipe");
        emit message(m_row, tr("Error"));
        return;

    }
    size_t to_write = sizeof(wave_header);
    if(to_write != fwrite(&wave_header, 1, to_write, enc_pipe))
    {
        qWarning("Converter: output file write error");
        use_file ? fclose(enc_pipe) : pclose(enc_pipe);
        return;
    }

    convert(m_decoder, enc_pipe, m_preset["use_16bit"].toBool());
    use_file ? fclose(enc_pipe) : pclose(enc_pipe);
    m_mutex.lock();
    if(m_user_stop)
    {
        qDebug("Converter: task '%s' aborted", qPrintable(m_preset["name"].toString()));
        emit message(m_row, tr("Cancelled"));
        m_mutex.unlock();
        return;
    }
    else
        qDebug("Converter: task '%s' finished with success", qPrintable(m_preset["name"].toString()));
    m_mutex.unlock();

    if(use_file)
    {
        qDebug("Converter: starting file encoding...");
        emit message(m_row, tr("Encoding"));
        enc_pipe = popen(qPrintable(command), "w");
        if(!enc_pipe)
        {
            qWarning("Converter: unable to start encoder");
            QFile::remove(tmp_path);
            return;
        }
        pclose(enc_pipe);
        qDebug("Converter: encoding finished");
        QFile::remove(tmp_path);
    }

    if(m_preset["tags"].toBool())
    {
        qDebug("Converter: writing tags");
        TagLib::FileRef file(qPrintable(full_path));
        if(file.tag())
        {
            file.tag()->setTitle(QStringToTString(info.value(Qmmp::TITLE)));
            file.tag()->setArtist(QStringToTString(info.value(Qmmp::ARTIST)));
            file.tag()->setAlbum(QStringToTString(info.value(Qmmp::ALBUM)));
            file.tag()->setGenre(QStringToTString(info.value(Qmmp::GENRE)));
            file.tag()->setComment(QStringToTString(info.value(Qmmp::COMMENT)));
            file.tag()->setYear(info.value(Qmmp::YEAR).toUInt());
            file.tag()->setTrack(info.value(Qmmp::TRACK).toUInt());

            if(full_path.endsWith(".mp3", Qt::CaseInsensitive))
            {
                TagLib::MPEG::File *mpeg_file = dynamic_cast <TagLib::MPEG::File *> (file.file());
#if ((TAGLIB_MAJOR_VERSION == 1) && (TAGLIB_MINOR_VERSION <= 11))
                mpeg_file->save(TagLib::MPEG::File::ID3v2, true);
#else
                mpeg_file->save(TagLib::MPEG::File::ID3v2, TagLib::File::StripOthers);
#endif
            }
            else
                file.save();
        }
    }

    qDebug("Converter: thread finished");
    emit message(m_row, tr("Finished"));
    emit finished(this);
}

bool Converter::convert(Decoder *decoder, FILE *file, bool use16bit)
{
    AudioParameters ap = decoder->audioParameters();
    AudioConverter inConverter, outConverter;
    qint64 len, total = 0;
    Qmmp::AudioFormat outFormat = Qmmp::PCM_S16LE;
    int percent = 0;
    int prev_percent = 0;
    int samples = 0, output_at = 0;

    qint64 totalSize = decoder->totalTime() * ap.sampleRate() * ap.channels() * ap.sampleSize() / 1000;

    inConverter.configure(ap.format());

    if(use16bit)
        outFormat = Qmmp::PCM_S16LE;
    else if(ap.sampleSize() == 1)
        outFormat = Qmmp::PCM_S8;
    else if(ap.sampleSize() == 2)
        outFormat = Qmmp::PCM_S16LE;
    else if(ap.sampleSize() == 4)
        outFormat = Qmmp::PCM_S32LE;

    outConverter.configure(outFormat);

    int outSampleSize = AudioParameters::sampleSize(outFormat);

    unsigned char input_buffer[QMMP_BLOCK_FRAMES * ap.frameSize() * 4];
    float converter_buffer[QMMP_BLOCK_FRAMES * ap.channels() * sizeof(float)];
    unsigned char output_buffer[QMMP_BLOCK_FRAMES * outSampleSize * ap.channels() * 4];

    emit progress(0);

    forever
    {
        // decode
        len = decoder->read(input_buffer, sizeof(input_buffer));

        if (len > 0)
        {
            total += len;

            samples = len / ap.sampleSize();

            inConverter.toFloat(input_buffer, converter_buffer, samples);
            outConverter.fromFloat(converter_buffer, output_buffer, samples);
            output_at = samples * outSampleSize;

            while(output_at > 0)
            {
                len = fwrite(output_buffer, 1, output_at, file);
                if(len == 0)
                {
                    qWarning("Converter: error");
                    return false;
                }
                output_at -= len;
                memmove(output_buffer, output_buffer + len, output_at);
            }
            percent = 100 * total / totalSize;
            if(percent != prev_percent)
            {
                prev_percent = percent;
                emit progress(percent);
            }
        }
        else
        {
            emit progress(100);
            qDebug("Converter: total written: %lld bytes", total);
            qDebug("finished!");
            return true;
        }

        m_mutex.lock();
        if(m_user_stop)
        {
            m_mutex.unlock();
            return false;
        }
        m_mutex.unlock();
    }
    return false;
}
