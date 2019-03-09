#ifndef READTIMEJOB_H
#define READTIMEJOB_H

#include <functional>
#include <QString>
#include <FileIdentifier/ifileObject.h>

class QIODevice;
typedef std::function<void(QString, QString&)> SetCacheCallBack;
typedef std::function<void()> EmitDataChangeCallBack;
class ReadTimeJob
{
public:
    ReadTimeJob(const QString& fullFileName, fileIdentifier::FileObjectPtr typeObj,
                SetCacheCallBack setCache, EmitDataChangeCallBack emitDataChange);
    ~ReadTimeJob();

    void operator()(int);

    bool readLine(QString &line);
    bool isCompressFile();

private:
    QString fullFileName_;
    fileIdentifier::FileObjectPtr typeObj_;
    bool isCompressFile_;
    SetCacheCallBack setCache_;
    EmitDataChangeCallBack emitDataChange_;
    QIODevice* file_;
};

#endif // READTIMEJOB_H
