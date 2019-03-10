#ifndef FOLDEROPENDIALOG_H
#define FOLDEROPENDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class FolderOpenDialog;
}

class FolderOpenDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FolderOpenDialog(QWidget *parent = nullptr);
    ~FolderOpenDialog();

    QString getFileName() const
    {
        return filePath_;
    }
private slots:
    void on_pushButton_clicked();

    void on_buttonBox_accepted();

private:
    Ui::FolderOpenDialog *ui;
    QString filePath_;
};

#endif // FOLDEROPENDIALOG_H
