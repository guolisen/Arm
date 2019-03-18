#include <QMessageBox>
#include <QDebug>
#include "logfilesystemmodel.h"
#include "filemodelcontainer.h"

#include <ssh/sftpfilesystemmodel.h>
#include <ssh/sshconnection.h>

FileModelContainer::FileModelContainer(QObject* parent):  QObject(parent),
    currentModeType_(LocalFileSystemModel),
    isRemoteConnected_(false), currentModel_(nullptr)
{

}

void FileModelContainer::directoryLoaded(const QString &path)
{
    emit directoryLoadedWrapper(path);
}

void FileModelContainer::operationFinished(QSsh::SftpJobId, const QString &error)
{
    emit directoryLoadedWrapper("");
}

bool FileModelContainer::init()
{
    localFSModel_ = new LogFileSystemModel(this);
    remoteFSModel_ = new QSsh::SftpFileSystemModel(this);

    localFSModel_->init();

    connect(localFSModel_, &LogFileSystemModel::directoryLoaded,
            this, &FileModelContainer::directoryLoaded);
    connect(remoteFSModel_, &QSsh::SftpFileSystemModel::sftpOperationFinished,
            this, &FileModelContainer::operationFinished);

    return true;
}

bool FileModelContainer::isRemote(const QString& path)
{
    if('/' == path[0])
    {
        currentModeType_ = RemoteFileSystemModel;
        return true;
    }

    currentModeType_ = LocalFileSystemModel;
    return false;
}

QAbstractItemModel* FileModelContainer::getModel()
{
    return currentModel_;
}

QAbstractItemModel* FileModelContainer::setRootLocalPath(const QString& path)
{
    localFSModel_->setRootPath(path);
    currentModel_ = localFSModel_;
    return localFSModel_;
}

QAbstractItemModel *FileModelContainer::setRootRemotePath(const QString &path, const QString &host)
{
    if(!isRemoteConnected_)
    {
        QSsh::SshConnectionParameters sshParams;
        sshParams.host = host;
        sshParams.userName = "root";
        sshParams.authenticationType = QSsh::SshConnectionParameters::AuthenticationByKey;
        sshParams.privateKeyFile = "C:/Users/guol13/.ssh/id_rsa";
        //
        //sshParams.password = m_ui->passwordLineEdit->text();
        sshParams.port = 22;
        sshParams.timeout = 10;

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
        remoteFSModel_->setRootDirectory(path);
    }

    currentModel_ = remoteFSModel_;
    qDebug() << "FileModelContainer::setRootRemotePath " << path;
    return remoteFSModel_;
}

void FileModelContainer::handleSftpOperationFailed(const QString &errorMessage)
{
    qDebug() << "FileModelContainer::handleSftpOperationFailed " << errorMessage;
    emit sftpOperationFailed(errorMessage);
}

void FileModelContainer::handleSftpOperationFinished(QSsh::SftpJobId jobId, const QString &error)
{
    QString message;
    if (error.isEmpty())
        message = tr("Operation %1 finished successfully.").arg(jobId);
    else
        message = tr("Operation %1 failed: %2.").arg(jobId).arg(error);

    qDebug() << "FileModelContainer::handleSftpOperationFinished " << message;

    emit sftpOperationFinished(message);
}

void FileModelContainer::handleConnectionError(const QString &errorMessage)
{
    isRemoteConnected_ = false;
    qDebug() << "FileModelContainer::handleConnectionError " << errorMessage;
    emit connectionError(errorMessage);
}
