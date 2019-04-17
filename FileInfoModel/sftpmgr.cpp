#include <QDebug>
#include <QApplication>
#include "sftpmgr.h"

namespace fileinfomodel
{

SftpMgr::SftpMgr(QSsh::SshConnectionParameters sshParams, QObject *parent) : QObject(parent),
    sshParams_(sshParams)
{
}

SftpMgr::~SftpMgr()
{
    disconnectToHost();
}

void SftpMgr::startToConnect()
{
    m_connection = new QSsh::SshConnection(sshParams_);
    connect(m_connection, SIGNAL(connected()), SLOT(handleConnected()));
    connect(m_connection, SIGNAL(error(QSsh::SshError)), SLOT(handleError()));
    connect(m_connection, SIGNAL(disconnected()), SLOT(handleDisconnected()));
    qDebug() << "Connecting to host '"
        << qPrintable(sshParams_.host) << "'...";

    m_connection->connectToHost();
    //sftpMutex_.lock();
    //cond_.wait(&sftpMutex_);
    //sftpMutex_.unlock();
}

QSsh::SftpJobId SftpMgr::download(const QString &remoteFilePath, QSharedPointer<QIODevice> localFile, quint32 size)
{
    QSsh::SftpJobId id = m_channel->downloadFile(remoteFilePath, localFile, size);
    return id;
}

void SftpMgr::disconnectToHost()
{
    earlyDisconnectFromHost();
}

void SftpMgr::handleConnected()
{
    qDebug() << "Connected. Initializing SFTP channel...";
    m_channel = m_connection->createSftpChannel();
    connect(m_channel.data(), SIGNAL(initialized()), this,
       SLOT(handleChannelInitialized()));
    connect(m_channel.data(), SIGNAL(initializationFailed(QString)), this,
        SLOT(handleChannelInitializationFailure(QString)));
    connect(m_channel.data(), SIGNAL(finished(QSsh::SftpJobId,QString)),
        this, SLOT(handleJobFinished(QSsh::SftpJobId,QString)));
    connect(m_channel.data(),
        SIGNAL(fileInfoAvailable(QSsh::SftpJobId,QList<QSsh::SftpFileInfo>)),
        SLOT(handleFileInfo(QSsh::SftpJobId,QList<QSsh::SftpFileInfo>)));
    connect(m_channel.data(), SIGNAL(closed()), this,
        SLOT(handleChannelClosed()));

    m_channel->initialize();
}

void SftpMgr::handleError()
{
    qDebug() << "Encountered SSH error: "
        << qPrintable(m_connection->errorString()) << ".";
    cond_.notify_all();
}

void SftpMgr::handleDisconnected()
{
    qDebug() << "Connection closed.";
    cond_.notify_all();
}

void SftpMgr::handleChannelInitialized()
{
    qDebug() << "Initialized OK.";
    cond_.notify_all();
    emit connectHostSuccess();
}

void SftpMgr::handleChannelInitializationFailure(const QString &reason)
{
    qDebug() << "Could not initialize SFTP channel: " << qPrintable(reason) << ".";
    cond_.notify_all();
    earlyDisconnectFromHost();
}

void SftpMgr::handleJobFinished(QSsh::SftpJobId job, const QString &error)
{
    qDebug() << "handleJobFinished ";
    emit jobFinished(job, error);
    cond_.notify_all();
}

void SftpMgr::handleFileInfo(QSsh::SftpJobId job, const QList<QSsh::SftpFileInfo> &fileInfoList)
{
    qDebug() << "handleFileInfo ";
    cond_.notify_all();
}

void SftpMgr::handleChannelClosed()
{
    qDebug() << "SFTP channel closed. Now disconnecting...";
    cond_.notify_all();
    m_connection->disconnectFromHost();
}

void SftpMgr::earlyDisconnectFromHost()
{
    qDebug() << "earlyDisconnectFromHost ";
    if (m_channel)
    {
        m_channel->closeChannel();
        disconnect(m_channel.data(), 0, this, 0);
    }
    m_connection->disconnectFromHost();
}

}
