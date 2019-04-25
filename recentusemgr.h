#ifndef RECENTUSEMGR_H
#define RECENTUSEMGR_H

#include <QObject>
#include <irecentusemgr.h>
#include <Core/iconfigmgr.h>

class RecentUseMgr : public IRecentUseMgr
{
public:
    RecentUseMgr(core::ContextPtr context, const QString& objName,
                 int entryNum, QObject* parent = nullptr);

    static IRecentUseMgr::Factory getFactory()
    {
        return [](core::ContextPtr context,  const QString& objName,
                int entryNum, QObject* parent = nullptr) -> RecentUseMgrPtr
        {
            return std::make_shared<RecentUseMgr>(context, objName, entryNum, parent);
        }; // NOLINT
    }

    virtual void addEntry(const QString& entry);
    virtual EntryList getEntryList();

private:
    core::ContextPtr context_;
    core::ConfigMgrPtr configMgrPtr_;
    QString objName_;
    int entryNum_;
    EntryList entryList_;
};

#endif // RECENTUSEMGR_H
