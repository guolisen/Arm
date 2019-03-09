//
// Created by Lewis on 2017/3/7.
//

#ifndef ELFBOX_SCRIPTCENTER_H
#define ELFBOX_SCRIPTCENTER_H

#include "IScriptCenter.h"

namespace script
{
class ScriptCenter : public IScriptCenter
{
public:
    ScriptCenter(ScriptCenterPtr impl) : impl_(impl) {}
    virtual ~ScriptCenter() = default;

    virtual bool initialize()
    {
        return impl_->initialize();
    }

    virtual bool execute(const std::string &fileName)
    {
        return impl_->execute(fileName);
    }

    virtual bool executeString(const std::string &codeStr)
    {
        return impl_->executeString(codeStr);
    }
    virtual luacpp::luaWrapper& luaState()
    {
        return impl_->luaState();
    }
    virtual void lock()
    {
        return impl_->lock();
    }
    virtual void unlock()
    {
        return impl_->unlock();
    }
private:
    ScriptCenterPtr impl_;
};
}

#endif //ELFBOX_SCRIPTCENTER_H
