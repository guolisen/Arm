#include <QSettings>
#include "configmgr.h"

namespace core
{

ConfigMgr::ConfigMgr(QObject *parent) : IConfigMgr(parent), setting_(new QSettings(this))
{

}

QVariant ConfigMgr::getConfigInfo(const QString &key)
{
    return QVariant();
}

void ConfigMgr::setConfigInfo(const QString &key, const QVariant &value)
{

}

}
