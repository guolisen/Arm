#include "remotecommanddialog.h"
#include "ui_remotecommanddialog.h"

RemoteCommandDialog::RemoteCommandDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RemoteCommandDialog)
{
    ui->setupUi(this);
}

RemoteCommandDialog::~RemoteCommandDialog()
{
    delete ui;
}

void RemoteCommandDialog::setCommand(const QString &command)
{
    ui->lineEdit->setText(command);
}

QString RemoteCommandDialog::getCommand()
{
    return ui->lineEdit->text();
}
