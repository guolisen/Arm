#ifndef SFTPFILEMODEL_H
#define SFTPFILEMODEL_H

#include <QAbstractItemModel>
#include <QThreadPool>
#include <ssh/sftpfilesystemmodel.h>
#include "FileIdentifier/ifileObject.h"
#include <FileIdentifier/fileidentifier.h>
#include <FileIdentifier/filetypecontainer.h>
#include <FileIdentifier/luafileobjectwrapper.h>
#include "Core/ThreadPool.h"
#include "Core/appcontext.h"
#include "Core/detail/Thread.h"
#include "Script/IScriptCenter.h"
#include "Script/ScriptCenter.h"
#include "Script/detail/ScriptCenterImpl.h"
#include "ifilemodel.h"

namespace fileinfomodel {

class SftpFileModel: public IFileModel
{
    Q_OBJECT
public:
    SftpFileModel(core::ContextPtr context, QAbstractItemModel* model, QObject* parent);
    virtual ~SftpFileModel();
    virtual QString getLogStartTimeStr(const QModelIndex &index);
    virtual void setCurrentDir(const QString& path, QTreeView* tree);
    virtual int getCurrentJobNum()
    {
        return currentJobsNum_;
    }
Q_SIGNALS:
    void cancel();
private:
    core::ContextPtr context_;
    QAbstractItemModel* model_;
    fileIdentifier::FileIdentifierPtr fileIdentifier_;
    QThreadPool* pool_;
    QMutex jobNumMutex_;
    int currentJobsNum_;
    bool isDestroyed_;
};

template<>
struct FileModelTrait<SftpFileModel>
{
    typedef QSsh::SftpFileSystemModel ModelBaseType;
    const int logStartTimeColNum = 3;
};

}
#endif // SFTPFILEMODEL_H
