#ifndef READTIMEJOB_H
#define READTIMEJOB_H

#include <functional>
#include <QObject>
#include <QString>
#include <QModelIndex>
#include <FileIdentifier/ifileObject.h>

class QIODevice;
typedef std::function<void(QString, QString&)> SetCacheCallBack;
class ReadTimeJob: public QObject
{
    Q_OBJECT
public:
    ReadTimeJob(const QModelIndex& index,const QString& fullFileName, fileIdentifier::FileObjectPtr typeObj,
                SetCacheCallBack setCache);
    ~ReadTimeJob();

    void operator()(int);

    bool readLine(QString &line);
    bool isCompressFile();

Q_SIGNALS:
    void dataChanged(const QModelIndex& index, ReadTimeJob* jobObj);

private:
    QString fullFileName_;
    fileIdentifier::FileObjectPtr typeObj_;
    bool isCompressFile_;
    SetCacheCallBack setCache_;
    QIODevice* file_;
    QModelIndex index_;
};

#endif // READTIMEJOB_H
