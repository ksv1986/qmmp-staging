#include <QSettings>
#include <qmmp/qmmp.h>
#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent), m_ui(new Ui::SettingsDialog)
{
    m_ui->setupUi(this);
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_ui->userTokenLineEdit->setText(settings.value("ListenBrainz/user_token").toString());
}

SettingsDialog::~SettingsDialog()
{
    delete m_ui;
}

void SettingsDialog::accept()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("ListenBrainz/user_token", m_ui->userTokenLineEdit->text());
    QDialog::accept();
}
