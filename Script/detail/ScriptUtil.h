//
// Created by Lewis on 2017/3/12.
//

#ifndef ELFBOX_SCRIPTUTIL_H
#define ELFBOX_SCRIPTUTIL_H

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
#include <string>

namespace script
{
namespace detail
{
//common::ContextPtr getContext(lua_State* L);

template <typename T> int toluaGetCompoent(lua_State* tolua_S)
{
#if 0
    T* component = getContext(tolua_S)->getComponent<T>(nullptr).get();
    std::string objName = T::GetTypeNameStatic();
    const common::TypeInfo *baseTypeInfo = T::GetTypeInfoStatic()->GetBaseTypeInfo();
    if (baseTypeInfo)
        objName = baseTypeInfo->GetTypeName();

    tolua_pushusertype(tolua_S, (void*)component, objName.c_str());
#endif
    return 1;
}

}
}

#endif //ELFBOX_SCRIPTUTIL_H
