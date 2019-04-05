#include "sftpfilemodel.h"
#include "logfilesystemmodel.h"
//#include "localreadtimejob.h"

namespace fileinfomodel {
typedef fileinfomodel::LogFileSystemModel<
            fileinfomodel::SftpFileModel> RemoteFileSystemType;
SftpFileModel::SftpFileModel(core::ContextPtr context, QAbstractItemModel* model, QObject* parent): IFileModel(parent),
    context_(context), model_(model),
    fileIdentifier_(context->getComponent<fileIdentifier::IFileIdentifier>(nullptr)),
    pool_(context->getComponent<core::IThreadPool>(nullptr))
{

}

QString SftpFileModel::getLogStartTimeStr(const QModelIndex &index)
{
    return "";
}

void SftpFileModel::setCurrentDir(const QString &path, QTreeView *tree)
{
    RemoteFileSystemType* fileSystem = dynamic_cast<RemoteFileSystemType*>(model_);
    fileSystem->setRootDirectory(path);
}

}
