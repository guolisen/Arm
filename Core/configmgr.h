#ifndef CONFIGMGR_H
#define CONFIGMGR_H

#include <QObject>
#include "iconfigmgr.h"

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

private:
    QSettings* setting_;
};

}
#endif // CONFIGMGR_H
