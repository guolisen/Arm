#ifndef LOCALFILEMODEL_H
#define LOCALFILEMODEL_H

#include <QString>
#include <QFileSystemModel>
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

namespace fileinfomodel
{
class LocalFileModel: IFileModel
{
public:
    LocalFileModel(QAbstractItemModel* model, QObject* parent = nullptr);

    virtual QString getLogStartTimeStr(const QModelIndex &index);

private:
    QAbstractItemModel* model_;
    fileIdentifier::FileIdentifierPtr fileIdentifier_;
    core::ThreadPoolPtr pool_;
};

template<>
struct FileModelTrait<LocalFileModel>
{
    typedef QFileSystemModel ModelBaseType;
    const int logStartTimeColNum = 4;
};

}
#endif // LOCALFILEMODEL_H
