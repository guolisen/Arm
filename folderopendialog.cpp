#include <QFileDialog>
#include "folderopendialog.h"
#include "ui_folderopendialog.h"

FolderOpenDialog::FolderOpenDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FolderOpenDialog)
{
    ui->setupUi(this);
}

FolderOpenDialog::~FolderOpenDialog()
{
    delete ui;
}

void FolderOpenDialog::on_pushButton_clicked()
{
    QFileDialog* fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle("Choose Source Directory");
    //fd->setDirectory(buf);
    fileDialog->setFileMode(QFileDialog::DirectoryOnly);
    QStringList fileName;
    fileDialog->setOption(QFileDialog::ShowDirsOnly);

    if (fileDialog->exec() == QDialog::Accepted)
    {
        fileName = fileDialog->selectedFiles();
        filePath_ = fileName[0];
        ui->comboBox->setEditText(filePath_);
    }
}

void FolderOpenDialog::on_buttonBox_accepted()
{
    filePath_ = ui->comboBox->currentText();
    QFileInfo info(filePath_);
    if (!info.exists())
        filePath_ = "";
    close();
}
