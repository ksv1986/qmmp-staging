/***************************************************************************
 *   Copyright (C) 2009 by Ilya Kotov                                      *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <QTextCodec>
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QFileInfo>

#include <qmmp/decoder.h>
#include <qmmp/decoderfactory.h>
#include <qmmp/metadatamodel.h>
#include <qmmp/tagmodel.h>
#include "abstractplaylistitem.h"
#include "tageditor.h"
#include "detailsdialog.h"

DetailsDialog::DetailsDialog(AbstractPlaylistItem *item, QWidget *parent)
        : QDialog(parent)
{
    setAttribute(Qt::WA_QuitOnClose, FALSE);
    setAttribute(Qt::WA_DeleteOnClose, FALSE);
    m_metaDataModel = 0;
    m_item = item;
    ui.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    m_path = item->url();
    setWindowTitle (m_path.section('/',-1));
    ui.pathEdit->setText(m_path);

    if(QFile::exists(item->url()))
    {
        DecoderFactory *fact = Decoder::findByPath(item->url());
        if(fact)
            m_metaDataModel = fact->createMetaDataModel(item->url(), this);
        else
            return;

        if(!m_metaDataModel)
            return;

        foreach(TagModel *tagModel, m_metaDataModel->tags())
        {
            ui.tabWidget->addTab(new TagEditor(tagModel, this), tagModel->name());
        }
        foreach(QString title, m_metaDataModel->descriptions().keys())
        {
            QTextEdit *textEdit = new QTextEdit(this);
            textEdit->setReadOnly(TRUE);
            textEdit->setPlainText(m_metaDataModel->descriptions().value(title));
            ui.tabWidget->addTab(textEdit, title);
        }
    }
    printInfo();
}

DetailsDialog::~DetailsDialog()
{}

void DetailsDialog::printInfo()
{
    QList <FileInfo *> flist = Decoder::createPlayList(m_path, TRUE);
    QMap <Qmmp::MetaData, QString> metaData;
    if(!flist.isEmpty() && QFile::exists(m_item->url()))
        metaData = flist.at(0)->metaData();
    else
        metaData = m_item->metaData();
    QString formattedText;
    formattedText.append("<TABLE>");
    //tags
    formattedText += formatRow(tr("Title"), metaData[Qmmp::TITLE]);
    formattedText += formatRow(tr("Artist"), metaData[Qmmp::ARTIST]);
    formattedText += formatRow(tr("Album"), metaData[Qmmp::ALBUM]);
    formattedText += formatRow(tr("Comment"), metaData[Qmmp::COMMENT]);
    formattedText += formatRow(tr("Genre"), metaData[Qmmp::GENRE]);
    formattedText += formatRow(tr("Composer"), metaData[Qmmp::COMPOSER]);
    if(metaData[Qmmp::YEAR] != "0")
        formattedText += formatRow(tr("Year"), metaData[Qmmp::YEAR]);
    if(metaData[Qmmp::TRACK] != "0")
        formattedText += formatRow(tr("Track"), metaData[Qmmp::TRACK]);
    if(metaData[Qmmp::DISCNUMBER] != "0")
        formattedText += formatRow(tr("Disc number"), metaData[Qmmp::DISCNUMBER]);
    //audio info
    if(!m_metaDataModel)
    {
        formattedText.append("</TABLE>");
        ui.textEdit->setHtml(formattedText);
        return;
    }
    QHash <QString, QString> ap = m_metaDataModel->audioProperties();
    //line
    formattedText.append("<tr>");
    formattedText.append("<td colspan=2>");
    formattedText.append("<hr>");
    formattedText.append("</td>");
    formattedText.append("</tr>");

    foreach(QString key, ap.keys())
        formattedText += formatRow(key, ap.value(key));

    formattedText.append("</TABLE>");
    ui.textEdit->setHtml(formattedText);
}

QString DetailsDialog::formatRow(const QString key, const QString value)
{
    if(value.isEmpty())
        return QString();
    QString str("<tr>");
    str.append("<td><b>" + key + "</b></td> <td style=\"padding-left: 15px;\">" + value + "</td>");
    str.append("</tr>");
    return str;
}

void DetailsDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if(ui.buttonBox->standardButton(button) == QDialogButtonBox::Save)
    {
        TagEditor *tab = qobject_cast<TagEditor *> (ui.tabWidget->currentWidget());
        if(tab)
            tab->save();
    }
    else
        reject();
}
