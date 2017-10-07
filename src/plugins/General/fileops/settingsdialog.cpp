/***************************************************************************
 *   Copyright (C) 2009-2017 by Ilya Kotov                                 *
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
#include <QSettings>
#include <QHeaderView>
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDesktopServices>
#include <qmmp/qmmp.h>
#include <qmmpui/filedialog.h>
#include <qmmpui/metadataformattermenu.h>
#include "fileops.h"
#include "hotkeydialog.h"
#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
        : QDialog(parent)
{
    m_ui.setupUi(this);
    m_ui.tableWidget->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    m_ui.tableWidget->verticalHeader()->hide();
    m_ui.tableWidget->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("FileOps");
    int count = settings.value("count", 0).toInt();
    for (int i = 0; i < count; ++i)
    {
        m_ui.tableWidget->insertRow (i);
        QCheckBox *checkBox = new QCheckBox;
        checkBox->setFocusPolicy (Qt::NoFocus);

        QComboBox *comboBox = new QComboBox;
        comboBox->addItem (tr("Copy"), FileOps::COPY);
        comboBox->addItem (tr("Rename"), FileOps::RENAME);
        comboBox->addItem (tr("Move"), FileOps::MOVE);
        comboBox->addItem (tr("Remove"), FileOps::REMOVE);
        comboBox->setFocusPolicy (Qt::NoFocus);


        checkBox->setChecked(settings.value(QString("enabled_%1").arg(i), true).toBool());
        int ci = comboBox->findData (settings.value(QString("action_%1").arg(i), FileOps::COPY).toInt());
        if (ci >= 0)
            comboBox->setCurrentIndex(ci);

        connect(comboBox, SIGNAL(activated(int)), SLOT(updateLineEdits()));

        ActionItem *item = new ActionItem(settings.value(QString("name_%1").arg(i), "Action").toString());
        item->setPattern(settings.value(QString("pattern_%1").arg(i)).toString());
        item->setDestination(settings.value(QString("destination_%1").arg(i)).toString());

        m_ui.tableWidget->setCellWidget (i, 0, checkBox);
        m_ui.tableWidget->setCellWidget (i, 1, comboBox);
        m_ui.tableWidget->setItem (i, 2, item);
        QTableWidgetItem *item2 = new QTableWidgetItem();
        item2->setText(settings.value(QString("hotkey_%1").arg(i)).toString());
        m_ui.tableWidget->setItem (i, 3, item2);
        m_ui.tableWidget->item (i, 3)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }
    settings.endGroup();
    connect (m_ui.tableWidget, SIGNAL(currentCellChanged(int,int,int,int)),
             SLOT(updateLineEdits()));
    updateLineEdits();
    createMenus();
}


SettingsDialog::~SettingsDialog()
{}

void SettingsDialog::accept()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("FileOps");
    //remove unused keys
    int count = settings.value("count", 0).toInt();
    for (int i = m_ui.tableWidget->rowCount() - 1; i < count; ++i)
    {
        settings.remove (QString("enabled_%1").arg(i));
        settings.remove (QString("action_%1").arg(i));
        settings.remove (QString("name_%1").arg(i));
        settings.remove (QString("pattern_%1").arg(i));
        settings.remove (QString("destination_%1").arg(i));
    }
    //save actions
    settings.setValue("count", m_ui.tableWidget->rowCount());
    for (int i = 0; i < m_ui.tableWidget->rowCount(); ++i)
    {
        QCheckBox *checkBox = qobject_cast<QCheckBox *>(m_ui.tableWidget->cellWidget (i, 0));
        settings.setValue (QString("enabled_%1").arg(i), checkBox->isChecked());

        QComboBox *comboBox = qobject_cast<QComboBox *>(m_ui.tableWidget->cellWidget (i, 1));
        settings.setValue (QString("action_%1").arg(i), comboBox->itemData (comboBox->currentIndex()));

        ActionItem *item = (ActionItem *) m_ui.tableWidget->item(i,2);
        settings.setValue (QString("name_%1").arg(i), item->text());
        settings.setValue (QString("pattern_%1").arg(i), item->pattern());
        settings.setValue (QString("destination_%1").arg(i), item->destination());
        settings.setValue (QString("hotkey_%1").arg(i), m_ui.tableWidget->item(i,3)->text());
    }
    settings.endGroup();
    QDialog::accept();
}

void SettingsDialog::on_newButton_clicked()
{
    int row = m_ui.tableWidget->rowCount ();
    m_ui.tableWidget->insertRow (row);
    QCheckBox *checkBox = new QCheckBox;
    checkBox->setFocusPolicy (Qt::NoFocus);
    checkBox->setChecked(true);

    QComboBox *comboBox = new QComboBox;
    comboBox->addItem (tr("Copy"), FileOps::COPY);
    comboBox->addItem (tr("Rename"), FileOps::RENAME);
    comboBox->addItem (tr("Move"), FileOps::MOVE);
    comboBox->addItem (tr("Remove"), FileOps::REMOVE);
    comboBox->setFocusPolicy (Qt::NoFocus);

    ActionItem *item = new ActionItem(tr("New action"));
    item->setDestination(QDesktopServices::storageLocation(QDesktopServices::MusicLocation));
    item->setPattern("%p - %t");

    connect(comboBox, SIGNAL(activated(int)), SLOT(updateLineEdits()));

    m_ui.tableWidget->setCellWidget (row, 0, checkBox);
    m_ui.tableWidget->setCellWidget (row, 1, comboBox);
    m_ui.tableWidget->setItem (row, 2, item);
    QTableWidgetItem *item2 = new QTableWidgetItem();
    m_ui.tableWidget->setItem (row, 3, item2);
    m_ui.tableWidget->item (row, 3)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}

void SettingsDialog::on_deleteButton_clicked()
{
    if (m_ui.tableWidget->currentRow () >= 0)
        m_ui.tableWidget->removeRow (m_ui.tableWidget->currentRow ());
}

void SettingsDialog::updateLineEdits()
{
    if (m_ui.tableWidget->currentRow () >= 0)
    {
        ActionItem *item = (ActionItem *) m_ui.tableWidget->item(m_ui.tableWidget->currentRow (), 2);
        m_ui.destinationEdit->setText(item->destination());
        m_ui.patternEdit->setText(item->pattern());

        QComboBox *comboBox = qobject_cast<QComboBox *>
                (m_ui.tableWidget->cellWidget (m_ui.tableWidget->currentRow (), 1));
        int action = comboBox->itemData (comboBox->currentIndex()).toInt();
        if(action == FileOps::COPY || action == FileOps::MOVE)
        {
            m_ui.destinationEdit->setEnabled(true);
            m_ui.patternEdit->setEnabled(true);
            m_ui.destButton->setEnabled(true);
            m_ui.patternButton->setEnabled(true);
        }
        else if(action == FileOps::RENAME)
        {
            m_ui.destinationEdit->setEnabled(false);
            m_ui.patternEdit->setEnabled(true);
            m_ui.destButton->setEnabled(false);
            m_ui.patternButton->setEnabled(true);
        }
        else if(action == FileOps::REMOVE)
        {
            m_ui.destinationEdit->setEnabled(false);
            m_ui.patternEdit->setEnabled(false);
            m_ui.destButton->setEnabled(false);
            m_ui.patternButton->setEnabled(false);
        }
    }
    else
    {
        m_ui.destinationEdit->setEnabled(false);
        m_ui.patternEdit->setEnabled(false);
        m_ui.destButton->setEnabled(false);
        m_ui.patternButton->setEnabled(false);
    }
}

void SettingsDialog::on_destinationEdit_textChanged(QString dest)
{
    if (m_ui.tableWidget->currentRow () >= 0)
    {
        ActionItem *item = (ActionItem *) m_ui.tableWidget->item(m_ui.tableWidget->currentRow (), 2);
        item->setDestination(dest);
    }
}

void SettingsDialog::on_patternEdit_textChanged(QString pattern)
{
    if (m_ui.tableWidget->currentRow () >= 0)
    {
        ActionItem *item = (ActionItem *) m_ui.tableWidget->item(m_ui.tableWidget->currentRow (), 2);
        item->setPattern(pattern);
    }
}

void SettingsDialog::createMenus()
{
    MetaDataFormatterMenu *menu = new MetaDataFormatterMenu(MetaDataFormatterMenu::TITLE_MENU, this);
    m_ui.patternButton->setMenu(menu);
    m_ui.patternButton->setPopupMode(QToolButton::InstantPopup);
    connect(menu, SIGNAL(patternSelected(QString)), SLOT(addTitleString(QString)));
}

void SettingsDialog::addTitleString(const QString &str)
{
    m_ui.patternEdit->insert(str);
}

void SettingsDialog::on_destButton_clicked()
{
    QString dir = FileDialog::getExistingDirectory(this, tr("Choose a directory"),
                                        m_ui.destinationEdit->text());
    if(!dir.isEmpty())
        m_ui.destinationEdit->setText(dir);
}

void SettingsDialog::on_tableWidget_itemDoubleClicked (QTableWidgetItem *item)
{
    HotkeyDialog *dialog = new HotkeyDialog(item->text(), this);
    if (m_ui.tableWidget->column (item) == 3 && dialog->exec() == QDialog::Accepted)
        item->setText(dialog->key());
    dialog->deleteLater();
}
