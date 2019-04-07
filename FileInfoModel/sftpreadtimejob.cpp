#include <QBuffer>
#include "sftpreadtimejob.h"

namespace fileinfomodel
{

typedef fileinfomodel::LogFileSystemModel<
            fileinfomodel::SftpFileModel> RemoteFileSystemType;

SftpReadTimeJob::SftpReadTimeJob(QAbstractItemModel* model,
                                 const QModelIndex& index,
                                 const QString& fullFileName,
                                 fileIdentifier::FileObjectPtr typeObj,
                                 SetCacheCallBack setCache):
                                 model_(model), fullFileName_(fullFileName), typeObj_(typeObj),
                                 setCache_(setCache), index_(index), buffer_(new QBuffer(this)),
                                 currentId_(0)
{
#if 0
    file_ = new QuaGzipFile(fullFileName_, this);
    if (!file_->open(QIODevice::ReadOnly))
    {
        qDebug() << "Can't open the file of" << fullFileName_;
        return;
    }
#endif




}

void SftpReadTimeJob::connectHostProcess()
{
    qDebug() << "connectHostProcess!";
    buffer_->open(QBuffer::ReadWrite);

    //RemoteFileSystemType* fileSystem = dynamic_cast<RemoteFileSystemType*>(model_);

    //currentId_ = fileSystem->downloadFile(index_, buffer_, 200);

    QSsh::SftpFileNode* fileNode = static_cast<QSsh::SftpFileNode *>(index_.internalPointer());

    QMetaObject::invokeMethod(sftpMgr_, std::bind(&fileinfomodel::SftpMgr::download, sftpMgr_, fileNode->path, buffer_, 200));
}

void SftpReadTimeJob::handleSftpOperationFinished(QSsh::SftpJobId jobId, QString error)
{
    qDebug() << tr("@!!SftpReadTimeJob::handleSftpOperationFinished %1 %2").arg(currentId_).arg(jobId);
    if (currentId_ != jobId)
        return;
    QString message1;
    QString s = QString::fromLatin1(buffer_->buffer());
    message1 = tr("SftpReadTimeJob::handleSftpOperationFinished Result: %1").arg(s);
    qDebug() << message1;
}

void SftpReadTimeJob::operator()(int)
{
    QSsh::SshConnectionParameters sshParams;
    sshParams.host = "192.168.0.105";
    sshParams.userName = "guolisen";
    sshParams.authenticationType = QSsh::SshConnectionParameters::AuthenticationByPassword;
    //sshParams.privateKeyFile = "C:/Users/qq/.ssh/id_rsa";

    sshParams.password = "lifesgood";
    sshParams.port = 22;
    sshParams.timeout = 5000;

    sftpMgr_ = new SftpMgr(sshParams);
    //RemoteFileSystemType* fileSystem = dynamic_cast<RemoteFileSystemType*>(model_);
    connect(sftpMgr_, &SftpMgr::connectHostSuccess, this, &SftpReadTimeJob::connectHostProcess);
    connect(sftpMgr_, &SftpMgr::jobFinished, this,
        &SftpReadTimeJob::handleSftpOperationFinished);

    sftpMgr_->startToConnect();


#if 0

    if (buffer_->waitForReadyRead(3000))
    {
        QString message1;
        QString s = QString::fromLatin1(buffer_->buffer());
        message1 = tr("!!!!!!!!!!!!Result: %1").arg(s);
        qDebug() << message1;
    }
#endif
#if 0
    QString lineStr;

    if(!readLine(lineStr) || lineStr.isEmpty())
    {
        lineStr = "No Time Info";
        setCache_(fullFileName_, lineStr);
        return;
    }

    fileIdentifier::DateEntry dateEntry = typeObj_->getLineTime(lineStr.toStdString());
    if (!dateEntry.exist)
        continue;

    QDate date(dateEntry.year, dateEntry.month, dateEntry.day);
    QTime time(dateEntry.hour, dateEntry.minute, dateEntry.second, dateEntry.mSecond/1000);
    QDateTime dt(date, time);
    lineStr = dt.toString(Qt::SystemLocaleLongDate);
    setCache_(fullFileName_, lineStr);
    emit dataChanged(index_, this);
    return;
#endif

}
}
