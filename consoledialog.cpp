#include "consoledialog.h"
#include "ui_consoledialog.h"

ConsoleDialog::ConsoleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConsoleDialog)
{
    ui->setupUi(this);
    Qt::WindowFlags flag = windowFlags();
    flag = flag & (~Qt::WindowContextHelpButtonHint);
    setWindowFlags(flag);
    //ui->textEdit->setAcceptRichText(true);
    //ui->textEdit->setAutoFormatting(QTextEdit::AutoAll);
    ui->textEdit->setReadOnly(true);
}

ConsoleDialog::~ConsoleDialog()
{
    delete ui;
}

void ConsoleDialog::setMessageToEditor(const QString &message)
{
    ui->textEdit->setPlainText(message);
    ui->textEdit->moveCursor(QTextCursor::End);
}

void ConsoleDialog::on_pushButton_clicked()
{
    ui->textEdit->setPlainText("");
}
