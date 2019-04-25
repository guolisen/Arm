#include <Core/appcontext.h>
#include <Core/configmgr.h>
#include "recentusemgr.h"

RecentUseMgr::RecentUseMgr(core::ContextPtr context, const QString& objName,
                           int entryNum, QObject* parent):
                            IRecentUseMgr(parent), context_(context),
                            configMgrPtr_(context_->getComponent<core::IConfigMgr>(nullptr)),
                            objName_(objName), entryNum_(entryNum)
{
    //virtual QList<QString> getConfigArray(const QString& key, const QString &valueKey);
    //virtual void setConfigArray(const QString& key, const QString &valueKey, const QList<QString>& configList);
    entryList_ = configMgrPtr_->getConfigArray(objName_, "Array");
}

void RecentUseMgr::addEntry(const QString &entry)
{
    if (entry.isEmpty())
        return;
    if (entryList_.contains(entry))
    {
        entryList_.removeOne(entry);
    }
    if (entryList_.size() < entryNum_)
    {
        entryList_.push_front(entry);
        configMgrPtr_->setConfigArray(objName_, "Array", entryList_);
        return;
    }
    entryList_.push_front(entry);
    entryList_.pop_back();
    configMgrPtr_->setConfigArray(objName_, "Array", entryList_);
}

EntryList RecentUseMgr::getEntryList()
{
    return entryList_;
}
