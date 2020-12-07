#include <QDir>
#include <qmmpui/filedialog.h>
#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::SettingsDialog)
{
    m_ui->setupUi(this);
    m_lastPath = QDir::homePath();
}

SettingsDialog::~SettingsDialog()
{
    delete m_ui;
}

void SettingsDialog::on_addDirButton_clicked()
{
    QString path = FileDialog::getExistingDirectory(this, tr("Select Directories for Scanning"), m_lastPath);
    if(!path.isEmpty())
    {
        m_ui->dirsListWidget->addItem(path);
        m_lastPath = QFileInfo(path).absolutePath();
    }
}

void SettingsDialog::on_removeDirButton_clicked()
{
    QList<QListWidgetItem *> items = m_ui->dirsListWidget->selectedItems();
    qDeleteAll(items);
}
