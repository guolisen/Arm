#include <QDebug>
#include <QFileDialog>
#include "settingdialog.h"
#include "ui_settingdialog.h"

SettingDialog::SettingDialog(core::ContextPtr context, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog),
    context_(context),
    configMgr_(context_->getComponent<core::IConfigMgr>(nullptr))
{
    ui->setupUi(this);
    init();
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

void SettingDialog::init()
{
    editorPath_ = configMgr_->getConfigInfo("Arm/Setting/EditorPath").toString();
    if (editorPath_.isEmpty())
        editorPath_ = QDir::cleanPath("C:/Program Files (x86)/Notepad++/notepad++.exe");
    ui->lineEdit->setText(editorPath_);

    //IP
    QString siteIp = configMgr_->getConfigInfo("Arm/Setting/siteIp").toString();
    if (siteIp.isEmpty())
        siteIp = "";
    ui->ipEdit->setText(siteIp);

    //port
    QString port = configMgr_->getConfigInfo("Arm/Setting/port").toString();
    if (port.isEmpty())
        port = "";
    ui->portEdit->setText(port);

    //userName
    QString userName = configMgr_->getConfigInfo("Arm/Setting/userName").toString();
    if (userName.isEmpty())
        userName = "";
    ui->userEdit->setText(userName);

    //password
    QString password = configMgr_->getConfigInfo("Arm/Setting/password").toString();
    if (password.isEmpty())
        password = "";
    ui->passwordEdit->setText(password);

    //timeOut
    QString timeOut = configMgr_->getConfigInfo("Arm/Setting/timeOut").toString();
    if (timeOut.isEmpty())
        timeOut = "";
    ui->timeoutEdit->setText(timeOut);
}

void SettingDialog::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
                this, tr("Editor Path"), editorPath_, tr("*.exe"));
    if (!fileName.isEmpty())
    {
        editorPath_ = QDir::cleanPath(fileName);
        ui->lineEdit->setText(editorPath_);
    }
}

void SettingDialog::on_buttonBox_accepted()
{
    editorPath_ = ui->lineEdit->text();
    QFileInfo info(QDir::cleanPath(editorPath_));
    if (!info.exists())
        editorPath_ = "";
    configMgr_->setConfigInfo("Arm/Setting/EditorPath", editorPath_);

    QString siteIp = ui->ipEdit->text();
    configMgr_->setConfigInfo("Arm/Setting/siteIp", siteIp);

    QString port = ui->portEdit->text();
    configMgr_->setConfigInfo("Arm/Setting/port", port);

    QString userName = ui->userEdit->text();
    configMgr_->setConfigInfo("Arm/Setting/userName", userName);

    QString password = ui->passwordEdit->text();
    configMgr_->setConfigInfo("Arm/Setting/password", password);

    QString timeOut = ui->timeoutEdit->text();
    configMgr_->setConfigInfo("Arm/Setting/timeOut", timeOut);

    close();
}
