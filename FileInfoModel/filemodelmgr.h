#ifndef FILEMODELCONTAINER_H
#define FILEMODELCONTAINER_H

#include <QObject>
#include <QWaitCondition>
#include <ssh/sftpfilesystemmodel.h>
#include <Core/iconfigmgr.h>
#include <QTreeView>
#include "logfilesystemmodel.h"
#include "localfilemodel.h"
#include "sftpfilemodel.h"

class QProgressDialog;
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
    virtual ~FileModelMgr();
    bool init();
    void setRootPath(const QString &path, QTreeView* tree);
    QString getRootPath() const
    {
        return rootPath_;
    }
    QAbstractItemModel *getModel();
    QString createCacheFile(const QModelIndex &index);
    FileModelType getCurrentModeType() const { return currentModeType_; }
    void update(const QModelIndex& index);
    void clearCache();
Q_SIGNALS:
    void directoryUpdateWrapper(const QModelIndex &index);
    void directoryLoadedWrapper(const QString &path);
    void sftpOperationFailed(const QString &errorMessage);
    void sftpOperationFinished(const QString &error);
    void connectionError(const QString &errorMessage);
    void downloadFinished();

public slots:
    void directoryLoaded(const QString &path);
    void handleSftpOperationFailed(const QString &errorMessage);
    void handleSftpOperationFinished(QSsh::SftpJobId jobId, const QString &error);
    void handleConnectionError(const QString &errorMessage);
    void handleConnectionSuccess();
    void handleDownloadPrograss(quint64 current, quint64 total);
private:
    bool isRemote(const QString &path);
    void setRootLocalPath(const QString& path, QTreeView* tree);
    void setRootRemotePath(const QString &path, QTreeView* tree);

    int downloadAsync(const QModelIndex &index, const QString &targetFilePath);
    void createProgressBar();
private:
    FileModelType currentModeType_;
    bool isRemoteConnected_;
    LocalFileModelType* localFSModel_;
    RemoteFileModelType* remoteFSModel_;
    QAbstractItemModel* currentModel_;
    core::ContextPtr context_;
    QString rootPath_;
    QTreeView* treeView_;
    QSsh::SftpJobId downloadId_;
    QString downloadError_;
    QProgressDialog* pd_;
    void createRemoteModel();
    void releaseRemoteModel();
};

}
#endif // FILEMODELCONTAINER_H
