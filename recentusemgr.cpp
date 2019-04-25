#include "recentusemgr.h"

RecentUseMgr::RecentUseMgr(core::ContextPtr context, const QString& objName,
                           int entryNum, QObject* parent):
                            IRecentUseMgr(parent), context_(context),
                            objName_(objName), entryNum_(entryNum)
{

}

void RecentUseMgr::addEntry(const QString &entry)
{
    if(entryList_.size() < entryNum_)
        entryList_ << entry;
}

EntryList RecentUseMgr::getEntryList(const QString &entry)
{

}
