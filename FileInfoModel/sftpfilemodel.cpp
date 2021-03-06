#include <QDebug>
#include <QBuffer>
#include "sftpfilemodel.h"
#include "logfilesystemmodel.h"
#include "sftpreadtimejob.h"
#include <Core/iconfigmgr.h>

namespace fileinfomodel {
typedef fileinfomodel::LogFileSystemModel<
            fileinfomodel::SftpFileModel> RemoteFileSystemType;
SftpFileModel::SftpFileModel(core::ContextPtr context, QAbstractItemModel* model, QObject* parent): IFileModel(parent),
    context_(context), model_(model),
    fileIdentifier_(context->getComponent<fileIdentifier::IFileIdentifier>(nullptr)),
    pool_(new QThreadPool(this)), currentJobsNum_(0), isDestroyed_(false)
{
    core::ConfigMgrPtr configMgrPtr = context_->getComponent<core::IConfigMgr>(nullptr);
    int threadNumber = configMgrPtr->getConfigInfo("Arm/Setting/threadNumber", 5).toInt();
    pool_->setMaxThreadCount(threadNumber);
}

SftpFileModel::~SftpFileModel()
{
    isDestroyed_ = true;
    pool_->clear();
    emit cancel();
}

QString SftpFileModel::getLogStartTimeStr(const QModelIndex &index)
{
    if (isDestroyed_)
        return "";
    RemoteFileSystemType* fileSystem = dynamic_cast<RemoteFileSystemType*>(model_);
    QSsh::SftpFileNode* fn = static_cast<QSsh::SftpFileNode *>(index.internalPointer());
    if (!fn || (QSsh::FileTypeDirectory == fn->fileInfo.type))
        return QString();

    auto fileTypeObj = fileIdentifier_->checkFileType(fn->fileInfo.name.toStdString());
    if(!fileTypeObj)
        return QString();

    auto cacheTime = fileSystem->findCache(fn->path);
    if (!cacheTime.isEmpty())
        return cacheTime;

    fileSystem->setCache(fn->path, "Loading");

    auto setCacheFunc = std::bind(&RemoteFileSystemType::setCache, fileSystem,
                                  std::placeholders::_1, std::placeholders::_2);
    SftpReadTimeJob* readTimeJobPtr = new SftpReadTimeJob(context_, model_,
                index, fn->path, fileTypeObj, setCacheFunc);

    connect(this, &SftpFileModel::cancel, readTimeJobPtr, &SftpReadTimeJob::cancel);
    connect(readTimeJobPtr, &SftpReadTimeJob::dataChanged, this,
            [this](const QModelIndex& index)
    {
        emit dataChanged(index);
        QMutexLocker lock(&jobNumMutex_);
        --currentJobsNum_;
    });

    pool_->start(readTimeJobPtr);
    {
        QMutexLocker lock(&jobNumMutex_);
        ++currentJobsNum_;
    }
    return "Loading";
}

void SftpFileModel::setCurrentDir(const QString &path, QTreeView *tree)
{
    RemoteFileSystemType* fileSystem = dynamic_cast<RemoteFileSystemType*>(model_);
    fileSystem->setRootDirectory(path);
}

}
