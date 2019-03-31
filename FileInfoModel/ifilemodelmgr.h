#ifndef IFILEMODELMGR_H
#define IFILEMODELMGR_H

#ifndef FILEMODELCONTAINER_H
#define FILEMODELCONTAINER_H

#include <QObject>
#include <ssh/sftpfilesystemmodel.h>
#include <Core/iconfigmgr.h>
#include <QTreeView>

#if 0
enum FileModelType
{
    LocalFileSystemModel,
    RemoteFileSystemModel
};

class FileModelMgr: public QObject
{
    Q_OBJECT
public:
    FileModelMgr(core::ConfigMgrPtr config, QObject* parent = nullptr);
    void setRootPath(const QString &path, QTreeView* tree);
    QAbstractItemModel *getModel();

    void directoryLoaded(const QString &path);
    void operationFinished(QSsh::SftpJobId, const QString &error);
    void handleSftpOperationFailed(const QString &errorMessage);
    void handleSftpOperationFinished(QSsh::SftpJobId jobId, const QString &error);
    void handleConnectionError(const QString &errorMessage);
};
#endif
#endif // FILEMODELCONTAINER_H


#endif // IFILEMODELMGR_H
