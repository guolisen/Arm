#ifndef ARMWINDOW_H
#define ARMWINDOW_H

#include <QDebug>
#include <QMenu>
#include <QMainWindow>
#include <Core/appcontext.h>
#include <FileInfoModel/filemodelmgr.h>
#include <QSettings>
#include "FileInfoModel/remoteprocess.h"
#include <irecentusemgr.h>

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
    explicit ArmWindow(core::ContextPtr context, IRecentUseMgr::Factory recentUseFactory, QWidget *parent = nullptr);
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
    bool requestRemoteCommand(const QString& defaultCommand);
    void on_treeView_doubleClicked(const QModelIndex &index);
    void on_treeView_customContextMenuRequested(const QPoint &pos);
    void handleStdOut(QByteArray data);
    void updateColumn(const QModelIndex& index);
    void unCompressRemoteFile();
    void handleClosed(int exitStatus);
    void downloadFile();
private:
    void createMenu();
    RemoteProcess* createRemoteProcess();
    void executeRemoteCommand(const QString& command);
    void downloadFile(const QModelIndex &index);
    void uploadFile(const QModelIndex &index);

    Ui::ArmWindow *ui;
    core::ContextPtr context_;
    IRecentUseMgr::Factory recentUseFactory_;
    fileinfomodel::FileModelMgr* modelMgr_;
    QString editorPath_;
    QMenu* rightPopMenu_;
    RemoteProcess* remoteProcess_;
    ConsoleDialog* consoleDialog_;
    core::ConfigMgrPtr configMgrPtr_;
    bool isNeedUpdate_;
};

#endif // ARMWINDOW_H
