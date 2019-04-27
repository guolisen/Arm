#include <QDebug>
#include <QFileDialog>
#include <QProcess>
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
    QString editorPath = configMgr_->getConfigInfo("Arm/Setting/EditorPath",
        "C:/Program Files (x86)/Notepad++/notepad++.exe").toString();
    ui->lineEdit->setText(editorPath);

    QString keyFilePath = configMgr_->getConfigInfo("Arm/Setting/keyFilePath", "").toString();
    ui->keyEdit->setText(keyFilePath);

    //IP
    EntryList entryList = recentUseTool_->getEntryList();
    ui->ipEdit->clear();
    ui->ipEdit->addItems(entryList);
    ui->ipEdit->setCurrentIndex(0);

    QString siteIp = configMgr_->getConfigInfo("Arm/Setting/siteIp", "").toString();
    ui->ipEdit->setCurrentText(siteIp);

    //port
    QString port = configMgr_->getConfigInfo("Arm/Setting/port", "22").toString();
    ui->portEdit->setText(port);

    //userName
    QString userName = configMgr_->getConfigInfo("Arm/Setting/userName", "c4dev").toString();
    ui->userEdit->setText(userName);

    //password
    QString password = configMgr_->getConfigInfo("Arm/Setting/password", "c4dev!").toString();
    ui->passwordLineEdit->setText(password);

    //timeOut
    QString timeOut = configMgr_->getConfigInfo("Arm/Setting/timeOut", "9999").toString();
    ui->timeoutEdit->setText(timeOut);

    //keyFile
    QString keyFile = configMgr_->getConfigInfo("Arm/Setting/keyFile", "0").toString();
    ui->KeyCheckBox->setCheckState((Qt::CheckState)keyFile.toInt());
    keyCheckBoxHandle((Qt::CheckState)keyFile.toInt());

    //emcKeyFile
    QString emcKeyFile = configMgr_->getConfigInfo("Arm/Setting/emcKeyFile", "0").toString();
    ui->emcKeyCheckBox->setCheckState((Qt::CheckState)emcKeyFile.toInt());
    emcKeyCheckBoxHandle((Qt::CheckState)emcKeyFile.toInt());

    connect(ui->emcKeyCheckBox, &QCheckBox::stateChanged, this, &SettingDialog::emcKeyCheckBoxHandle);
    connect(ui->KeyCheckBox, &QCheckBox::stateChanged, this, &SettingDialog::keyCheckBoxHandle);
}


void SettingDialog::keyCheckBoxHandle(int check)
{
    if (!ui->KeyCheckBox->isEnabled())
        return;
    if (check)
    {
        ui->emcKeyCheckBox->setEnabled(false);
        ui->passwordLineEdit->setEnabled(false);
        return;
    }

    ui->passwordLineEdit->setEnabled(true);
    ui->emcKeyCheckBox->setEnabled(true);
}

void SettingDialog::emcKeyCheckBoxHandle(int check)
{
    if (!ui->emcKeyCheckBox->isEnabled())
        return;
    if (check)
    {
        ui->userEdit->setEnabled(false);
        ui->passwordLineEdit->setEnabled(false);
        ui->KeyCheckBox->setEnabled(false);
        ui->keyEdit->setEnabled(false);
        ui->keyFileBrower->setEnabled(false);
        return;
    }

    ui->userEdit->setEnabled(true);
    ui->passwordLineEdit->setEnabled(true);
    ui->KeyCheckBox->setEnabled(true);
    ui->keyEdit->setEnabled(true);
    ui->keyFileBrower->setEnabled(true);
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

    QString siteIp = ui->ipEdit->currentText();
    configMgr_->setConfigInfo("Arm/Setting/siteIp", siteIp);

    QString port = ui->portEdit->text();
    configMgr_->setConfigInfo("Arm/Setting/port", port);

    QString userName = ui->userEdit->text();
    configMgr_->setConfigInfo("Arm/Setting/userName", userName);

    QString password = ui->passwordLineEdit->text();
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

void SettingDialog::on_pushButton_2_clicked()
{
    QProcess* proc = new QProcess(this);
    proc->start("cmd", {"/c", "ArmHelp.chm"});
    proc->waitForStarted(1000);
    proc->waitForFinished(1000);
    proc->close();
    delete proc;
}
