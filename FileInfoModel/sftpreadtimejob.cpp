#include <QBuffer>
#include <QFile>
#include <QDebug>
#include <QDate>
#include <QTime>
#include <QDateTime>
#include <quagzipfile.h>
#include <QEventLoop>
#include <QApplication>
#include <functional>
#include <quagzipfile.h>
#include "sftpreadtimejob.h"
#include <zlib.h>

namespace fileinfomodel
{

typedef LogFileSystemModel<SftpFileModel> RemoteFileSystemType;

static int gzdecompress(Byte *zdata, uLong nzdata,
                 Byte *data, uLong *ndata)
{
    int err = 0;
    z_stream d_stream = {0};
    static char dummy_head[2] = {
        0x8 + 0x7 * 0x10,
        (((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
    };
    d_stream.zalloc = NULL;
    d_stream.zfree = NULL;
    d_stream.opaque = NULL;
    d_stream.next_in = zdata;
    d_stream.avail_in = 0;
    d_stream.next_out = data;

    if (inflateInit2(&d_stream, MAX_WBITS + 16) != Z_OK) return -1;
    //if(inflateInit2(&d_stream, 47) != Z_OK) return -1;
    while (d_stream.total_out < *ndata && d_stream.total_in < nzdata) {
        d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
        if ((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) break;
        if (err != Z_OK) {
            if (err == Z_DATA_ERROR) {
                d_stream.next_in = (Bytef*)dummy_head;
                d_stream.avail_in = sizeof(dummy_head);
                if ((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK) {
                    return -1;
                }
            }
            else return -1;
        }
    }
    if (inflateEnd(&d_stream) != Z_OK) return -1;
    *ndata = d_stream.total_out;
    return 0;
}

SftpReadTimeJob::SftpReadTimeJob(QAbstractItemModel* model,
                                 const QModelIndex& index,
                                 const QString& fullFileName,
                                 fileIdentifier::FileObjectPtr typeObj,
                                 SetCacheCallBack setCache):
                                 model_(model), fullFileName_(fullFileName), typeObj_(typeObj),
                                 setCache_(setCache), index_(index), buffer_(new QBuffer()),
                                 currentId_(0)
{
    setAutoDelete(true);
}

bool SftpReadTimeJob::isCompressFile()
{
    if (fullFileName_.contains(".gz"))
        return true;
    return false;
}

void SftpReadTimeJob::connectHostProcess()
{
    qDebug() << "connectHostProcess!";
    buffer_->open(QBuffer::ReadWrite);

    QSsh::SftpFileNode* fileNode = static_cast<QSsh::SftpFileNode *>(index_.internalPointer());
    QMetaObject::invokeMethod(sftpMgr_, std::bind(&fileinfomodel::SftpMgr::download, sftpMgr_, fileNode->path, buffer_, 500));
}

void SftpReadTimeJob::handleSftpOperationFinished(QSsh::SftpJobId jobId, QString error)
{
    QString message1;
    QString s = QString::fromLatin1(buffer_->buffer());
    //message1 = tr("SftpReadTimeJob::handleSftpOperationFinished Result: %1").arg(s);
    qDebug() << "SftpReadTimeJob::handleSftpOperationFinished Result: " << s;

    std::string lineStr;
    if (isCompressFile())
    {
        QByteArray compressData = uncompressData();
        if (compressData.isEmpty())
        {
           qDebug() << "Uncompress error " << fullFileName_;
           emit jobfinished();
           return;
        }
        lineStr = compressData.toStdString();
    }
    else
    {
        lineStr = s.toStdString();
    }

    fileIdentifier::DateEntry dateEntry = typeObj_->getLineTime(lineStr);
    if (!dateEntry.exist)
    {
        lineStr = "No Time Info";
        setCache_(fullFileName_, QString::fromStdString(lineStr));
        emit jobfinished();
        return;
    }

    QString dataStr;
    QDate date(dateEntry.year, dateEntry.month, dateEntry.day);
    QTime time(dateEntry.hour, dateEntry.minute, dateEntry.second, dateEntry.mSecond/1000);
    QDateTime dt(date, time);
    dataStr = dt.toString(Qt::SystemLocaleLongDate);
    setCache_(fullFileName_, dataStr);
    emit jobfinished();
}

QByteArray SftpReadTimeJob::uncompressData()
{
    Bytef buf[2048] = {0};
    ulong srcSize = buffer_->data().size();
    const char* srcBuf = buffer_->data().data();
    ulong destSize = compressBound(srcSize);

    if(gzdecompress((Bytef*)srcBuf, srcSize, (Bytef*)buf, &destSize) != Z_OK)
    {
        qDebug() << "uncompress failed!";
        return QByteArray();
    }
    qDebug() << "UnCompress: " << (char*)buf;
    return QByteArray::fromRawData((const char *)buf, destSize);
}

void SftpReadTimeJob::run()
{
    QSsh::SshConnectionParameters sshParams;
    sshParams.host = "192.168.0.107";
    sshParams.userName = "guolisen";
    sshParams.authenticationType = QSsh::SshConnectionParameters::AuthenticationByPassword;
    //sshParams.privateKeyFile = "C:/Users/qq/.ssh/id_rsa";

    sshParams.password = "lifesgood";
    sshParams.port = 22;
    sshParams.timeout = 5000;

    sftpMgr_ = new SftpMgr(sshParams);
    //sftpMgr_->moveToThread(QApplication::instance()->thread());
    //RemoteFileSystemType* fileSystem = dynamic_cast<RemoteFileSystemType*>(model_);
    connect(sftpMgr_, &SftpMgr::connectHostSuccess, this, &SftpReadTimeJob::connectHostProcess);
    connect(sftpMgr_, &SftpMgr::jobFinished, this,
        &SftpReadTimeJob::handleSftpOperationFinished);

    sftpMgr_->startToConnect();

    QEventLoop loop;
    connect(this, &SftpReadTimeJob::jobfinished, &loop, &QEventLoop::quit);
    loop.exec();
    emit dataChanged(index_);
    sftpMgr_->disconnectToHost();
    qDebug() << "Thread Out!";
}
}
