#ifndef UNCOMPRESSFILECACHE_H
#define UNCOMPRESSFILECACHE_H

#include <QString>

class UncompressFileCache
{
public:
    UncompressFileCache();

    QString createUncompressCacheFile(const QString& fullFilePath);
    QString getCacheFileName(const QString &fullFilePath);
private:

    QString getNewFileName(const QString &oldFileName);
    QString getCompressFile(const QString &fullFilePath);

    int fileNumber_;
};

#endif // UNCOMPRESSFILECACHE_H
