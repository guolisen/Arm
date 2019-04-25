#ifndef IRECENTUSEMGR_H
#define IRECENTUSEMGR_H

#include <QObject>
#include <memory>
#include <functional>
#include <Core/appcontext.h>

typedef QList<QString> EntryList;
class IRecentUseMgr : public QObject
{
    Q_OBJECT
public:
    typedef std::function<std::shared_ptr<IRecentUseMgr>(
            core::ContextPtr, const QString&, int, QObject*)> Factory;

    explicit IRecentUseMgr(QObject *parent = nullptr):
        QObject(parent) {};
    virtual ~IRecentUseMgr() = default;

    virtual void addEntry(const QString& entry) = 0;
    virtual EntryList getEntryList(const QString& entry) = 0;
};

typedef std::shared_ptr<IRecentUseMgr> RecentUseMgrPtr;

#endif // IRECENTUSEMGR_H
