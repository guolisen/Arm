#ifndef FILEMODELCONTAINER_H
#define FILEMODELCONTAINER_H

#include <QObject>
#include <ssh/sftpfilesystemmodel.h>
#include <Core/iconfigmgr.h>
#include <QTreeView>

enum FileModelType
{
    LocalFileSystemModel,
    RemoteFileSystemModel
};

class LogFileSystemModel;
namespace QSsh {
class SftpFileSystemModel;
}

class FileModelMgr: public QObject
{
    Q_OBJECT
public:
    FileModelMgr(core::ConfigMgrPtr config, QObject* parent = nullptr);
    bool init();
    void setRootPath(const QString &path, QTreeView* tree);
    QAbstractItemModel *getModel();

Q_SIGNALS:
    void directoryLoadedWrapper(const QString &path);
    void sftpOperationFailed(const QString &errorMessage);
    void sftpOperationFinished(const QString &error);
    void connectionError(const QString &errorMessage);

private slots:
    void directoryLoaded(const QString &path);
    void operationFinished(QSsh::SftpJobId, const QString &error);
    void handleSftpOperationFailed(const QString &errorMessage);
    void handleSftpOperationFinished(QSsh::SftpJobId jobId, const QString &error);
    void handleConnectionError(const QString &errorMessage);

private:
    bool isRemote(const QString &path);
    void setRootLocalPath(const QString& path, QTreeView* tree);
    void setRootRemotePath(const QString &path, QTreeView* tree);
    void setModelToTree(const QString& path, QTreeView* tree);
private:
    FileModelType currentModeType_;
    bool isRemoteConnected_;
    LogFileSystemModel* localFSModel_;
    QSsh::SftpFileSystemModel* remoteFSModel_;
    QAbstractItemModel* currentModel_;
    core::ConfigMgrPtr config_;

};

#endif // FILEMODELCONTAINER_H
