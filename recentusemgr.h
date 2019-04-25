#ifndef RECENTUSEMGR_H
#define RECENTUSEMGR_H

#include <QObject>
#include <irecentusemgr.h>

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
            return std::make_shared<RecentUseMgr>(context, parent);
        }; // NOLINT
    }

    virtual void addEntry(const QString& entry);
    virtual EntryList getEntryList(const QString& entry);

private:
    core::ContextPtr context_;
    QString objName_;
    int entryNum_;
    EntryList entryList_;
};

#endif // RECENTUSEMGR_H
