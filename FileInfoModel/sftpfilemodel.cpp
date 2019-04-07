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
    pool_(context->getComponent<core::IThreadPool>(nullptr)), buffer_(new QBuffer(this))
{

}

static void threadWrapper(unsigned int id, std::shared_ptr<SftpReadTimeJob> job)
{
    (*job)(id);
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

    //buffer_->open(QBuffer::ReadWrite);
    //fileSystem->downloadFile(index, buffer_, 200);

#if 0
    //Sleep(4000);
    //if (buffer_->waitForReadyRead(3000))
    {
        QString message1;
        QString s = QString::fromLatin1(buffer_->buffer());
        message1 = tr("!!!!!!!!!!!!Result: %1").arg(s);
        qDebug() << message1;
    }
#endif
#if 1
    auto setCacheFunc = std::bind(&RemoteFileSystemType::setCache, fileSystem,
                                  std::placeholders::_1, std::placeholders::_2);
    std::shared_ptr<SftpReadTimeJob> readTimeJobPtr = std::make_shared<SftpReadTimeJob>(model_,
                index, fn->path, fileTypeObj, setCacheFunc);

    connect(readTimeJobPtr.get(), &SftpReadTimeJob::dataChanged, this,
            [this](const QModelIndex& index){ emit dataChanged(index); });
    pool_->attach(std::bind(&threadWrapper, std::placeholders::_1, readTimeJobPtr), 1);
#endif
    return "Loading";
}

void SftpFileModel::setCurrentDir(const QString &path, QTreeView *tree)
{
    RemoteFileSystemType* fileSystem = dynamic_cast<RemoteFileSystemType*>(model_);
    fileSystem->setRootDirectory(path);
}

}
