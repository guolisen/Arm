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

namespace fileinfomodel
{

class QIODevice;
typedef std::function<void(QString, QString)> SetCacheCallBack;
class SftpReadTimeJob : public QObject
{
    Q_OBJECT
public:
    SftpReadTimeJob(QAbstractItemModel* model, const QModelIndex& index, const QString& fullFileName, fileIdentifier::FileObjectPtr typeObj,
                SetCacheCallBack setCache);
    void operator ()(int)
    bool readLine(QString &line);

public slots:
    void handleSftpOperationFinished(QSsh::SftpJobId jobId, QString error);
    void connectHostProcess();
Q_SIGNALS:
    void dataChanged(const QModelIndex& index);

private:
    QAbstractItemModel* model_;
    QString fullFileName_;
    fileIdentifier::FileObjectPtr typeObj_;
    SetCacheCallBack setCache_;
    QIODevice* file_;
    QModelIndex index_;
    QSharedPointer<QBuffer> buffer_;
    QSsh::SftpJobId currentId_;
    SftpMgr* sftpMgr_;
};

template<>
struct JobTrait<SftpReadTimeJob>
{
    typedef QSsh::SftpFileSystemModel ModelBaseType;
    const int logStartTimeColNum = 2;
};

}
#endif // SFTPREADTIMEJOB_H
