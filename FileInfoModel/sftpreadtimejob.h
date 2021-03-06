#ifndef SFTPREADTIMEJOB_H
#define SFTPREADTIMEJOB_H

#include <QObject>
#include <QBuffer>
#include <ssh/sftpfilesystemmodel.h>
#include "ireadtimejob.h"
#include <FileIdentifier/ifileObject.h>
#include "ireadtimejob.h"
#include "logfilesystemmodel.h"
#include "sftpfilemodel.h"
#include "sftpmgr.h"

class QEventLoop;
namespace fileinfomodel
{

typedef std::function<void(QString, QString)> SetCacheCallBack;
class SftpReadTimeJob : public QObject, public QRunnable
{
    Q_OBJECT
public:
    SftpReadTimeJob(core::ContextPtr context, QAbstractItemModel* model, const QModelIndex& index, const QString& fullFileName, fileIdentifier::FileObjectPtr typeObj,
                SetCacheCallBack setCache);
    ~SftpReadTimeJob();
    virtual void run();

public slots:
    void handleSftpOperationFinished(QSsh::SftpJobId jobId, QString error);
    void connectHostProcess();
    void cancel();
Q_SIGNALS:
    void dataChanged(const QModelIndex& index);
    void jobfinished();
private:
    bool isCompressFile();
    QByteArray uncompressData();

private:
    core::ContextPtr context_;
    QAbstractItemModel* model_;
    QString fullFileName_;
    fileIdentifier::FileObjectPtr typeObj_;
    SetCacheCallBack setCache_;
    QModelIndex index_;
    QSharedPointer<QBuffer> buffer_;
    QSsh::SftpJobId currentId_;
    SftpMgr* sftpMgr_;
    bool isDestroyed_;
    QEventLoop* loop_;
};

template<>
struct JobTrait<SftpReadTimeJob>
{
    typedef QSsh::SftpFileSystemModel ModelBaseType;
    const int logStartTimeColNum = 2;
};

}
#endif // SFTPREADTIMEJOB_H
