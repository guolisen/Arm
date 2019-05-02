#ifndef FILEMODELCONTAINER_H
#define FILEMODELCONTAINER_H

#include <QObject>
#include <QWaitCondition>
#include <ssh/sftpfilesystemmodel.h>
#include <Core/iconfigmgr.h>
#include <QTreeView>
#include <QEventLoop>
#include "logfilesystemmodel.h"
#include "localfilemodel.h"
#include "sftpfilemodel.h"

class QProgressDialog;
class SortFilterProxyModel;
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
    FileModelMgr(core::ContextPtr context, QWidget* parent = nullptr);
    virtual ~FileModelMgr();
    bool init();
    void setRootPath(const QString &path, QTreeView* tree);
    QString getRootPath() const
    {
        return rootPath_;
    }
    QAbstractItemModel *getModel();
    QString createCacheFile(const QModelIndex &index, QString& cacheFileName);
    FileModelType getCurrentModeType() const { return currentModeType_; }
    void update(const QModelIndex& index);
    void clearCache();
    void setNameFilter(const QString &s);
    QString downloadAsync(const QModelIndex &index, const QString &targetFilePath);
    QString uploadAsync(const QString &localFilePath, const QModelIndex &index);
    QString removeAsync(const QString &removeFilePath);
Q_SIGNALS:
    void directoryUpdateWrapper(const QModelIndex &index);
    void directoryLoadedWrapper(const QString &path);
    void sftpOperationFailed(const QString &errorMessage);
    void sftpOperationFinished(const QString &error);
    void connectionError(const QString &errorMessage);
    void transferFinished();

public slots:
    void directoryLoaded(const QString &path);
    void handleSftpOperationFailed(const QString &errorMessage);
    void handleSftpOperationFinished(QSsh::SftpJobId jobId, const QString &error);
    void handleConnectionError(const QString &errorMessage);
    void handleConnectionSuccess();
    void handleTransferPrograss(quint64 current, quint64 total);
private:
    bool isRemote(const QString &path);
    void setRootLocalPath(const QString& path, QTreeView* tree);
    void setRootRemotePath(const QString &path, QTreeView* tree);

    void createProgressBar();
    void createRemoteModel();
    void releaseRemoteModel();
private:
    QWidget* parentWin_;
    FileModelType currentModeType_;
    bool isRemoteConnected_;
    LocalFileModelType* localFSModel_;
    RemoteFileModelType* remoteFSModel_;
    QAbstractItemModel* currentModel_;
    core::ContextPtr context_;
    QString rootPath_;
    QTreeView* treeView_;
    QSsh::SftpJobId transferTaskId_;
    QString transferTaskError_;
    QProgressDialog* pd_;
    SortFilterProxyModel* proxyModel_;
    QEventLoop loop_;
};

}
#endif // FILEMODELCONTAINER_H
