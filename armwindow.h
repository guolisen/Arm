#ifndef ARMWINDOW_H
#define ARMWINDOW_H

#include <QDebug>
#include <QMenu>
#include <QMainWindow>
#include <Core/appcontext.h>
#include <FileInfoModel/filemodelmgr.h>
#include <QSettings>
#include "FileInfoModel/remoteprocess.h"

namespace Ui {
class ArmWindow;
}

class QProcess;
class QSettings;
class ConsoleDialog;
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
    void handleReadyRead(QByteArray data);
    void findStringProcess(const QString &s);
    void resizeColumn(const QString &path);
    void setting();
    void createPopMenu();
    void uncompressInRemote();
    void on_treeView_doubleClicked(const QModelIndex &index);
    void on_treeView_customContextMenuRequested(const QPoint &pos);
    void handleStdOut(QByteArray data);
    void updateColumn(const QModelIndex& index);
private:
    void createMenu();
    void createRemoteProcess();

    Ui::ArmWindow *ui;
    core::ContextPtr context_;
    fileinfomodel::FileModelMgr* modelMgr_;
    QString editorPath_;
    QMenu* rightPopMenu_;
    RemoteProcess* remoteProcess_;
    ConsoleDialog* consoleDialog_;
    core::ConfigMgrPtr configMgrPtr_;
};

#endif // ARMWINDOW_H
