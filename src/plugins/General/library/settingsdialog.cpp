#include <qmmp/qmmp.h>
#include <qmmpui/filedialog.h>
#include <QDir>

#include <QSettings>
#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::SettingsDialog)
{
    m_ui->setupUi(this);
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_lastPath = settings.value("Library/last_path", QDir::homePath()).toString();
    QStringList dirs = settings.value("Library/dirs").toStringList();
    m_ui->dirsListWidget->addItems(dirs);
}

SettingsDialog::~SettingsDialog()
{
    delete m_ui;
}

void SettingsDialog::accept()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("Library/last_path", m_lastPath);

    QStringList dirs;
    for(int i = 0; i < m_ui->dirsListWidget->count(); ++i)
        dirs << m_ui->dirsListWidget->item(i)->text();

    settings.setValue("Library/dirs", dirs);
    QDialog::accept();
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
