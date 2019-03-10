#include <QDebug>
#include <QFileDialog>
#include "settingdialog.h"
#include "ui_settingdialog.h"

SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
    editorPath_ = settings_.value("Arm/Setting/EditorPath").toString();
    if (editorPath_.isEmpty())
        editorPath_ = QDir::cleanPath("C:/Program Files (x86)/Notepad++/notepad++.exe");
    ui->lineEdit->setText(editorPath_);
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

void SettingDialog::on_pushButton_clicked()
{
#if 0
    QFileDialog* fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle("Choose Editor Directory");
    //fd->setDirectory(buf);
    fileDialog->setFileMode(QFileDialog::ExistingFile);
    QStringList fileName;
    fileDialog->setDirectory(editorPath_);
    if (fileDialog->exec() == QDialog::Accepted)
    {
        fileName = fileDialog->selectedFiles();
        editorPath_ = QDir::cleanPath(fileName[0]);
        ui->lineEdit->setText(editorPath_);
        settings_.setValue("Setting/editorPath", editorPath_);
    }
#endif

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
    settings_.setValue("Arm/Setting/EditorPath", editorPath_);
    close();
}
