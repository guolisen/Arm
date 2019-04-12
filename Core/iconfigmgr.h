#ifndef ICONFIGMGR_H
#define ICONFIGMGR_H

#include <memory>
#include <QObject>
#include <QVariant>
#include <ssh/sftpfilesystemmodel.h>
#include <ssh/sshconnection.h>

namespace core
{

class IConfigMgr : public QObject
{
    Q_OBJECT
public:
    explicit IConfigMgr(QObject *parent = nullptr): QObject(parent){}
    virtual ~IConfigMgr() = default;

    virtual QVariant getConfigInfo(const QString& key) = 0;
    virtual void setConfigInfo(const QString& key, const QVariant& value) = 0;
    virtual QSsh::SshConnectionParameters getSshParameters() = 0;
    virtual QString getCurrentVersion() = 0;
};

typedef std::shared_ptr<IConfigMgr> ConfigMgrPtr;

}
#endif // ICONFIGMGR_H
