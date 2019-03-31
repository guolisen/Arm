#ifndef READTIMEJOB_H
#define READTIMEJOB_H

#include <functional>
#include <QObject>
#include <QString>
#include <QModelIndex>
#include <QFileSystemModel>
#include <FileIdentifier/ifileObject.h>
#include "ireadtimejob.h"

class QIODevice;
typedef std::function<void(QString, QString&)> SetCacheCallBack;
class LocalReadTimeJob: public QObject
{
    Q_OBJECT
public:
    LocalReadTimeJob(const QModelIndex& index,const QString& fullFileName, fileIdentifier::FileObjectPtr typeObj,
                SetCacheCallBack setCache);
    ~LocalReadTimeJob();

    void operator()(int);

    bool readLine(QString &line);

Q_SIGNALS:
    void dataChanged(const QModelIndex& index, LocalReadTimeJob* jobObj);

private:
    QString fullFileName_;
    fileIdentifier::FileObjectPtr typeObj_;
    SetCacheCallBack setCache_;
    QIODevice* file_;
    QModelIndex index_;
};

template<>
struct JobTrait<LocalReadTimeJob>
{
    typedef QFileSystemModel ModelBaseType;
    const int logStartTimeColNum = 4;
};

#endif // READTIMEJOB_H
