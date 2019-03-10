#include <memory>
#include "FileIdentifier/ifileObject.h"
#include <FileIdentifier/fileidentifier.h>
#include <FileIdentifier/filetypecontainer.h>
#include <FileIdentifier/luafileobjectwrapper.h>
#include "Core/ThreadPool.h"
#include "Core/detail/Thread.h"
#include "Script/IScriptCenter.h"
#include "Script/ScriptCenter.h"
#include "Script/detail/ScriptCenterImpl.h"
#include <QDebug>
#include "logfilesystemmodel.h"
#include "readtimejob.h"
#include <QMutex>

LogFileSystemModel::~LogFileSystemModel()
{
    pool_->complete(-1);
}

QVariant LogFileSystemModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
    {
        return QFileSystemModel::data(index, role);
    }
    if(index.column() == columnCount() - 1)
    {
        switch(role)
        {
           case(Qt::DisplayRole):
               return logStartTime(index, fileInfo(index));
           case(Qt::TextAlignmentRole):
               return Qt::AlignLeft;
           default:
               break;
        }
    }
    return QFileSystemModel::data(index,role);
}

void LogFileSystemModel::init()
{
    pool_ = std::make_shared<core::ThreadPool>(
                core::detail::Thread::getFactory());
    pool_->createThreads(5);

    script::ScriptCenterPtr scriptCenter =
        std::make_shared<script::ScriptCenter>(
                std::make_shared<script::detail::ScriptCenterImpl>());
    scriptCenter->initialize();

    fileIdentifier::FileTypeContainerPtr container =
            std::make_shared<fileIdentifier::FileTypeContainer>(
                fileIdentifier::LuaFileObjectWrapper::getFactory(), scriptCenter);

    QDir currentFolder("FileTypeScript");
    currentFolder.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks | QDir::AllDirs);
    const QFileInfoList list = currentFolder.entryInfoList();
    Q_FOREACH(const QFileInfo fileInfo, list)
    {
        qDebug() << "current : " << fileInfo.fileName();
        QString fileName = fileInfo.filePath();
        if (fileName.contains(".lua"))
            container->registerLuaFileType(fileName.toStdString());
    }

    fileIdentifier_ = std::make_shared<fileIdentifier::FileIdentifier>(container);

    setReadOnly(true);
}

static void threadWrapper(unsigned int id, std::shared_ptr<ReadTimeJob> job)
{
    (*job)(id);
}

void LogFileSystemModel::dataTriger(const QModelIndex &index, ReadTimeJob* jobObj)
{
    emit dataChanged(index, index);
    disconnect(jobObj, &ReadTimeJob::dataChanged, this, &LogFileSystemModel::dataTriger);
}

QString LogFileSystemModel::logStartTime(const QModelIndex &index, const QFileInfo &fi) const
{
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
    std::shared_ptr<ReadTimeJob> readTimeJobPtr =
            std::make_shared<ReadTimeJob>(index, fi.filePath(), fileTypeObj,
                                          setCacheFunc);
    connect(readTimeJobPtr.get(), &ReadTimeJob::dataChanged, this, &LogFileSystemModel::dataTriger);
    pool_->attach(std::bind(threadWrapper, std::placeholders::_1, readTimeJobPtr), 1);

    return "Loading";
}
