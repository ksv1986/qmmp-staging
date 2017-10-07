/***************************************************************************
 *   Copyright (C) 2015-2017 by Ilya Kotov                                 *
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

#include <QMenu>
#include "columneditor_p.h"
#include "metadataformattermenu.h"
#include "ui_columneditor.h"

ColumnEditor::ColumnEditor(const QString &name, const QString &patt, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ColumnEditor)
{
    m_ui->setupUi(this);
    createMenu();
    fillTypes();

    //load inital values
    m_ui->nameLineEdit->setText(name);
    m_ui->formatLineEdit->setText(patt);
}

ColumnEditor::~ColumnEditor()
{
    delete m_ui;
}

QString ColumnEditor::name() const
{
    return m_ui->nameLineEdit->text();
}

QString ColumnEditor::pattern() const
{
    return m_ui->formatLineEdit->text();
}

void ColumnEditor::insertExpression(const QString &str)
{
    if (m_ui->formatLineEdit->cursorPosition () < 1)
        m_ui->formatLineEdit->insert(str);
    else
        m_ui->formatLineEdit->insert(" - "+str);
}

void ColumnEditor::on_comboBox_activated(int index)
{
    m_ui->formatLineEdit->setText(m_ui->comboBox->itemData(index).toString());
    m_ui->nameLineEdit->setText(m_ui->comboBox->itemText(index));
}

void ColumnEditor::on_formatLineEdit_textChanged(const QString &text)
{
    int index = m_ui->comboBox->findData(text);
    if(index < 0)
        index = m_ui->comboBox->findData("custom");
    m_ui->comboBox->setCurrentIndex(index);
}

void ColumnEditor::createMenu()
{
    MetaDataFormatterMenu *menu = new MetaDataFormatterMenu(MetaDataFormatterMenu::TITLE_MENU, this);
    m_ui->formatButton->setMenu(menu);
    connect(menu, SIGNAL(patternSelected(QString)), SLOT(insertExpression(QString)));
}

void ColumnEditor::fillTypes()
{
    m_ui->comboBox->addItem(tr("Artist"),"%p");
    m_ui->comboBox->addItem(tr("Album"),"%a");
    m_ui->comboBox->addItem(tr("Artist - Album"),"%if(%p&%a,%p - %a,)");
    m_ui->comboBox->addItem(tr("Artist - Title"),"%if(%p,%p - %t,%t)");
    m_ui->comboBox->addItem(tr("Album Artist"),"%aa");
    m_ui->comboBox->addItem(tr("Title"),"%t");
    m_ui->comboBox->addItem(tr("Track Number"),"%n");
    m_ui->comboBox->addItem(tr("Two-digit Track Number"),"%NN");
    m_ui->comboBox->addItem(tr("Genre"),"%g");
    m_ui->comboBox->addItem(tr("Comment"),"%c");
    m_ui->comboBox->addItem(tr("Composer"),"%C");
    m_ui->comboBox->addItem(tr("Duration"),"%l");
    m_ui->comboBox->addItem(tr("Disc Number"),"%D");
    m_ui->comboBox->addItem(tr("File Name"),"%f");
    m_ui->comboBox->addItem(tr("File Path"),"%F");
    m_ui->comboBox->addItem(tr("Track Index"), "%I");
    m_ui->comboBox->addItem(tr("Year"),"%y");
    m_ui->comboBox->addItem(tr("Parent Directory"),"%dir(0)");
    m_ui->comboBox->addItem(tr("Custom"),"custom");
}
