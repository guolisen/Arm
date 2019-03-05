//
// Created by Lewis on 2017/3/7.
//

#include <sstream>
extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

//#include <tolua++.h>


#include "ScriptCenterImpl.h"
#include "Thirdparty/LuaWrapper/luaWrapper.h"

namespace script
{
namespace detail
{
ScriptCenterImpl::ScriptCenterImpl()
{
}

bool ScriptCenterImpl::initialize()
{
    //tolua_IObject_open (luaState_);
    //setContext();

    return true;
}


bool ScriptCenterImpl::execute(const std::string &fileName)
{
    if (luaL_loadfile(luaState_, fileName.c_str()))
    {
        const char* message = lua_tostring(luaState_, -1);
        printf("Load Buffer failed for %s: %s", fileName.c_str(), message);
        lua_pop(luaState_, 1);
        return false;
    }

    if (lua_pcall(luaState_, 0, 0, 0))
    {
        const char* message = lua_tostring(luaState_, -1);
        printf("Lua Execute failed for %s: %s", fileName.c_str(), message);
        lua_pop(luaState_, 1);
        return false;
    }

    return true;
}

bool ScriptCenterImpl::executeString(const std::string &codeStr)
{
    return false;
}

bool ScriptCenterImpl::findLuaFunction(const std::string &functionName)
{
    std::vector<std::string> splitNames = stringSplit(functionName, ".");

    std::string currentName = splitNames.front();
    lua_getglobal(luaState_, currentName.c_str());

    if (splitNames.size() > 1)
    {
        for (unsigned i = 0; i < splitNames.size() - 1; ++i)
        {
            if (i)
            {
                currentName = currentName + "." + splitNames[i];
                lua_getfield(luaState_, -1, splitNames[i].c_str());
                lua_replace(luaState_, -2);
            }
            if (!lua_istable(luaState_, -1))
            {
                lua_pop(luaState_, 1);
                lua_pushstring(luaState_, ("Could not find Lua table: Table name = '" + currentName + "'").c_str());
                return false;
            }
        }

        currentName = currentName + "." + splitNames.back();
        lua_getfield(luaState_, -1, splitNames.back().c_str());
        lua_replace(luaState_, -2);
    }

    if (!lua_isfunction(luaState_, -1))
    {
        lua_pop(luaState_, 1);
        lua_pushstring(luaState_, ("Could not find Lua function: Function name = '" + currentName + "'").c_str());
        return false;
    }

    return true;
}

std::vector<std::string> ScriptCenterImpl::stringSplit(
    const std::string &strLine, const std::string &flag)
{
    std::string tmpStr = strLine;
    for (auto iter = tmpStr.begin(); iter != tmpStr.end(); ++iter) {
        if (*iter == flag.c_str()[0]) *iter = ' ';
    }

    std::vector<std::string> result;
    std::istringstream buffer(tmpStr);
    std::string str;
    while (buffer.good()) {
        buffer >> str;
        result.push_back(str);
    }

    return result;
}

void ScriptCenterImpl::setContext()
{
    //tolua_pushusertype(luaState_, static_cast<void*>(&context_), "ContextPtr");
    //lua_setglobal(luaState_, ".context");
}

}
}

