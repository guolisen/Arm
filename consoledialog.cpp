#include "consoledialog.h"
#include "ui_consoledialog.h"

ConsoleDialog::ConsoleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConsoleDialog)
{
    ui->setupUi(this);
}

ConsoleDialog::~ConsoleDialog()
{
    delete ui;
}

void ConsoleDialog::setMessageToEditor(const QString &message)
{
    ui->textEdit->append(message);
}

void ConsoleDialog::on_pushButton_clicked()
{
    ui->textEdit->setText("");
}
