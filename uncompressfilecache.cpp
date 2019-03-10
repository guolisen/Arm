#include <QFile>
#include <QDebug>
#include <QFileInfo>
#include <quagzipfile.h>
#include "uncompressfilecache.h"

UncompressFileCache::UncompressFileCache(): fileNumber_(0)
{

}

QString UncompressFileCache::getCompressFile(const QString &fullFilePath)
{
    if (fullFilePath.contains(".gz"))
        return "gz";
    else if(fullFilePath.contains(".zip"))
        return "zip";

    return "";
}

QString UncompressFileCache::createUncompressCacheFile(const QString &fullFilePath)
{
    QString compressType = getCompressFile(fullFilePath);
    if (compressType.isEmpty())
        return fullFilePath;

    QuaGzipFile gzip(fullFilePath);
    if(!gzip.open(QIODevice::ReadOnly))
    {
        qDebug() << "Cannot open zip file";
        return QString();
    }
    QString cacheFileName = getCacheFileName(fullFilePath);
    QFile outFile("./" + cacheFileName);

    if (!outFile.open(QIODevice::WriteOnly))
    {
        qDebug() << "Cannot create cache file";
        return QString();
    }

    outFile.write(gzip.readAll());
    outFile.close();
    gzip.close();

    return cacheFileName;
}

QString UncompressFileCache::getCacheFileName(const QString &fullFilePath)
{
    QFileInfo info(fullFilePath);
    QString oldFileName = info.fileName();
    QString newFileName = "./cache_" + oldFileName;
    QFileInfo newInfo(newFileName);
    if (!newInfo.exists())
        return newFileName;
    return getNewFileName(newFileName);
}

QString UncompressFileCache::getNewFileName(const QString &newFileName)
{
    QString newFileNameNum = newFileName;
    newFileNameNum += "_"+ QString("%1").arg(++fileNumber_);
    QFileInfo newInfoNum(newFileNameNum);
    if (!newInfoNum.exists())
        return newFileNameNum;
    return getNewFileName(newFileName);
}




