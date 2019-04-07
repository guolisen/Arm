#include <QMessageBox>
#include <QDebug>
#include "logfilesystemmodel.h"
#include "filemodelmgr.h"
#include <ssh/sftpfilesystemmodel.h>
#include <ssh/sshconnection.h>
#include "logfilesystemmodel.h"
#include "localfilemodel.h"
#include "sftpfilemodel.h"

namespace fileinfomodel
{
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

bool FileModelMgr::init()
{
    connect(localFSModel_, &LocalFileModelType::directoryLoaded,
            this, [this](const QString& path)
    {
        directoryLoaded(path);
    });

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
        sshParams.host = "192.168.0.105";
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

    directoryLoaded("");
}

void FileModelMgr::handleConnectionError(const QString &errorMessage)
{
    isRemoteConnected_ = false;
    qDebug() << "FileModelContainer::handleConnectionError " << errorMessage;
    emit connectionError(errorMessage);
}
}
