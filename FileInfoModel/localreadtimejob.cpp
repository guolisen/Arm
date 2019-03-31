#include <QFile>
#include <QDebug>
#include <QDate>
#include <QTime>
#include <QDateTime>
#include <quagzipfile.h>
#include "localreadtimejob.h"

LocalReadTimeJob::LocalReadTimeJob(const QModelIndex& index,
                         const QString& fullFileName,
                         fileIdentifier::FileObjectPtr typeObj,
                         SetCacheCallBack setCache):
                         fullFileName_(fullFileName), typeObj_(typeObj),
                         setCache_(setCache), index_(index)
{
    file_ = new QuaGzipFile(fullFileName_, this);
    if (!file_->open(QIODevice::ReadOnly))
    {
        qDebug() << "Can't open the file of" << fullFileName_;
        return;
    }
}

LocalReadTimeJob::~LocalReadTimeJob()
{
    //if(file_->isOpen())
     //   file_->close();
}

void LocalReadTimeJob::operator()(int)
{
    QString lineStr;
    while (1)
    {
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
    }
}

bool LocalReadTimeJob::readLine(QString& line)
{
    line = "";
    char buf[10000] = {0};
    qint64 lineLength = file_->readLine(buf, sizeof(buf));
    if (lineLength == -1)
        return false;

    line = QString::fromLatin1(buf);
    return true;
}
