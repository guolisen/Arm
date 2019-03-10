#include <QFileDialog>
#include <QSettings>
#include "folderopendialog.h"
#include "ui_folderopendialog.h"

FolderOpenDialog::FolderOpenDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FolderOpenDialog)
{
    ui->setupUi(this);
    QSettings setting;
    filePath_ = setting.value("Arm/Setting/DefaultLogPath").toString();
    ui->comboBox->setEditText(filePath_);
}

FolderOpenDialog::~FolderOpenDialog()
{
    delete ui;
}

void FolderOpenDialog::on_pushButton_clicked()
{
#if 0
    QFileDialog* fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle("Choose Source Directory");
    //fd->setDirectory(buf);
    fileDialog->setFileMode(QFileDialog::DirectoryOnly);
    QStringList fileName;
    fileDialog->setOption(QFileDialog::ShowDirsOnly);
    fileDialog->setDirectory(defaultLogPath_);
    if (fileDialog->exec() == QDialog::Accepted)
    {
        fileName = fileDialog->selectedFiles();
        filePath_ = fileName[0];
        ui->comboBox->setEditText(filePath_);
    }
#endif

    QString logPath = QFileDialog::getExistingDirectory(
                this, tr("Editor Path"), filePath_);
    if (!logPath.isEmpty())
    {
        filePath_ = QDir::cleanPath(logPath);
        ui->comboBox->setEditText(filePath_);
    }
}

void FolderOpenDialog::on_buttonBox_accepted()
{
    filePath_ = ui->comboBox->currentText();
    QFileInfo info(filePath_);
    if (!info.exists())
        filePath_ = "";
    QSettings setting;
    setting.setValue("Arm/Setting/DefaultLogPath", filePath_);
    close();
}
