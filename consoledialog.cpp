#include <QDebug>
#include "consoledialog.h"
#include "ui_consoledialog.h"

ConsoleDialog::ConsoleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConsoleDialog), size_(0)
{
    ui->setupUi(this);
    Qt::WindowFlags flag = windowFlags();
    flag = flag & (~Qt::WindowContextHelpButtonHint);
    setWindowFlags(flag);
    //ui->textEdit->setAcceptRichText(true);
    //ui->textEdit->setAutoFormatting(QTextEdit::AutoAll);
    //ui->textEdit->setReadOnly(true);

    ui->textArea->setMarginWidth(0, 0);
}

ConsoleDialog::~ConsoleDialog()
{
    delete ui;
}

void ConsoleDialog::setMessageToEditor(const QString &message)
{
    ui->textArea->setText(message);
    size_ = 0;
    //ui->textArea->moveCursorPosition(TextCursor::End, TextCursor::KeepAnchor);
}
void ConsoleDialog::appendMessageToEditor(const QString &message)
{
    //qDebug() << message;
    size_ += message.length() + 1000;
    ui->textArea->append(message);

    int line = -1;
    int index = -1;
    ui->textArea->lineIndexFromPosition(size_, &line, &index);
    ui->textArea->setCursorPosition(line, 0);
    //ui->textArea->moveCursorPosition(TextCursor::End, TextCursor::KeepAnchor);
    //ui->textArea->textCursor().deletePreviousChar();

    //ui->textEdit->append(message);
    //ui->textEdit->moveCursor(QTextCursor::End);
    //ui->textEdit->textCursor().deletePreviousChar();
}
void ConsoleDialog::on_pushButton_clicked()
{
    //ui->textEdit->setPlainText("");
}

