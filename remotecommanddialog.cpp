#include "remotecommanddialog.h"
#include "ui_remotecommanddialog.h"

RemoteCommandDialog::RemoteCommandDialog(RecentUseMgrPtr recentUseTool, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RemoteCommandDialog),
    recentUseTool_(recentUseTool)
{
    ui->setupUi(this);
    setWindowTitle("Remote Command");
    Qt::WindowFlags flag = windowFlags();
    flag = flag & (~Qt::WindowContextHelpButtonHint);
    setWindowFlags(flag);
    EntryList entryList = recentUseTool_->getEntryList();
    ui->commandLine->addItems(entryList);
    ui->commandLine->setCurrentIndex(0);
}

RemoteCommandDialog::~RemoteCommandDialog()
{
    delete ui;
}

void RemoteCommandDialog::setCommand(const QString &command)
{
    ui->commandLine->setCurrentText(command);
}

QString RemoteCommandDialog::getCommand()
{
    return ui->commandLine->currentText();
}

void RemoteCommandDialog::on_buttonBox_accepted()
{
    recentUseTool_->addEntry(ui->commandLine->currentText());
}
