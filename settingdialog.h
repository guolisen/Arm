#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QSettings>
#include <QDialog>
#include <Core/appcontext.h>
#include <FileInfoModel/filemodelmgr.h>


namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(core::ContextPtr context, QWidget *parent = nullptr);
    ~SettingDialog();

private slots:
    void on_pushButton_clicked();

    void on_buttonBox_accepted();

private:
    Ui::SettingDialog *ui;
    core::ContextPtr context_;
    core::ConfigMgrPtr configMgr_;
    QSettings settings_;
    QString editorPath_;
    void init();
};

#endif // SETTINGDIALOG_H
