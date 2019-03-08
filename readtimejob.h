#ifndef READTIMEJOB_H
#define READTIMEJOB_H

#include <functional>
#include <QString>
#include <FileIdentifier/ifileObject.h>

class QFile;
typedef std::function<void(QString, QString&)> SetCacheCallBack;
class ReadTimeJob
{
public:
    ReadTimeJob(const QString& fullFileName, fileIdentifier::FileObjectPtr typeObj,
                SetCacheCallBack setCache);
    ~ReadTimeJob();

    void operator()(int);

    bool readLine(QString &line);
    bool isCompressFile();

private:
    bool readCompressFileLine(QString &line);
    bool readFileLine(QString &line);
    QString fullFileName_;
    fileIdentifier::FileObjectPtr typeObj_;
    bool isCompressFile_;
    SetCacheCallBack setCache_;
    QFile* file_;
};

#endif // READTIMEJOB_H
