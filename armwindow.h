#ifndef ARMWINDOW_H
#define ARMWINDOW_H

#include <QDebug>
#include <QMainWindow>
#include <Core/appcontext.h>

namespace Ui {
class ArmWindow;
}

class QProcess;
class QSettings;
class FileModelMgr;
class ArmWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit ArmWindow(core::ContextPtr context, QWidget *parent = nullptr);
    ~ArmWindow();
    void init();
public slots:
    void handleSftpOperationFailed(const QString &errorMessage);
    void handleSftpOperationFinished(const QString &error);
    void handleConnectionError(const QString &errorMessage);

private slots:
    void open();

    void findStringProcess(const QString &s);
    void resizeColumn(const QString &path);
    void on_treeView_doubleClicked(const QModelIndex &index);
    void setting();

private:
    void createMenu();

    Ui::ArmWindow *ui;
    core::ContextPtr context_;
    FileModelMgr* modelMgr_;
    QString editorPath_;
};

#endif // ARMWINDOW_H
