#include <QDebug>
#include <QFileDialog>
#include "settingdialog.h"
#include "ui_settingdialog.h"

SettingDialog::SettingDialog(core::ContextPtr context, RecentUseMgrPtr recentUseTool, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog),
    context_(context),
    configMgr_(context_->getComponent<core::IConfigMgr>(nullptr)),
    recentUseTool_(recentUseTool)
{
    ui->setupUi(this);
    Qt::WindowFlags flag = windowFlags();
    flag = flag & (~Qt::WindowContextHelpButtonHint);
    setWindowFlags(flag);
    init();
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

void SettingDialog::init()
{
    QString editorPath = configMgr_->getConfigInfo("Arm/Setting/EditorPath").toString();
    if (editorPath.isEmpty())
        editorPath = QDir::cleanPath("C:/Program Files (x86)/Notepad++/notepad++.exe");
    ui->lineEdit->setText(editorPath);

    QString keyFilePath = configMgr_->getConfigInfo("Arm/Setting/keyFilePath").toString();
    if (keyFilePath.isEmpty())
        keyFilePath = "";
    ui->keyEdit->setText(keyFilePath);

    //IP
    //QString siteIp = configMgr_->getConfigInfo("Arm/Setting/siteIp").toString();
    //if (siteIp.isEmpty())
    //    siteIp = "";
    //ui->ipEdit->setText(siteIp);

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

    //keyFile
    QString keyFile = configMgr_->getConfigInfo("Arm/Setting/keyFile", "0").toString();
    ui->KeyCheckBox->setCheckState((Qt::CheckState)keyFile.toInt());
    keyCheckBoxHandle((Qt::CheckState)keyFile.toInt());

    //keyFile
    QString emcKeyFile = configMgr_->getConfigInfo("Arm/Setting/emcKeyFile", "0").toString();
    ui->emcKeyCheckBox->setCheckState((Qt::CheckState)emcKeyFile.toInt());
    emcKeyCheckBoxHandle((Qt::CheckState)emcKeyFile.toInt());

    EntryList entryList = recentUseTool_->getEntryList();
    ui->ipEdit->clear();
    ui->ipEdit->addItems(entryList);
    ui->ipEdit->setCurrentIndex(0);

    connect(ui->emcKeyCheckBox, &QCheckBox::stateChanged, this, &SettingDialog::emcKeyCheckBoxHandle);
    connect(ui->KeyCheckBox, &QCheckBox::stateChanged, this, &SettingDialog::keyCheckBoxHandle);
}


void SettingDialog::keyCheckBoxHandle(int check)
{
    if (check)
    {
        ui->passwordEdit->setEnabled(false);
        ui->emcKeyCheckBox->setEnabled(false);
        return;
    }
    ui->passwordEdit->setEnabled(true);
    ui->emcKeyCheckBox->setEnabled(true);
}

void SettingDialog::emcKeyCheckBoxHandle(int check)
{
    if (check)
    {
        QString emcRootKey = configMgr_->getConfigInfo("Arm/Setting/emcRootKeyPath").toString();
        QString keyFilePath = QDir::cleanPath(emcRootKey);
        ui->keyEdit->setText(keyFilePath);

        ui->userEdit->setText("root");
        ui->userEdit->setEnabled(false);
        ui->passwordEdit->setEnabled(false);
        ui->KeyCheckBox->setEnabled(false);
        ui->keyEdit->setEnabled(false);
        return;
    }

    QString keyFile = configMgr_->getConfigInfo("Arm/Setting/keyFilePath").toString();
    QString keyFilePath = QDir::cleanPath(keyFile);
    ui->keyEdit->setText(keyFilePath);

    ui->userEdit->setEnabled(true);
    ui->passwordEdit->setEnabled(true);
    ui->KeyCheckBox->setEnabled(true);
    ui->keyEdit->setEnabled(true);
}

void SettingDialog::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
                this, tr("Editor Path"), ui->lineEdit->text(), tr("*.exe"));
    if (!fileName.isEmpty())
    {
        ui->lineEdit->setText(QDir::cleanPath(fileName));
    }
}

void SettingDialog::on_buttonBox_accepted()
{
    QString editorPath = QDir::cleanPath(ui->lineEdit->text());
    configMgr_->setConfigInfo("Arm/Setting/EditorPath", editorPath);

    QString keyFilePath = QDir::cleanPath(ui->keyEdit->text());
    configMgr_->setConfigInfo("Arm/Setting/keyFilePath", keyFilePath);

    QString port = ui->portEdit->text();
    configMgr_->setConfigInfo("Arm/Setting/port", port);

    QString userName = ui->userEdit->text();
    configMgr_->setConfigInfo("Arm/Setting/userName", userName);

    QString password = ui->passwordEdit->text();
    configMgr_->setConfigInfo("Arm/Setting/password", password);

    QString timeOut = ui->timeoutEdit->text();
    configMgr_->setConfigInfo("Arm/Setting/timeOut", timeOut);

    Qt::CheckState state = ui->KeyCheckBox->checkState();
    configMgr_->setConfigInfo("Arm/Setting/keyFile", state);

    Qt::CheckState stateEmc = ui->emcKeyCheckBox->checkState();
    configMgr_->setConfigInfo("Arm/Setting/emcKeyFile", stateEmc);

    recentUseTool_->addEntry(ui->ipEdit->currentText());

    close();
}


void SettingDialog::on_keyFileBrower_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
                this, tr("Key File Path"), ui->keyEdit->text(), tr("*.*"));
    if (!fileName.isEmpty())
    {
        ui->keyEdit->setText(QDir::cleanPath(fileName));
    }
}
