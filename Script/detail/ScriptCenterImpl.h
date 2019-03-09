//
// Created by Lewis on 2017/3/7.
//

#ifndef ELFBOX_SCRIPTCENTERIMPL_H
#define ELFBOX_SCRIPTCENTERIMPL_H

#include <vector>
#include <string>
#include <QMutex>
#include <QMutexLocker>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include "../IScriptCenter.h"
#include "Thirdparty/LuaWrapper/luaWrapper.h"

namespace script
{
namespace detail
{
class ScriptCenterImpl : public IScriptCenter
{
public:
    ScriptCenterImpl();
    virtual ~ScriptCenterImpl() = default;

    virtual bool initialize();

    virtual bool execute(const std::string &fileName);
    virtual bool executeString(const std::string &codeStr);
    virtual luacpp::luaWrapper& luaState()
    {
        return luaState_;
    }
    virtual void lock()
    {
        stateMutex_.lock();
    }
    virtual void unlock()
    {
        stateMutex_.unlock();
    }
private:
    void setContext();
    bool findLuaFunction(const std::string &fileName);
    std::vector<std::string> stringSplit(const std::string& str, const std::string& flag);
    luacpp::luaWrapper luaState_;
    QMutex stateMutex_;
};

}

}

#endif //ELFBOX_SCRIPTCENTERIMPL_H
