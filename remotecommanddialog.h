#ifndef REMOTECOMMANDDIALOG_H
#define REMOTECOMMANDDIALOG_H

#include <QDialog>

namespace Ui {
class RemoteCommandDialog;
}

class RemoteCommandDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RemoteCommandDialog(QWidget *parent = nullptr);
    ~RemoteCommandDialog();

    void setCommand(const QString& command);
    QString getCommand();

private:
    Ui::RemoteCommandDialog *ui;
};

#endif // REMOTECOMMANDDIALOG_H
