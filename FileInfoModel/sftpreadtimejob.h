#ifndef SFTPREADTIMEJOB_H
#define SFTPREADTIMEJOB_H

#include <QObject>
#include <ssh/sftpfilesystemmodel.h>
#include "ireadtimejob.h"


class SftpReadTimeJob : public QObject
{
    Q_OBJECT
public:
    explicit SftpReadTimeJob(QObject *parent = nullptr);


};

template<>
struct JobTrait<SftpReadTimeJob>
{
    typedef QSsh::SftpFileSystemModel ModelBaseType;
    const int logStartTimeColNum = 2;
};

#endif // SFTPREADTIMEJOB_H
