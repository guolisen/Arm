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
#include <QProgressDialog>
#include <FileInfoModel/sortfilterproxymodel.h>

namespace fileinfomodel
{
FileModelMgr::FileModelMgr(core::ContextPtr context, QObject* parent):  QObject(parent),
    currentModeType_(LocalFileSystemModel),
    isRemoteConnected_(false),
    localFSModel_(new LocalFileModelType(context, nullptr, this)),
    remoteFSModel_(nullptr),
    currentModel_(nullptr), context_(context), downloadId_(0), rootPath_("/")
{
    init();
}

FileModelMgr::~FileModelMgr()
{
    remoteFSModel_->shutDown();
}

void FileModelMgr::directoryLoaded(const QString &path)
{
    emit directoryLoadedWrapper(path);
}

void FileModelMgr::createProgressBar()
{
    pd_ = new QProgressDialog("", "", 0, 100);
    pd_->reset();
    pd_->setWindowModality(Qt::WindowModal);
    pd_->setMinimumDuration(5);
    pd_->setWindowTitle("Downloading...");
    pd_->setAutoClose(true);
    pd_->setModal(true);
    pd_->setCancelButton(nullptr);
    //pd_->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    pd_->setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);
}

void FileModelMgr::createRemoteModel()
{
    if (!remoteFSModel_)
    {
        remoteFSModel_ = new RemoteFileModelType(context_, nullptr, this);
    }

    proxyModel_ = new SortFilterProxyModel(this);
    proxyModel_->setSourceModel(remoteFSModel_);
    proxyModel_->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel_->setFilterKeyColumns(1);
    proxyModel_->clearFilter();
    proxyModel_->setRecursiveFilteringEnabled(true);

    connect(remoteFSModel_->getModel(), &IFileModel::dataChanged,
            this, [this](const QModelIndex &index)
    {
        emit directoryUpdateWrapper(index);
    });
    connect(remoteFSModel_, &QAbstractItemModel::layoutChanged,
            this, [this]()
    {
        emit directoryLoadedWrapper("");
    });

    connect(remoteFSModel_, SIGNAL(sftpOperationFinished(QSsh::SftpJobId,QString)),
        SLOT(handleSftpOperationFinished(QSsh::SftpJobId,QString)));
    connect(remoteFSModel_, SIGNAL(sftpOperationFailed(QString)), SLOT(handleSftpOperationFailed(QString)));
    connect(remoteFSModel_, SIGNAL(connectionError(QString)), SLOT(handleConnectionError(QString)));
    connect(remoteFSModel_, SIGNAL(connectionSuccess()), SLOT(handleConnectionSuccess()));
    connect(remoteFSModel_, SIGNAL(downloadPrograss(quint64, quint64)), SLOT(handleDownloadPrograss(quint64, quint64)));
}

void FileModelMgr::setNameFilter(const QString& s)
{
    proxyModel_->setRecursiveFilteringEnabled(true);
    if (s.isEmpty())
    {
        proxyModel_->clearFilter();
        proxyModel_->setFilterRegExp(QRegExp("", proxyModel_->filterCaseSensitivity(), QRegExp::Wildcard));
        return;
    }

    QString str = "*" + s + "*";
    proxyModel_->addFilterFixedString(str);
    proxyModel_->setFilterRegExp(QRegExp(str, proxyModel_->filterCaseSensitivity(), QRegExp::Wildcard));
}

bool FileModelMgr::init()
{
    connect(localFSModel_, &LocalFileModelType::directoryLoaded,
            this, [this](const QString& path)
    {
        emit directoryLoadedWrapper(path);
    });
    createRemoteModel();
    createProgressBar();

    return true;
}
void FileModelMgr::handleDownloadPrograss(quint64 current, quint64 total)
{
    //qDebug() << "handleDownloadPrograss " << current << " " << total;
    pd_->setValue(current);
}

void FileModelMgr::setRootPath(const QString &path, QTreeView* tree)
{
    rootPath_ = path;
    treeView_ = tree;
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

void FileModelMgr::update(const QModelIndex& index)
{
    if(currentModeType_ == RemoteFileSystemModel)
    {
        if (!remoteFSModel_->getCurrentJobNum())
            remoteFSModel_->update(index);
    }
}

void FileModelMgr::clearCache()
{
    remoteFSModel_->clearCache();
    localFSModel_->clearCache();
}

void FileModelMgr::setRootLocalPath(const QString& path, QTreeView* tree)
{
    currentModel_ = localFSModel_;
    tree->setModel(currentModel_);
    localFSModel_->setCurrentDir(path, tree);
}

void FileModelMgr::releaseRemoteModel()
{
    if (remoteFSModel_)
    {
        delete remoteFSModel_;
        remoteFSModel_ = nullptr;
        isRemoteConnected_ = false;
    }
}

void FileModelMgr::setRootRemotePath(const QString& path, QTreeView* tree)
{

    tree->setModel(nullptr);
    releaseRemoteModel();
    createRemoteModel();
    currentModel_ = remoteFSModel_;
    tree->setModel(proxyModel_);
    core::ConfigMgrPtr config = context_->getComponent<core::IConfigMgr>(nullptr);
    QSsh::SshConnectionParameters sshParams = config->getSshParameters();

    if(!isRemoteConnected_)
    {
        remoteFSModel_->shutDown();
        remoteFSModel_->setSshConnection(sshParams);
    }
    else
    {
        remoteFSModel_->setCurrentDir(rootPath_, treeView_);
    }

    qDebug() << "FileModelContainer::setRootRemotePath " << path;
}

void FileModelMgr::handleConnectionSuccess()
{
    qDebug() << "FileModelContainer::handleConnectionSuccess ";
    isRemoteConnected_ = true;
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

    qDebug() << "FileModelMgr::handleSftpOperationFinished " << message;

    if(downloadId_ == jobId)
    {
        qDebug() << "1downloadAsync OK! " << error;
        pd_->hide();
        downloadError_ = error;
        downloadId_ = 0;
        emit downloadFinished();
        return;
    }

    emit sftpOperationFinished(error);
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
        if (fn && fn->fileInfo.type == QSsh::FileTypeDirectory)
            return "";
        pd_->setMaximum(fn->fileInfo.size);
        pd_->setMinimum(0);
        pd_->setValue(0);
        pd_->setLabelText(fn->fileInfo.name);
        pd_->show();
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
        QFileInfo fi = localModel->fileInfo(index);
        if (fi.isDir())
            return "";
        localFile = localModel->filePath(index);
    }

    QString cacheFileName = fileCache.createUncompressCacheFile(localFile);
    if(cacheFileName.isEmpty())
       return "";
    return cacheFileName;
}

}
