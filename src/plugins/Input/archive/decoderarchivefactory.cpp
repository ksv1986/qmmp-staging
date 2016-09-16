/***************************************************************************
 *   Copyright (C) 2016 by Ilya Kotov                                      *
 *   forkotov02@hotmail.ru                                                 *
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
#include <QRegExp>
#include <QMessageBox>
#include <QTranslator>
#include <QtPlugin>
#include <archive.h>
#include <archive_entry.h>
#include "decoder_archive.h"
#include "decoderarchivefactory.h"

// DecoderSndFileFactory
bool DecoderArchiveFactory::canDecode(QIODevice *input) const
{
    return false;
}

const DecoderProperties DecoderArchiveFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("Archive Plugin");
    properties.filters << "*.rar" << "*.zip";
    properties.description = tr("Archives");
    //properties.contentType = "";
    properties.shortName = "archive";
    properties.hasAbout = false;
    properties.hasSettings = false;
    properties.noInput = true;
    properties.protocols << "rar" << "zip";
    return properties;
}

Decoder *DecoderArchiveFactory::create(const QString &url, QIODevice *)
{
    return new DecoderArchive(url);
}

QList<FileInfo *> DecoderArchiveFactory::createPlayList(const QString &fileName, bool useMetaData, QStringList *)
{
    QList <FileInfo *> list;


    struct archive *a;
    struct archive_entry *entry;
    int r;

    a = archive_read_new();
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);
    r = archive_read_open_filename(a, fileName.toLocal8Bit().constData(), 10240);
    if (r != ARCHIVE_OK)
    {
        //exit(1);
        return list;
    }
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK)
    {
        if(archive_entry_filetype(entry) == AE_IFREG)
        {
            QString pathName = QString::fromLocal8Bit(archive_entry_pathname(entry));
            if(!pathName.startsWith("/"))
                pathName.prepend("/");

            list << new FileInfo(QString("%1://%2#%3")
                                 .arg(fileName.section(".", -1)).toLower()
                                 .arg(fileName)
                                 .arg(pathName));
            //TODO read tags
        }
        archive_read_data_skip(a);
    }
    r = archive_read_free(a);

    return list;
}

MetaDataModel* DecoderArchiveFactory::createMetaDataModel(const QString&, QObject *)
{
    return 0;
}

void DecoderArchiveFactory::showSettings(QWidget *)
{}

void DecoderArchiveFactory::showAbout(QWidget *parent)
{
    /*char  version [128] ;
    sf_command (NULL, SFC_GET_LIB_VERSION, version, sizeof (version)) ;
    QMessageBox::about (parent, tr("About Sndfile Audio Plugin"),
                        tr("Qmmp Sndfile Audio Plugin")+"\n"+
                        tr("Compiled against")+" "+QString(version)+"\n" +
                        tr("Written by: Ilya Kotov <forkotov02@hotmail.ru>"));*/
}

QTranslator *DecoderArchiveFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/archive_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN2(archive, DecoderArchiveFactory)
