#ifndef SFTPFILEMODEL_H
#define SFTPFILEMODEL_H

#include <QAbstractItemModel>
#include <ssh/sftpfilesystemmodel.h>
#include "ifilemodel.h"

namespace fileinfomodel {

class SftpFileModel
{
public:
    SftpFileModel(QAbstractItemModel* model, QObject* parent);
};

template<>
struct FileModelTrait<SftpFileModel>
{
    typedef QSsh::SftpFileSystemModel ModelBaseType;
    const int logStartTimeColNum = 3;
};

}
#endif // SFTPFILEMODEL_H
