/*	
    Copyright (C) <2012>  <huangweilook@21cn.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/	
#ifndef _POPVALUE_H
#define _POPVALUE_H

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <set>
#include "any.h"
#include "ObjPush.h"
#include "luaClass.h"

namespace luacpp{

extern std::set<void*> userdataSet;

template<typename T>
inline T _pop(lua_State *L,Int2Type<true>)
{
	typedef typename pointerTraits<T>::PointeeType rType;
	objUserData<rType> *obj = objUserData<rType>::checkobjuserdata(L,-1);
	lua_pop(L,1);
	return obj->ptr;
}

template<typename T>
inline T _pop_impl(lua_State *L,Int2Type<true>)
{
	T ret = (T)lua_tonumber(L,-1);
	lua_pop(L,1);
	return ret;
}

template<typename T>
inline T _pop_impl(lua_State *L,Int2Type<false>)
{
	return *_pop<typename refTraits<T>::RefType*>(L,Int2Type<true>());
}

template<typename T>
inline T _pop(lua_State *L,Int2Type<false>)
{
	return _pop_impl<T>(L,Int2Type<IndexOf<SupportType,T>::value >= 0>());
}

template<typename T>
inline T popvalue(lua_State *L)
{
	return _pop<T>(L,Int2Type<pointerTraits<T>::isPointer>());
}


template<>
inline luaObject popvalue(lua_State *L)
{
	int r = luaL_ref(L,LUA_REGISTRYINDEX);
	luaObject obj(L,r);
	return obj;
}

template<>
inline std::string popvalue(lua_State *L)
{
	const char *str = lua_tostring(L,-1);
	std::string ret(str);
	lua_pop(L,1);
	return ret;
}

template<typename T>
inline T pop_void_ptr(lua_State *L)
{
	T ret;
	ret = lua_touserdata(L,-1);

	if(userdataSet.find((void*)ret) != userdataSet.end())
		ret = ((objUserData<void>*)ret)->ptr;
	lua_pop(L,1);
	return ret;	
}

template<>
inline const void *popvalue(lua_State *L)
{
	return pop_void_ptr<const void*>(L);
}

template<>
inline void *popvalue(lua_State *L)
{
	return pop_void_ptr<void*>(L);
}

template<>
inline bool popvalue(lua_State *L)
{
	bool ret = lua_toboolean(L,-1) == 1;
	lua_pop(L,1);
	return ret;
}

template<>
inline double popvalue(lua_State *L)
{
	double ret = lua_tonumber(L,-1);
	lua_pop(L,1);
	return ret;
}

template<>
inline float popvalue(lua_State *L)
{
	double ret = lua_tonumber(L,-1);
	lua_pop(L,1);
	return (float)ret;
}

template<>
inline luatable popvalue(lua_State *L)
{
	luatable ret;
	int len = luaL_len(L, -1);
	for( int i = 1; i <= len; ++i)
	{
		lua_rawgeti(L,-1,i);
		int type = lua_type(L,-1);
		if(type == LUA_TNIL)
		{
			ret.push_back(any());
			lua_pop(L,1);
		}
		else if(type == LUA_TUSERDATA || type == LUA_TLIGHTUSERDATA)
		{
			void *r = lua_touserdata(L,-1);
			if(userdataSet.find(r) != userdataSet.end())
				ret.push_back((const void*)((objUserData<void>*)r)->ptr);
			else
				ret.push_back(r);
			lua_pop(L,1);
		}		
		else if(type == LUA_TNUMBER){
			ret.push_back(popvalue<double>(L));
		}
		else if(type == LUA_TSTRING)
			ret.push_back(popvalue<std::string>(L));
		else if(type == LUA_TBOOLEAN)
			ret.push_back(popvalue<bool>(L));
		else if(type == LUA_TTABLE)
		{
			int _len = luaL_len(L, -1);	
			if(_len == 0)
			{
				lua_pushnil(L);//push a key
				if(lua_next(L,-2)){
					lua_pop(L,2);//pop value and value
					ret.push_back(popvalue<luaObject>(L));
				}
				else{
					ret.push_back(popvalue<luatable>(L));
				}
			}else{
				ret.push_back(popvalue<luatable>(L));
			}
		}
		else
			throw std::string("error");
	}
	lua_pop(L,1);
	return ret;
}

//Get lua global object
template<typename T>
T Get(lua_State *L,const char *name)
{
	lua_getglobal(L,name);
	return popvalue<T>(L);
}

}

#endif
