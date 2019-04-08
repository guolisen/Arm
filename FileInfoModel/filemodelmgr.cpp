#include <QMessageBox>
#include <QDebug>
#include <QEventLoop>
#include <QProcess>
#include "logfilesystemmodel.h"
#include "filemodelmgr.h"
#include <ssh/sftpfilesystemmodel.h>
#include <ssh/sshconnection.h>
#include "logfilesystemmodel.h"
#include "localfilemodel.h"
#include "sftpfilemodel.h"
#include "FileInfoModel/uncompressfilecache.h"

namespace fileinfomodel
{
FileModelMgr::FileModelMgr(core::ContextPtr context, QObject* parent):  QObject(parent),
    currentModeType_(LocalFileSystemModel),
    isRemoteConnected_(false),
    localFSModel_(new LocalFileModelType(context, nullptr, this)),
    remoteFSModel_(new RemoteFileModelType(context, nullptr, this)),
    currentModel_(nullptr), context_(context), downloadId_(0)
{
    init();
}

void FileModelMgr::directoryLoaded(const QString &path)
{
    emit directoryLoadedWrapper(path);
}

bool FileModelMgr::init()
{
    connect(localFSModel_, &LocalFileModelType::directoryLoaded,
            this, [this](const QString& path)
    {
        directoryLoaded(path);
    });
#if 1
    connect(remoteFSModel_, &RemoteFileModelType::dataChanged,
            this, [this](const QModelIndex &srcindex, const QModelIndex &destindex)
    {
        directoryLoaded("");
    });
#endif
    connect(remoteFSModel_, SIGNAL(sftpOperationFinished(QSsh::SftpJobId,QString)),
        SLOT(handleSftpOperationFinished(QSsh::SftpJobId,QString)));
    connect(remoteFSModel_, SIGNAL(sftpOperationFailed(QString)), SLOT(handleSftpOperationFailed(QString)));
    connect(remoteFSModel_, SIGNAL(connectionError(QString)), SLOT(handleConnectionError(QString)));
    connect(remoteFSModel_, SIGNAL(connectionSuccess()), SLOT(handleConnectionSuccess()));

    return true;
}

void FileModelMgr::setRootPath(const QString &path, QTreeView* tree)
{
    if (isRemote(path))
    {
        setRootRemotePath(path, tree);
    }
    else
    {
        setRootLocalPath(path, tree);
    }
}

bool FileModelMgr::isRemote(const QString& path)
{
    if('/' == path[0])
    {
        currentModeType_ = RemoteFileSystemModel;
        return true;
    }

    currentModeType_ = LocalFileSystemModel;
    return false;
}

QAbstractItemModel* FileModelMgr::getModel()
{
    return currentModel_;
}

void FileModelMgr::setRootLocalPath(const QString& path, QTreeView* tree)
{
    currentModel_ = localFSModel_;
    tree->setModel(currentModel_);
    localFSModel_->setCurrentDir(path, tree);
}

void FileModelMgr::setRootRemotePath(const QString& path, QTreeView* tree)
{
    rootPath_ = path;
    treeView_ = tree;
    currentModel_ = remoteFSModel_;
    tree->setModel(currentModel_);

    if(!isRemoteConnected_)
    {
        QSsh::SshConnectionParameters sshParams;
        sshParams.host = "192.168.0.101";
        sshParams.userName = "guolisen";
        sshParams.authenticationType = QSsh::SshConnectionParameters::AuthenticationByPassword;
        //sshParams.privateKeyFile = "C:/Users/qq/.ssh/id_rsa";

        sshParams.password = "lifesgood";
        sshParams.port = 22;
        sshParams.timeout = 100;

        remoteFSModel_->setSshConnection(sshParams);
        isRemoteConnected_ = true;
    }
    else
    {
        remoteFSModel_->setCurrentDir(rootPath_, treeView_);
    }

    // /disks/USD_dumps21/ARs/0992000-0992999/992710
    qDebug() << "FileModelContainer::setRootRemotePath " << path;
}

void FileModelMgr::handleConnectionSuccess()
{
    qDebug() << "FileModelContainer::handleConnectionSuccess ";
    remoteFSModel_->setCurrentDir(rootPath_, treeView_);
}

void FileModelMgr::handleSftpOperationFailed(const QString &errorMessage)
{
    qDebug() << "FileModelContainer::handleSftpOperationFailed " << errorMessage;
    emit sftpOperationFailed(errorMessage);
}

void FileModelMgr::handleSftpOperationFinished(QSsh::SftpJobId jobId, const QString &error)
{
    QString message;
    if (error.isEmpty())
        message = tr("Operation %1 finished successfully.").arg(jobId);
    else
        message = tr("Operation %1 failed: %2.").arg(jobId).arg(error);

    qDebug() << "FileModelContainer::handleSftpOperationFinished " << message;

    if(downloadId_ == jobId)
    {
        qDebug() << "1downloadAsync OK! " << error;
        downloadError_ = error;
        emit downloadFinished();
        return;
    }

    directoryLoaded("");
}

void FileModelMgr::handleConnectionError(const QString &errorMessage)
{
    isRemoteConnected_ = false;
    qDebug() << "FileModelContainer::handleConnectionError " << errorMessage;
    emit connectionError(errorMessage);
}

int FileModelMgr::downloadAsync(const QModelIndex &index, const QString &targetFilePath)
{
    downloadId_ = remoteFSModel_->downloadFile(index, targetFilePath);
    QEventLoop loop;
    connect(this, &FileModelMgr::downloadFinished, &loop, &QEventLoop::quit);
    loop.exec();

    if (!downloadError_.isEmpty())
        return -1;
    return 0;
}

QString FileModelMgr::createCacheFile(const QModelIndex &index)
{
    QString localFile;
    UncompressFileCache fileCache;
    if (currentModeType_ == RemoteFileSystemModel)
    {
        const RemoteFileModelType* remoteModel =
                dynamic_cast<const RemoteFileModelType*>(index.model());
        QSsh::SftpFileNode* fn = static_cast<QSsh::SftpFileNode *>(index.internalPointer());
        localFile = fileCache.getCacheFileName(fn->fileInfo.name);
        if (downloadAsync(index, localFile) < 0)
        {
            return "";
        }

        qDebug() << "downloadAsync OK!";
    }
    else
    {
        LocalFileModelType* localModel = (LocalFileModelType*)index.model();
        localFile = localModel->filePath(index);
    }

    QString cacheFileName = fileCache.createUncompressCacheFile(localFile);
    if(cacheFileName.isEmpty())
       return "";
    return cacheFileName;
}

}
