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

QVariant ConfigMgr::getConfigInfo(const QString &key, const QVariant &defaultValue)
{
    return setting_->value(key, defaultValue);
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
    QString siteIp = getConfigInfo("Arm/Setting/siteIp", "").toString();
    sshParams.host = siteIp;
    qDebug() << "sshParams.host: " << sshParams.host;

    //port
    QString port = getConfigInfo("Arm/Setting/port", "22").toString();
    sshParams.port = port.toInt();
    //qDebug() << "sshParams.port: " << sshParams.port;

    //userName
    QString userName = getConfigInfo("Arm/Setting/userName", "c4dev").toString();
    sshParams.userName = userName;
    qDebug() << "sshParams.userName: " << sshParams.userName;

    //password
    QString password = getConfigInfo("Arm/Setting/password", "c4dev!").toString();
    sshParams.password = password;
    //qDebug() << "sshParams.password: " << sshParams.password;

    //timeOut
    QString timeOut = getConfigInfo("Arm/Setting/timeOut", "99999").toString();
    sshParams.timeout = timeOut.toInt();
    //qDebug() << "sshParams.timeout: " << sshParams.timeout;

    //keyFile
    QString keyFile = getConfigInfo("Arm/Setting/keyFile", "0").toString();
    if (keyFile.toInt())
    {
        sshParams.authenticationType = QSsh::SshConnectionParameters::AuthenticationByKey;
        QString keyFilePath = getConfigInfo("Arm/Setting/keyFilePath", "").toString();
        sshParams.privateKeyFile = keyFilePath;
    }
    else
        sshParams.authenticationType = QSsh::SshConnectionParameters::AuthenticationByPassword;
    //qDebug() << "sshParams.authenticationType: " << sshParams.authenticationType;
    //qDebug() << "sshParams.privateKeyFile: " << sshParams.privateKeyFile;

    return sshParams;
}

QString ConfigMgr::getCurrentVersion()
{
    return "0.25";
}
}
