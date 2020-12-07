#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

private slots:
    void on_addDirButton_clicked();
    void on_removeDirButton_clicked();

private:
    Ui::SettingsDialog *m_ui;
    QString m_lastPath;
};

#endif // SETTINGSDIALOG_H
