#ifndef REMOTECOMMANDDIALOG_H
#define REMOTECOMMANDDIALOG_H

#include <QDialog>
#include <irecentusemgr.h>

namespace Ui {
class RemoteCommandDialog;
}

class RemoteCommandDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RemoteCommandDialog(RecentUseMgrPtr recentUseTool, QWidget *parent = nullptr);
    ~RemoteCommandDialog();

    void setCommand(const QString& command);
    QString getCommand();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::RemoteCommandDialog *ui;
    RecentUseMgrPtr recentUseTool_;
};

#endif // REMOTECOMMANDDIALOG_H
