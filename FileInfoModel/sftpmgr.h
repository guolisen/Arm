#ifndef SFTPMGR_H
#define SFTPMGR_H

#include <ssh/sftpchannel.h>
#include <ssh/sshconnection.h>

#include <QElapsedTimer>
#include <QHash>
#include <QList>
#include <QObject>
#include <QSharedPointer>
#include <QWaitCondition>
#include <QMutex>

namespace fileinfomodel
{

class SftpMgr : public QObject
{
    Q_OBJECT

public:
    SftpMgr(QSsh::SshConnectionParameters sshParams, QObject *parent = nullptr);
    virtual ~SftpMgr() = default;
    void startToConnect();
    QSsh::SftpJobId download(const QString &remoteFilePath, QSharedPointer<QIODevice> localFile, quint32 size);
Q_SIGNALS:
    void connectHostSuccess();
    void jobFinished(QSsh::SftpJobId job, const QString &error);

private slots:
    void handleConnected();
    void handleError();
    void handleDisconnected();
    void handleChannelInitialized();
    void handleChannelInitializationFailure(const QString &reason);
    void handleJobFinished(QSsh::SftpJobId job, const QString &error);
    void handleFileInfo(QSsh::SftpJobId job, const QList<QSsh::SftpFileInfo> &fileInfoList);
    void handleChannelClosed();

private:
    QString remoteFilePath(const QString &localFileName) const;
    void earlyDisconnectFromHost();
    bool checkJobId(QSsh::SftpJobId job, QSsh::SftpJobId expectedJob, const char *activity);

    bool handleJobFinished(QSsh::SftpJobId job, QSsh::SftpJobId expectedJob, const QString &error,
        const char *activity);

    QSsh::SshConnectionParameters sshParams_;
    QSsh::SshConnection *m_connection;
    QSsh::SftpChannel::Ptr m_channel;
    QString m_remoteDirPath;
    QSsh::SftpFileInfo m_dirInfo;
    QWaitCondition cond_;
    QMutex sftpMutex_;
};

}
#endif // SFTPMGR_H
