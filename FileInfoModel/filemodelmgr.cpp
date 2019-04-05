#include <QMessageBox>
#include <QDebug>
#include "logfilesystemmodel.h"
#include "filemodelmgr.h"
#include <ssh/sftpfilesystemmodel.h>
#include <ssh/sshconnection.h>
#include "logfilesystemmodel.h"
#include "localfilemodel.h"
#include "sftpfilemodel.h"

FileModelMgr::FileModelMgr(core::ContextPtr context, QObject* parent):  QObject(parent),
    currentModeType_(LocalFileSystemModel),
    isRemoteConnected_(false),
    localFSModel_(new LocalFileModelType(context, nullptr, this)),
    remoteFSModel_(new RemoteFileModelType(context, nullptr, this)),
    currentModel_(nullptr), context_(context)
{
    init();
}

void FileModelMgr::directoryLoaded(const QString &path)
{
    emit directoryLoadedWrapper(path);
}

void FileModelMgr::operationFinished(QSsh::SftpJobId, const QString &error)
{
    emit directoryLoadedWrapper("");
}

bool FileModelMgr::init()
{
    //localFSModel_->init();

    connect(localFSModel_, &LocalFileModelType::directoryLoaded,
            this, [this](const QString& path)
    {
        directoryLoaded(path);
    });
    //connect(remoteFSModel_, &QSsh::SftpFileSystemModel::sftpOperationFinished,
    //        this, &FileModelMgr::operationFinished);

    return true;
}

void FileModelMgr::setRootPath(const QString &path, QTreeView* tree)
{
    if (isRemote(path))
        setRootRemotePath(path, tree);
    else
        setRootLocalPath(path, tree);
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
    localFSModel_->setRootPath(path);
    currentModel_ = localFSModel_;
    setModelToTree(path, tree);
}

void FileModelMgr::setModelToTree(const QString& path, QTreeView* tree)
{
    tree->setModel(currentModel_);
    QModelIndex rootIndex;
    rootIndex = localFSModel_->index(QDir::cleanPath(path));
    tree->setRootIndex(rootIndex);
    tree->update();
}

void FileModelMgr::setRootRemotePath(const QString& path, QTreeView* tree)
{
#if 0
    if(!isRemoteConnected_)
    {
        QSsh::SshConnectionParameters sshParams;
        sshParams.host = "192.168.0.102";
        sshParams.userName = "guolisen";
        sshParams.authenticationType = QSsh::SshConnectionParameters::AuthenticationByPassword;
        //sshParams.privateKeyFile = "C:/Users/qq/.ssh/id_rsa";
        //
        sshParams.password = "lifesgood";
        sshParams.port = 22;
        sshParams.timeout = 100;

        connect(remoteFSModel_, SIGNAL(sftpOperationFailed(QString)),
            SLOT(handleSftpOperationFailed(QString)));
        connect(remoteFSModel_, SIGNAL(connectionError(QString)), SLOT(handleConnectionError(QString)));
        connect(remoteFSModel_, SIGNAL(sftpOperationFinished(QSsh::SftpJobId,QString)),
            SLOT(handleSftpOperationFinished(QSsh::SftpJobId,QString)));
        remoteFSModel_->setSshConnection(sshParams);
        isRemoteConnected_ = true;
    }
    else {
        // /disks/USD_dumps21/ARs/0992000-0992999/992710
       //remoteFSModel_->setRootDirectory(path);
    }

    currentModel_ = remoteFSModel_;
    setModelToTree(path, tree);
    qDebug() << "FileModelContainer::setRootRemotePath " << path;

#endif
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

    emit sftpOperationFinished(message);
}

void FileModelMgr::handleConnectionError(const QString &errorMessage)
{
    isRemoteConnected_ = false;
    qDebug() << "FileModelContainer::handleConnectionError " << errorMessage;
    emit connectionError(errorMessage);
}
