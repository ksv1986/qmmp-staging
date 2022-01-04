/***************************************************************************
 *   Copyright(C) 2009-2021 by Ilya Kotov                                 *
 *   forkotov02@ya.ru                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
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
#include <QStandardPaths>
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
    m_ui.tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("FileOps");
    int i = 0;
    while(!settings.value(QString("name_%1").arg(i)).isNull())
    {
        m_ui.tableWidget->insertRow(i);
        QCheckBox *checkBox = new QCheckBox;
        checkBox->setFocusPolicy(Qt::NoFocus);
        checkBox->setChecked(settings.value(QString("enabled_%1").arg(i), true).toBool());

        QComboBox *comboBox = createComboBox();
        int ci = comboBox->findData(settings.value(QString("action_%1").arg(i), FileOps::COPY).toInt());
        comboBox->setCurrentIndex(qMax(ci, 0));
        connect(comboBox, SIGNAL(activated(int)), SLOT(updateLineEdits()));

        QTableWidgetItem *nameItem = new QTableWidgetItem(settings.value(QString("name_%1").arg(i)).toString());
        nameItem->setData(PatternRole, settings.value(QString("pattern_%1").arg(i)).toString());
        nameItem->setData(DestionationRole, settings.value(QString("destination_%1").arg(i)).toString());
        nameItem->setData(CommandRole, settings.value(QString("command_%1").arg(i)).toString());

        QTableWidgetItem *hotkeyItem = new QTableWidgetItem();
        hotkeyItem->setText(settings.value(QString("hotkey_%1").arg(i)).toString());
        hotkeyItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

        m_ui.tableWidget->setCellWidget(i, 0, checkBox);
        m_ui.tableWidget->setCellWidget(i, 1, comboBox);
        m_ui.tableWidget->setItem(i, 2, nameItem);
        m_ui.tableWidget->setItem(i, 3, hotkeyItem);

        ++i;
    }
    settings.endGroup();
    connect(m_ui.tableWidget, SIGNAL(currentCellChanged(int,int,int,int)), SLOT(updateLineEdits()));
    updateLineEdits();
    createMenus();
}


SettingsDialog::~SettingsDialog()
{}

void SettingsDialog::accept()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("FileOps");
    //remove all previous keys
    settings.remove(QString());
    //save actions
    for(int i = 0; i < m_ui.tableWidget->rowCount(); ++i)
    {
        QTableWidgetItem *item = m_ui.tableWidget->item(i, 2);
        if(item->text().isEmpty())
            continue;

        settings.setValue(QString("name_%1").arg(i), item->text());
        settings.setValue(QString("destination_%1").arg(i), item->data(DestionationRole).toString());

        QCheckBox *checkBox = qobject_cast<QCheckBox *>(m_ui.tableWidget->cellWidget(i, 0));
        settings.setValue(QString("enabled_%1").arg(i), checkBox->isChecked());

        QComboBox *comboBox = qobject_cast<QComboBox *>(m_ui.tableWidget->cellWidget(i, 1));
        settings.setValue(QString("action_%1").arg(i), comboBox->itemData(comboBox->currentIndex()));

        if(comboBox->itemData(comboBox->currentIndex()) == FileOps::EXECUTE)
            settings.setValue(QString("command_%1").arg(i), item->data(CommandRole).toString());
        else
            settings.setValue(QString("pattern_%1").arg(i), item->data(PatternRole).toString());

        settings.setValue(QString("hotkey_%1").arg(i), m_ui.tableWidget->item(i, 3)->text());
    }
    settings.endGroup();
    QDialog::accept();
}

void SettingsDialog::on_newButton_clicked()
{
    int row = m_ui.tableWidget->rowCount();
    m_ui.tableWidget->insertRow(row);
    QCheckBox *checkBox = new QCheckBox;
    checkBox->setFocusPolicy(Qt::NoFocus);
    checkBox->setChecked(true);

    QComboBox *comboBox = createComboBox();

    QTableWidgetItem *nameItem = new QTableWidgetItem(tr("New action"));
    nameItem->setData(DestionationRole, QStandardPaths::writableLocation(QStandardPaths::MusicLocation));
    nameItem->setData(PatternRole, "%p - %t");

    connect(comboBox, SIGNAL(activated(int)), SLOT(updateLineEdits()));

    m_ui.tableWidget->setCellWidget(row, 0, checkBox);
    m_ui.tableWidget->setCellWidget(row, 1, comboBox);
    m_ui.tableWidget->setItem(row, 2, nameItem);
    QTableWidgetItem *hotkeyItem = new QTableWidgetItem();
    m_ui.tableWidget->setItem(row, 3, hotkeyItem);
    m_ui.tableWidget->item(row, 3)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}

void SettingsDialog::on_deleteButton_clicked()
{
    if(m_ui.tableWidget->currentRow() >= 0)
        m_ui.tableWidget->removeRow(m_ui.tableWidget->currentRow());
}

void SettingsDialog::updateLineEdits()
{
    m_ui.destinationEdit->setVisible(false);
    m_ui.destinationLabel->setVisible(false);
    m_ui.destButton->setVisible(false);
    m_ui.patternEdit->setVisible(false);
    m_ui.patternLabel->setVisible(false);
    m_ui.patternButton->setVisible(false);
    m_ui.patternLabel->setText(tr("File name pattern:"));

    if(m_ui.tableWidget->currentRow() >= 0)
    {
        QTableWidgetItem *nameItem = m_ui.tableWidget->item(m_ui.tableWidget->currentRow(), 2);
        m_ui.destinationEdit->setText(nameItem->data(DestionationRole).toString());
        QComboBox *comboBox = qobject_cast<QComboBox *>(m_ui.tableWidget->cellWidget(m_ui.tableWidget->currentRow(), 1));
        int action = comboBox->itemData(comboBox->currentIndex()).toInt();

        if(action == FileOps::EXECUTE)
             m_ui.patternEdit->setText(nameItem->data(CommandRole).toString());
        else
            m_ui.patternEdit->setText(nameItem->data(PatternRole).toString());

        if(action == FileOps::COPY || action == FileOps::MOVE)
        {
            m_ui.destinationEdit->setVisible(true);
            m_ui.destinationLabel->setVisible(true);
            m_ui.destButton->setVisible(true);
            m_ui.patternEdit->setVisible(true);
            m_ui.patternLabel->setVisible(true);
            m_ui.patternButton->setVisible(true);
        }
        else if(action == FileOps::RENAME || action == FileOps::EXECUTE)
        {
            m_ui.patternEdit->setVisible(true);
            m_ui.patternLabel->setVisible(true);
            m_ui.patternButton->setVisible(true);
            if(action == FileOps::EXECUTE)
                m_ui.patternLabel->setText(tr("Command:"));
        }
    }
}

void SettingsDialog::on_destinationEdit_textChanged(QString dest)
{
    if(m_ui.tableWidget->currentRow() >= 0)
    {
        QTableWidgetItem *item = m_ui.tableWidget->item(m_ui.tableWidget->currentRow(), 2);
        item->setData(DestionationRole, dest);
    }
}

void SettingsDialog::on_patternEdit_textChanged(QString pattern)
{
    if(m_ui.tableWidget->currentRow() >= 0)
    {
        QTableWidgetItem *item = m_ui.tableWidget->item(m_ui.tableWidget->currentRow(), 2);
        item->setData(PatternRole, pattern);
        item->setData(CommandRole, pattern);
    }
}

void SettingsDialog::createMenus()
{
    MetaDataFormatterMenu *menu = new MetaDataFormatterMenu(MetaDataFormatterMenu::TITLE_MENU, this);
    m_ui.patternButton->setMenu(menu);
    m_ui.patternButton->setPopupMode(QToolButton::InstantPopup);
    connect(menu, SIGNAL(patternSelected(QString)), SLOT(addTitleString(QString)));
}

QComboBox *SettingsDialog::createComboBox()
{
    QComboBox *comboBox = new QComboBox;
    comboBox->addItem(tr("Copy"), FileOps::COPY);
    comboBox->addItem(tr("Rename"), FileOps::RENAME);
    comboBox->addItem(tr("Move"), FileOps::MOVE);
    comboBox->addItem(tr("Remove"), FileOps::REMOVE);
    comboBox->addItem(tr("Execute"), FileOps::EXECUTE);
    comboBox->setFocusPolicy(Qt::NoFocus);
    return comboBox;
}

void SettingsDialog::addTitleString(const QString &str)
{
    m_ui.patternEdit->insert(str);
}

void SettingsDialog::on_destButton_clicked()
{
    QString dir = FileDialog::getExistingDirectory(this, tr("Choose a directory"), m_ui.destinationEdit->text());
    if(!dir.isEmpty())
        m_ui.destinationEdit->setText(dir);
}

void SettingsDialog::on_tableWidget_itemDoubleClicked(QTableWidgetItem *item)
{
    HotkeyDialog *dialog = new HotkeyDialog(item->text(), this);
    if(m_ui.tableWidget->column(item) == 3 && dialog->exec() == QDialog::Accepted)
        item->setText(dialog->key());
    dialog->deleteLater();
}
