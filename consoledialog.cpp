#include <QDebug>
#include <Qsci/qscilexer.h>
#include <Qsci/qscilexerbash.h>
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

    ui->textArea->setReadOnly(true);
    QsciLexer* lexer = new QsciLexerBash();
    ui->textArea->setLexer(lexer);

    QFont font;
    font.fromString("Courier New");
    font.setPointSize(9);
    lexer->setFont(font);

    QColor color;
    color.setNamedColor("#CCCCCC");
    lexer->setColor(color, -1);
    QColor errorColor;
    errorColor.setNamedColor("Red");
    lexer->setColor(errorColor, 1);

    QColor stringColor;
    stringColor.setNamedColor("Green");
    lexer->setColor(stringColor, 5);
    lexer->setColor(stringColor, 6);

    QColor bgcolor;
    bgcolor.setNamedColor("black");
    lexer->setPaper(bgcolor);
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

