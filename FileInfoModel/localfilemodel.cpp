#include <QFileInfo>
#include <QTreeView>
#include "logfilesystemmodel.h"
#include "localfilemodel.h"
#include "localreadtimejob.h"

namespace fileinfomodel
{
typedef fileinfomodel::LogFileSystemModel<
            fileinfomodel::LocalFileModel> LocalFileSystemType;
LocalFileModel::LocalFileModel(core::ContextPtr context, QAbstractItemModel* model, QObject* parent): IFileModel(parent),
    context_(context), model_(model),
    fileIdentifier_(context->getComponent<fileIdentifier::IFileIdentifier>(nullptr)),
    pool_(context->getComponent<core::IThreadPool>(nullptr))
{

}

static void threadWrapper(unsigned int id, std::shared_ptr<LocalReadTimeJob> job)
{
    (*job)(id);
}

QString LocalFileModel::getLogStartTimeStr(const QModelIndex &index)
{
    LocalFileSystemType* fileSystem = dynamic_cast<LocalFileSystemType*>(model_);
    QFileInfo fi = fileSystem->fileInfo(index);
    if (!fi.isFile())
        return QString();

    auto fileTypeObj = fileIdentifier_->checkFileType(fi.fileName().toStdString());
    if(!fileTypeObj)
        return QString();

    auto cacheTime = fileSystem->findCache(fi.filePath());
    if (!cacheTime.isEmpty())
        return cacheTime;

    fileSystem->setCache(fi.filePath(), "Loading");

    auto setCacheFunc = std::bind(&LocalFileSystemType::setCache, fileSystem,
                                  std::placeholders::_1, std::placeholders::_2);
    std::shared_ptr<LocalReadTimeJob> readTimeJobPtr = std::make_shared<LocalReadTimeJob>(
                index, fi.filePath(), fileTypeObj, setCacheFunc);

    connect(readTimeJobPtr.get(), &LocalReadTimeJob::dataChanged, this,
            [this](const QModelIndex& index, LocalReadTimeJob* jobObj){ emit dataChanged(index); });
    pool_->attach(std::bind(&threadWrapper, std::placeholders::_1, readTimeJobPtr), 1);

    return "Loading";
}

void LocalFileModel::setCurrentDir(const QString &path, QTreeView *tree)
{
    LocalFileSystemType* fileSystem = dynamic_cast<LocalFileSystemType*>(model_);
    fileSystem->setRootPath(path);

    QModelIndex rootIndex = fileSystem->index(QDir::cleanPath(path));
    tree->setRootIndex(rootIndex);
    tree->update();
}

}
