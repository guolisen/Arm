#ifndef CONFIGMGR_H
#define CONFIGMGR_H

#include <QObject>
#include "iconfigmgr.h"
#include <ssh/sftpfilesystemmodel.h>
#include <ssh/sshconnection.h>

class QSettings;
namespace core
{

class ConfigMgr : public IConfigMgr
{
    Q_OBJECT
public:
    explicit ConfigMgr(QObject *parent = nullptr);

    virtual QVariant getConfigInfo(const QString& key);
    virtual void setConfigInfo(const QString& key, const QVariant& value);

    virtual QSsh::SshConnectionParameters getSshParameters();
    virtual QString getCurrentVersion();
private:
    QSettings* setting_;
};

}
#endif // CONFIGMGR_H
