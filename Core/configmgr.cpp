#include <QSettings>
#include "configmgr.h"
#include <ssh/sftpfilesystemmodel.h>
#include <ssh/sshconnection.h>

namespace core
{

ConfigMgr::ConfigMgr(QObject *parent) : IConfigMgr(parent),
    setting_(new QSettings("Config.ini", QSettings::IniFormat, this))
{

}

QVariant ConfigMgr::getConfigInfo(const QString &key)
{
    return setting_->value(key);
}

void ConfigMgr::setConfigInfo(const QString &key, const QVariant &value)
{
    setting_->setValue(key, value);
    setting_->sync();
}

QSsh::SshConnectionParameters ConfigMgr::getSshParameters()
{
    QSsh::SshConnectionParameters sshParams;
    sshParams.authenticationType = QSsh::SshConnectionParameters::AuthenticationByPassword;

    //IP
    QString siteIp = getConfigInfo("Arm/Setting/siteIp").toString();
    if (siteIp.isEmpty())
        siteIp = "";
    sshParams.host = siteIp;

    //port
    QString port = getConfigInfo("Arm/Setting/port").toString();
    if (port.isEmpty())
        port = "22";
    sshParams.port = port.toInt();

    //userName
    QString userName = getConfigInfo("Arm/Setting/userName").toString();
    if (userName.isEmpty())
        userName = "c4dev";
    sshParams.userName = userName;

    //password
    QString password = getConfigInfo("Arm/Setting/password").toString();
    if (password.isEmpty())
        password = "c4dev!";
    sshParams.password = password;

    //timeOut
    QString timeOut = getConfigInfo("Arm/Setting/timeOut").toString();
    if (timeOut.isEmpty())
        timeOut = "99999";
    sshParams.timeout = timeOut.toInt();

    return sshParams;
}
}
