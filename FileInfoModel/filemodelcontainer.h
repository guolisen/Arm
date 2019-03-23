#ifndef FILEMODELCONTAINER_H
#define FILEMODELCONTAINER_H

#include <QObject>
#include <ssh/sftpfilesystemmodel.h>

enum FileModelType
{
    LocalFileSystemModel,
    RemoteFileSystemModel
};

class LogFileSystemModel;
namespace QSsh {
class SftpFileSystemModel;
}

class FileModelContainer: public QObject
{
    Q_OBJECT
public:
    FileModelContainer(QObject* parent = nullptr);
    bool init();

    bool isRemote(const QString &path);
    QAbstractItemModel *setRootLocalPath(const QString &path);
    QAbstractItemModel *setRootRemotePath(const QString &path, const QString &host);

    QAbstractItemModel *getModel();
    QSsh::SftpFileSystemModel *getRemoteModel()
    {
        return remoteFSModel_;
    }
    LogFileSystemModel *getLocalModel()
    {
        return localFSModel_;
    }
Q_SIGNALS:
    void directoryLoadedWrapper(const QString &path);
    void sftpOperationFailed(const QString &errorMessage);
    void sftpOperationFinished(const QString &error);
    void connectionError(const QString &errorMessage);

private slots:
    void directoryLoaded(const QString &path);
    void operationFinished(QSsh::SftpJobId, const QString &error);
    void handleSftpOperationFailed(const QString &errorMessage);
    void handleSftpOperationFinished(QSsh::SftpJobId jobId, const QString &error);
    void handleConnectionError(const QString &errorMessage);
private:
    FileModelType currentModeType_;
    bool isRemoteConnected_;
    LogFileSystemModel* localFSModel_;
    QSsh::SftpFileSystemModel* remoteFSModel_;
    QAbstractItemModel* currentModel_;
};

#endif // FILEMODELCONTAINER_H
