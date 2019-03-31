#include "localfilemodel.h"

namespace fileinfomodel
{

LocalFileModel::LocalFileModel(QAbstractItemModel* model, QObject* parent): IFileModel(parent),
    model_(model)
{

}

#if 0
void threadWrapper(unsigned int id, std::shared_ptr<JobT> job)
{
    (*job)(id);
}
#endif

QString LocalFileModel::getLogStartTimeStr(const QModelIndex &index)
{
#if 0
    if (!fi.isFile())
        return QString();

    auto fileTypeObj = fileIdentifier_->checkFileType(fi.fileName().toStdString());
    if(!fileTypeObj)
        return QString();

    auto cacheTime = findCache(fi.filePath());
    if (!cacheTime.isEmpty())
        return cacheTime;

    setCache(fi.filePath(), "Loading");
    auto setCacheFunc = std::bind(&LogFileSystemModel::setCache, this,
                                  std::placeholders::_1, std::placeholders::_2);
    std::shared_ptr<JobT> readTimeJobPtr =
            std::make_shared<JobT>(index, fi.filePath(), fileTypeObj,
                                          setCacheFunc);
    connect(readTimeJobPtr.get(), &JobT::dataChanged, this, &LogFileSystemModel::dataTrigger);
    pool_->attach(std::bind(LogFileSystemModel<JobT>::threadWrapper, std::placeholders::_1, readTimeJobPtr), 1);
#endif
    return "Loading";
}

}
