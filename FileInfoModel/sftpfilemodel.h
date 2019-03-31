#ifndef SFTPFILEMODEL_H
#define SFTPFILEMODEL_H

#include <QAbstractItemModel>
#include <ssh/sftpfilesystemmodel.h>
#include "ifilemodel.h"
#include <Core/appcontext.h>

namespace fileinfomodel {

class SftpFileModel: public IFileModel
{
public:
    SftpFileModel(core::ContextPtr context, QAbstractItemModel* model, QObject* parent);

    virtual QString getLogStartTimeStr(const QModelIndex &index);
};

template<>
struct FileModelTrait<SftpFileModel>
{
    typedef QSsh::SftpFileSystemModel ModelBaseType;
    const int logStartTimeColNum = 3;
};

}
#endif // SFTPFILEMODEL_H
