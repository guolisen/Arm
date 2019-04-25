#ifndef FOLDEROPENDIALOG_H
#define FOLDEROPENDIALOG_H

#include <QDialog>
#include <QString>
#include <irecentusemgr.h>

namespace Ui {
class FolderOpenDialog;
}

class FolderOpenDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FolderOpenDialog(RecentUseMgrPtr recentUseTool, QWidget *parent = nullptr);
    ~FolderOpenDialog();

    QString getFileName() const;

private slots:
    void on_pushButton_clicked();
    void on_buttonBox_accepted();

private:
    Ui::FolderOpenDialog *ui;
    RecentUseMgrPtr recentUseTool_;
};

#endif // FOLDEROPENDIALOG_H
