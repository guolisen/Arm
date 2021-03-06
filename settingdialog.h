#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QSettings>
#include <QDialog>
#include <Core/appcontext.h>
#include <FileInfoModel/filemodelmgr.h>
#include <irecentusemgr.h>

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(core::ContextPtr context, RecentUseMgrPtr recentUseTool, QWidget *parent = nullptr);
    ~SettingDialog();

private slots:
    void on_pushButton_clicked();
    void on_buttonBox_accepted();
    void on_keyFileBrower_clicked();
    void emcKeyCheckBoxHandle(int check);
    void keyCheckBoxHandle(int check);
    void on_pushButton_2_clicked();

private:
    void init();
    Ui::SettingDialog *ui;
    core::ContextPtr context_;
    core::ConfigMgrPtr configMgr_;
    QSettings settings_;
    RecentUseMgrPtr recentUseTool_;
};

#endif // SETTINGDIALOG_H
