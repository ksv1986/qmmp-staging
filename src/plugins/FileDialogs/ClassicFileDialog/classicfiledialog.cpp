/**************************************************************************
*   Copyright (C) 2008-2012 by Ilya Kotov                                 *
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

#include <QtPlugin>
#include <QTranslator>
#include <QMessageBox>
#include <qmmp/qmmp.h>
#include "classicfiledialogimpl.h"
#include "classicfiledialog.h"

ClassicFileDialog::ClassicFileDialog()
{
    m_dialog = new ClassicFileDialogImpl();
    connect(m_dialog,SIGNAL(filesAdded(const QStringList&)),this,SIGNAL(filesAdded(const QStringList&)));
}

void ClassicFileDialog::handleSelected(/*const QStringList& s */)
{
}

ClassicFileDialog::~ClassicFileDialog()
{
    qWarning("ClassicFileDialog::~ClassicFileDialog()");
    delete m_dialog;
}
void ClassicFileDialog::raise(const QString &dir, Mode mode, const QString &caption,
                           const QStringList &mask)
{
    m_dialog->setModeAndMask(dir, mode, mask);
    m_dialog->setWindowTitle(caption);
    m_dialog->show();
    m_dialog->raise();
}

QString ClassicFileDialog::existingDirectory(QWidget *parent, const QString &caption, const QString &dir)
{
    ClassicFileDialogImpl *dialog = new ClassicFileDialogImpl(parent);
    dialog->setWindowTitle(caption);
    dialog->setModeAndMask(dir, FileDialog::AddDir);
    QStringList l;
    if (dialog->exec() == QDialog::Accepted)
        l = dialog->selectedFiles();
    dialog->deleteLater();
    return l.isEmpty() ? QString() : l.at(0);
}

QString ClassicFileDialog::openFileName(QWidget *parent, const QString &caption,
                                     const QString &dir, const QString &filter, QString*)
{
    ClassicFileDialogImpl *dialog = new ClassicFileDialogImpl(parent);
    dialog->setWindowTitle(caption);
    dialog->setModeAndMask(dir, FileDialog::AddFile, filter.split(";;"));
    QStringList l;
    if (dialog->exec() == QDialog::Accepted)
        l = dialog->selectedFiles();
    dialog->deleteLater();
    return l.isEmpty() ? QString() : l.at(0);
}

QStringList ClassicFileDialog::openFileNames(QWidget *parent, const QString &caption,
        const QString &dir, const QString &filter, QString *)
{
    ClassicFileDialogImpl *dialog = new ClassicFileDialogImpl(parent);
    dialog->setWindowTitle(caption);
    dialog->setModeAndMask(dir, FileDialog::AddFiles, filter.split(";;"));
    QStringList l;
    if (dialog->exec() == QDialog::Accepted)
        l = dialog->selectedFiles();
    dialog->deleteLater();
    return l;
}

QString ClassicFileDialog::saveFileName (QWidget *parent, const QString &caption,
                                      const QString &dir, const QString &filter, QString*)
{
    ClassicFileDialogImpl *dialog = new ClassicFileDialogImpl(parent);
    dialog->setWindowTitle(caption);
    dialog->setModeAndMask(dir, FileDialog::SaveFile, filter.split(";;"));
    QStringList l;
    if (dialog->exec() == QDialog::Accepted)
        l = dialog->selectedFiles();
    dialog->deleteLater();
    if (l.isEmpty())
        return QString();
    else
        return l.at(0);
}

FileDialog* ClassicFileDialogFactory::create()
{
    return new ClassicFileDialog();
}

const FileDialogProperties ClassicFileDialogFactory::properties() const
{
    FileDialogProperties properties;
    properties.name = tr("Classic File Dialog");
    properties.shortName = "classic_dialog";
    properties.hasAbout = true;
    properties.modal = false;
    return properties;
}

void ClassicFileDialogFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About Qmmp File Dialog"),
                        tr("Qmmp File Dialog")+"\n"+
                        tr("Written by:\n"
                           "Vladimir Kuznetsov <vovanec@gmail.com>\n"
                           "Ilya Kotov <forkotov02@hotmail.ru>")+"\n"+
                        tr("Some code is copied from the Qt library"));

}

QTranslator *ClassicFileDialogFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/classic_file_dialog_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN2(ClassicFileDialog, ClassicFileDialogFactory)
