#ifndef FILEMODELCONTAINER_H
#define FILEMODELCONTAINER_H

#include <QObject>
#include <ssh/sftpfilesystemmodel.h>
#include <Core/iconfigmgr.h>
#include <QTreeView>
#include "logfilesystemmodel.h"
#include "localfilemodel.h"
#include "sftpfilemodel.h"

namespace fileinfomodel
{

enum FileModelType
{
    LocalFileSystemModel,
    RemoteFileSystemModel
};

typedef LogFileSystemModel<LocalFileModel> LocalFileModelType;
typedef LogFileSystemModel<SftpFileModel> RemoteFileModelType;

class FileModelMgr: public QObject
{
    Q_OBJECT
public:
    FileModelMgr(core::ContextPtr context, QObject* parent = nullptr);
    bool init();
    void setRootPath(const QString &path, QTreeView* tree);
    QAbstractItemModel *getModel();

Q_SIGNALS:
    void directoryLoadedWrapper(const QString &path);
    void sftpOperationFailed(const QString &errorMessage);
    void sftpOperationFinished(const QString &error);
    void connectionError(const QString &errorMessage);

public slots:
    void directoryLoaded(const QString &path);
    void handleSftpOperationFailed(const QString &errorMessage);
    void handleSftpOperationFinished(QSsh::SftpJobId jobId, const QString &error);
    void handleConnectionError(const QString &errorMessage);
    void handleConnectionSuccess();

private:
    bool isRemote(const QString &path);
    void setRootLocalPath(const QString& path, QTreeView* tree);
    void setRootRemotePath(const QString &path, QTreeView* tree);

private:
    FileModelType currentModeType_;
    bool isRemoteConnected_;
    LocalFileModelType* localFSModel_;
    RemoteFileModelType* remoteFSModel_;
    QAbstractItemModel* currentModel_;
    core::ContextPtr context_;
    QString rootPath_;
    QTreeView* treeView_;
};

}
#endif // FILEMODELCONTAINER_H
