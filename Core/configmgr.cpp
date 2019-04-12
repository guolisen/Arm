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

    //IP
    QString siteIp = getConfigInfo("Arm/Setting/siteIp").toString();
    if (siteIp.isEmpty())
        siteIp = "";
    sshParams.host = siteIp;
    qDebug() << "sshParams.host: " << sshParams.host;

    //port
    QString port = getConfigInfo("Arm/Setting/port").toString();
    if (port.isEmpty())
        port = "22";
    sshParams.port = port.toInt();
    qDebug() << "sshParams.port: " << sshParams.port;

    //userName
    QString userName = getConfigInfo("Arm/Setting/userName").toString();
    if (userName.isEmpty())
        userName = "c4dev";
    sshParams.userName = userName;
    qDebug() << "sshParams.userName: " << sshParams.userName;

    //password
    QString password = getConfigInfo("Arm/Setting/password").toString();
    if (password.isEmpty())
        password = "c4dev!";
    sshParams.password = password;
    qDebug() << "sshParams.password: " << sshParams.password;

    //timeOut
    QString timeOut = getConfigInfo("Arm/Setting/timeOut").toString();
    if (timeOut.isEmpty())
        timeOut = "99999";
    sshParams.timeout = timeOut.toInt();
    qDebug() << "sshParams.timeout: " << sshParams.timeout;

    //keyFile
    QString keyFile = getConfigInfo("Arm/Setting/keyFile").toString();
    if (keyFile.isEmpty())
        keyFile = "0";
    if (keyFile.toInt())
    {
        sshParams.authenticationType = QSsh::SshConnectionParameters::AuthenticationByKey;
        QString keyFilePath = getConfigInfo("Arm/Setting/keyFilePath").toString();
        sshParams.privateKeyFile = keyFilePath;
    }
    else
        sshParams.authenticationType = QSsh::SshConnectionParameters::AuthenticationByPassword;
    qDebug() << "sshParams.authenticationType: " << sshParams.authenticationType;
    qDebug() << "sshParams.privateKeyFile: " << sshParams.privateKeyFile;

    return sshParams;
}

QString ConfigMgr::getCurrentVersion()
{
    return "0.2";
}
}
