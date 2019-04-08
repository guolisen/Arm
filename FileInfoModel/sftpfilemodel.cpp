#include <QDebug>
#include <QBuffer>
#include "sftpfilemodel.h"
#include "logfilesystemmodel.h"
#include "sftpreadtimejob.h"

namespace fileinfomodel {
typedef fileinfomodel::LogFileSystemModel<
            fileinfomodel::SftpFileModel> RemoteFileSystemType;
SftpFileModel::SftpFileModel(core::ContextPtr context, QAbstractItemModel* model, QObject* parent): IFileModel(parent),
    context_(context), model_(model),
    fileIdentifier_(context->getComponent<fileIdentifier::IFileIdentifier>(nullptr)),
    pool_(new QThreadPool(this))
{
    pool_->setMaxThreadCount(5);
}

QString SftpFileModel::getLogStartTimeStr(const QModelIndex &index)
{
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
    SftpReadTimeJob* readTimeJobPtr = new SftpReadTimeJob(model_,
                index, fn->path, fileTypeObj, setCacheFunc);

    connect(readTimeJobPtr, &SftpReadTimeJob::dataChanged, this,
            [this](const QModelIndex& index){ emit dataChanged(index); });

    pool_->start(readTimeJobPtr);
    return "Loading";
}

void SftpFileModel::setCurrentDir(const QString &path, QTreeView *tree)
{
    RemoteFileSystemType* fileSystem = dynamic_cast<RemoteFileSystemType*>(model_);
    fileSystem->setRootDirectory(path);
}

}
