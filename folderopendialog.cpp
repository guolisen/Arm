#include <QFileDialog>
#include <QSettings>
#include "folderopendialog.h"
#include "ui_folderopendialog.h"
#include <irecentusemgr.h>

FolderOpenDialog::FolderOpenDialog(RecentUseMgrPtr recentUseTool, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FolderOpenDialog),
    recentUseTool_(recentUseTool)
{
    ui->setupUi(this);
    QSettings setting;
    //filePath_ = setting.value("Arm/Setting/DefaultLogPath").toString();
    Qt::WindowFlags flag = windowFlags();
    flag = flag & (~Qt::WindowContextHelpButtonHint);
    setWindowFlags(flag);
    //ui->comboBox->setEditText(filePath_);
    EntryList entryList = recentUseTool_->getEntryList();
    ui->comboBox->addItems(entryList);
    ui->comboBox->setCurrentIndex(0);
}

FolderOpenDialog::~FolderOpenDialog()
{
    delete ui;
}

QString FolderOpenDialog::getFileName() const
{
    return ui->comboBox->currentText();
}

void FolderOpenDialog::on_pushButton_clicked()
{
    QString logPath = QFileDialog::getExistingDirectory(
                this, tr("Editor Path"), ui->comboBox->currentText());
    if (!logPath.isEmpty())
    {
        QString filePath = QDir::cleanPath(logPath);
        ui->comboBox->setCurrentText(filePath);
    }
}

void FolderOpenDialog::on_buttonBox_accepted()
{
    recentUseTool_->addEntry(ui->comboBox->currentText());
    close();
}
