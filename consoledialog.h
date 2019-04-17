#ifndef CONSOLEDIALOG_H
#define CONSOLEDIALOG_H

#include <QDialog>

namespace Ui {
class ConsoleDialog;
}

class ConsoleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConsoleDialog(QWidget *parent = nullptr);
    ~ConsoleDialog();

    void setMessageToEditor(const QString& message);
    void appendMessageToEditor(const QString &message);
private slots:
    void on_pushButton_clicked();

private:
    Ui::ConsoleDialog *ui;
};

#endif // CONSOLEDIALOG_H
