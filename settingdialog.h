#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QSettings>
#include <QDialog>

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(QWidget *parent = nullptr);
    ~SettingDialog();

private slots:
    void on_pushButton_clicked();

    void on_buttonBox_accepted();

private:
    Ui::SettingDialog *ui;
    QSettings settings_;
    QString editorPath_;
};

#endif // SETTINGDIALOG_H
