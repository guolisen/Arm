#include <QFile>
#include <QDebug>
#include <QDate>
#include <QTime>
#include <QDateTime>
#include "readtimejob.h"

ReadTimeJob::ReadTimeJob(const QString& fullFileName,
                         fileIdentifier::FileObjectPtr typeObj,
                         SetCacheCallBack setCache):
                         fullFileName_(fullFileName), typeObj_(typeObj),
                         isCompressFile_(isCompressFile()), setCache_(setCache),
                         file_(new QFile(fullFileName_))
{
    if (!file_->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Can't open the file of" << fullFileName_;
        return;
    }
}

ReadTimeJob::~ReadTimeJob()
{
    file_->close();
    delete file_;
}

void ReadTimeJob::operator()(int)
{
    QString lineStr;

    while (1)
    {
        if(!readLine(lineStr))
        {
            lineStr = "File is broken";
            setCache_(fullFileName_, lineStr);
            return;
        }

        fileIdentifier::DateEntry dateEntry = typeObj_->getLineTime(lineStr.toStdString());
        if (!dateEntry.exist)
            continue;

        QDate date(dateEntry.year, dateEntry.month, dateEntry.day);
        QTime time(dateEntry.hour, dateEntry.minute, dateEntry.second, dateEntry.mSecond);
        QDateTime dt(date, time);
        lineStr = dt.toString();
        setCache_(fullFileName_, lineStr);
        return;
    }
}

bool ReadTimeJob::readLine(QString& line)
{
    if (isCompressFile_)
        return readCompressFileLine(line);
    else
        return readFileLine(line);
}

bool ReadTimeJob::isCompressFile()
{
    return fullFileName_.contains("gz") ||
            fullFileName_.contains("zip") ||
            fullFileName_.contains("tgz");
}

bool  ReadTimeJob::readCompressFileLine(QString& line)
{
    return false;
}

bool ReadTimeJob::readFileLine(QString& line)
{
    line = "";

    char buf[10000];
    qint64 lineLength = file_->readLine(buf, sizeof(buf));
    if (lineLength == -1)
        return false;

    line = QString::fromLatin1(buf);
    return true;
}
