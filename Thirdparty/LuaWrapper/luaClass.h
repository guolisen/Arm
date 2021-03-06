#ifndef _LUACLASS_H
#define _LUACLASS_H
#include <string.h>
#include <map>
#include <set>
#include "luacommon.h"
#include "luaObject.h"
#include "any.h"
#include "luaFunction.h"

namespace luacpp{
template<typename T>
void push_obj(lua_State *L,const T &obj);
void set_userdata(lua_State *L,void *ptr,int index);

extern std::map<void*,void*> ptrToUserData;
extern std::set<void*> userdataSet;

enum{
	MEMBER_FUNCTION = 1,
	STATIC_FUNCTION,
	MEMBER_FIELD,
	CONSTRUCTOR,
};

template <typename T>
struct memberfield
{

	typedef void (*property_getter)(T *,lua_State*,void *(T::*));//for property get
    typedef void (*property_setter)(T *,lua_State*,void *(T::*));//for property set
    char tt;
    union{
    	struct {
    		property_getter getter;
    		property_setter setter;
    		void *(T::*property);
    	};
    	struct {
    		void  (T::*mfunction)(void);    //member func
    		lua_CFunction mlua_func;
    	};
    	struct {
    		void  (*sfunction)(void);       //static func
    		lua_CFunction slua_func;
    	};
        void (*constructor)(lua_State *L);
    };

	memberfield() {
		memset(this,0,sizeof(*this));
	}

    template<typename PARENT>
    memberfield(const memberfield<PARENT> &p){
    	memcpy(this,&p,sizeof(p));
    }

};

template<typename T>
class objUserData;

template<typename T,typename property_type>
static void GetPropertyData(T *self,lua_State *L,Int2Type<true>,void*(T::*field))
{
	//for obj type
	push_obj<property_type*>(L,(property_type*)&(self->*field));
}

template<typename T,typename property_type>
static void GetPropertyData(T *self,lua_State *L,Int2Type<false>,void*(T::*property))
{
	push_obj<property_type>(L,*(property_type*)&(self->*property));
}

template<typename T,typename property_type>
static void _GetProperty(T *self,lua_State *L,void*(T::*property),Int2Type<false>)
{
	//for other type
	GetPropertyData<T,property_type>(self,L,Int2Type<!pointerTraits<property_type>::isPointer\
	 && IndexOf<SupportType,typename pointerTraits<property_type>::PointeeType>::value == -1>(),property);
}

template<typename T,typename property_type>
static void _GetProperty(T *self,lua_State *L,void*(T::*property),Int2Type<true>)
{
	//for luatable
	luatable *lt_ptr = (luatable*)(&(self->*property));
	push_obj<luatable>(L,*lt_ptr);
}


template<typename T,typename property_type>
static void GetProperty(T *self,lua_State *L,void*(T::*property) )
{
	typedef LOKI_TYPELIST_1(luatable) lt;
	_GetProperty<T,property_type>(self,L,property,Int2Type<IndexOf<lt,property_type>::value==0>());
}

template<typename T,typename property_type>
static void SetPropertyData(T *self,lua_State *L,Int2Type<true>,void*(T::*property))
{
	//empty function for obj type
}

template<typename T,typename property_type>
static void SetPropertyData(T *self,lua_State *L,Int2Type<false>,void*(T::*property))
{
	property_type new_value;
	new_value = *(property_type*)(&(self->*property)) = popvalue<property_type>(L);
	push_obj<property_type>(L,new_value);
}

template<typename CLASS_TYPE,typename property_type>
struct Seter
{
	Seter(CLASS_TYPE *self,lua_State *L,void*(CLASS_TYPE::*property))
	{
		SetPropertyData<CLASS_TYPE,property_type>(self,L,Int2Type<IndexOf<SupportType,\
			typename pointerTraits<property_type>::PointeeType>::value == -1>(),property);
	}
};

template<typename CLASS_TYPE,typename property_type>
struct Seter<CLASS_TYPE,const property_type*>
{
	Seter(CLASS_TYPE *self,lua_State *L,void*(CLASS_TYPE::*property)){}
};

template<typename CLASS_TYPE>
struct Seter<CLASS_TYPE,const char*>
{
	Seter(CLASS_TYPE *self,lua_State *L,void*(CLASS_TYPE::*property)){}
};

template<typename CLASS_TYPE>
struct Seter<CLASS_TYPE,char*>
{
	Seter(CLASS_TYPE *self,lua_State *L,void*(CLASS_TYPE::*property)){}
};


void get_luatable(luatable &,lua_State *L);

template<typename CLASS_TYPE>
struct Seter<CLASS_TYPE,luatable>
{
	Seter(CLASS_TYPE *self,lua_State *L,void*(CLASS_TYPE::*property))
	{
		luatable *lt_ptr = (luatable*)(&(self->*property));
		get_luatable(*lt_ptr,L);
	}
};

template<typename T,typename property_type>
static void SetProperty(T *self,lua_State *L,void*(T::*property) )
{
	Seter<T,property_type> seter(self,L,property);
}

template<typename T>
class luaClassWrapper
{
    friend class objUserData<T>;

    public:

        static void luaopen_objlib(lua_State *L) {
            luaL_Reg meta[] = {
                {"__gc",&objUserData<T>::on_gc},
                {"__index",&objUserData<T>::Index},
                {"__newindex",&objUserData<T>::NewIndex},
                //{"__eq",&objUserData<T>::Eq},                
                {NULL, NULL}
            };             
            luaL_newmetatable(L, luaRegisterClass<T>::GetClassName());
            luaL_setfuncs(L, meta, 0);
            lua_pop(L,1);
        }

        static std::map<std::string,memberfield<T> > &GetAllFields()
        {
            return fields;
        }

        static void InsertFields(const char *name,memberfield<T> &mf)
        {
            fields.insert(std::make_pair(std::string(name),mf));
        }

		static int InsertConstructors(int arg_count,memberfield<T> &mf)
		{
			if(constructors[arg_count].constructor == NULL)
			{
				constructors[arg_count] = mf;
				++constructor_size;
				return constructor_size;
			}
			return -1;
		}

    private:
        static std::map<std::string,memberfield<T> > fields;
		static int constructor_size;
		static memberfield<T> constructors[16];
};


template<typename T>
std::map<std::string,memberfield<T> > luaClassWrapper<T>::fields;

template<typename T>
int luaClassWrapper<T>::constructor_size = 0;

template<typename T>
memberfield<T> luaClassWrapper<T>::constructors[16];

extern int pushObj(lua_State *L,const void *ptr,const char *classname);

template<typename T>
class objUserData
{
public:

    static objUserData<T> *checkobjuserdata (lua_State *L,int index) {
      void *ud = lua_touserdata(L,index);
      luaL_argcheck(L, ud != NULL, 1, "userdata expected");
      return (objUserData<T> *)ud;
    }

    static int pushObj(lua_State *L,const T *ptr)
    {
        return luacpp::pushObj(L,ptr,luaRegisterClass<T>::GetClassName());
    }

    static int NewIndex(lua_State *L)
    {
       objUserData<T> *self = checkobjuserdata(L,1);
       const char *name = luaL_checkstring(L, 2);
       typename std::map<std::string,memberfield<T> >::iterator it = luaClassWrapper<T>::fields.find(std::string(name));
       if(it != luaClassWrapper<T>::fields.end())
	   {
	     memberfield<T> &mf = it->second;
	     if(mf.tt == MEMBER_FIELD) mf.setter(self->ptr,L,mf.property);
	   }			
       return 0;
    }

    static int Index(lua_State *L)
    {
        objUserData<T> *obj = checkobjuserdata(L,1);
        const char *name = luaL_checkstring(L, 2);
        typename std::map<std::string,memberfield<T> >::iterator it = luaClassWrapper<T>::fields.find(std::string(name));
        if(it != luaClassWrapper<T>::fields.end())
        {
        	memberfield<T> &mf = it->second;
        	if(mf.tt == MEMBER_FUNCTION)
        	{
        		lua_pushlightuserdata(L,&mf.mfunction);
                lua_pushcclosure(L,mf.mlua_func,1);
                return 1;
        	}
        	else if(mf.tt == STATIC_FUNCTION)
        	{
        		lua_pushlightuserdata(L,(void*)mf.sfunction);
                lua_pushcclosure(L,mf.slua_func,1);
                return 1;        	
            }
            else if(mf.tt == MEMBER_FIELD)
            {
            	mf.getter(obj->ptr,L,mf.property);
            	return 1;
            }
        }
        return 0;
    }

    static int Eq(lua_State *L) {
        if(lua_type(L,1) != lua_type(L,2)) {
            lua_pushboolean(L,0);
            return 1;
        }
        objUserData<void*>* left = (objUserData<void*>*)lua_touserdata(L,1);
        objUserData<void*>* right = (objUserData<void*>*)lua_touserdata(L,2);
        lua_pushboolean(L,left->ptr == right->ptr ? 1:0);
        return 1;
    }

    static int Construct(lua_State *L)
    {
        int arg_size = lua_gettop(L);
		if(arg_size < 16 && luaClassWrapper<T>::constructors[arg_size].constructor)
		{
			luaClassWrapper<T>::constructors[arg_size].constructor(L);
            luaL_setmetatable(L,luaRegisterClass<T>::GetClassName());
		}
		else
		{
			char str[512];
			snprintf(str,512,"%s: unsupport %d arguments constructor\n",luaRegisterClass<T>::GetClassName(),arg_size);
			luaL_error(L,str);
		}
		return 1;
    }

	static int on_gc(lua_State *L)
	{
		objUserData<T> *self  = (objUserData<T> *)lua_touserdata(L,-1);
        if(self->ptr)
        {
            ptrToUserData.erase((void*)self->ptr);
    		userdataSet.erase((void*)self);
    		if(self->construct_by_lua)
    		{
    			delete self->ptr;
    		}
            self->ptr = NULL;
        }
		return 0;
	}

public:
    T   *ptr;
	bool construct_by_lua;

};


#ifndef _GETFUNC
#define _GETFUNC (*(__func*)lua_touserdata(L,lua_upvalueindex(1)))
#endif

template<typename FUNC>
class memberfunbinder{};

template<typename Ret,typename Cla>
class memberfunbinder<Ret(Cla::*)()>
{
public:
	typedef Cla CLASS_TYPE;
    static int lua_cfunction(lua_State *L)
    {
        objUserData<Cla> *obj = objUserData<Cla>::checkobjuserdata(L,1);
        Cla *cla = obj->ptr;
        return doCall<Ret>(L,cla,Int2Type<isVoid<Ret>::is_Void>());
    }

private:
    typedef Ret(Cla::*__func)();

    template<typename Result>
    static int doCall(lua_State *L,Cla *cla,Int2Type<false>)
    {
        push_obj<Result>(L,(cla->*_GETFUNC)());
        return 1;
    }

     template<typename Result>
    static int doCall(lua_State *L,Cla *cla,Int2Type<true>)
    {
        (cla->*_GETFUNC)();
        return 0;
    }

};

template<typename Ret,typename Arg1,typename Cla>
class memberfunbinder<Ret(Cla::*)(Arg1)>
{
public:
	typedef Cla CLASS_TYPE;
    static int lua_cfunction(lua_State *L)
    {
        objUserData<Cla> *obj = objUserData<Cla>::checkobjuserdata(L,1);
        Cla *cla = obj->ptr;
        typename GetReplaceType<Arg1>::type tmp_arg1 = popvalue<typename GetReplaceType<Arg1>::type>(L);
		Arg1 arg1 = GetRawValue<typename GetReplaceType<Arg1>::type>(tmp_arg1);
        return doCall<Ret>(L,cla,arg1,Int2Type<isVoid<Ret>::is_Void>());
    }

private:

    typedef Ret(Cla::*__func)(Arg1);

    template<typename Result>
    static int doCall(lua_State *L,Cla *cla,Arg1 arg1,Int2Type<false>)
    {
        push_obj<Result>(L,(cla->*_GETFUNC)(arg1));
        return 1;
    }

    template<typename Result>
    static int doCall(lua_State *L,Cla *cla,Arg1 arg1,Int2Type<true>)
    {
        (cla->*_GETFUNC)(arg1);
        return 0;
    }

};

template<typename Ret,typename Arg1,typename Arg2,typename Cla>
class memberfunbinder<Ret(Cla::*)(Arg1,Arg2)>
{
public:
	typedef Cla CLASS_TYPE;
    static int lua_cfunction(lua_State *L)
    {
        objUserData<Cla> *obj = objUserData<Cla>::checkobjuserdata(L,1);
        Cla *cla = obj->ptr;
        typename GetReplaceType<Arg2>::type tmp_arg2 = popvalue<typename GetReplaceType<Arg2>::type>(L);
        typename GetReplaceType<Arg1>::type tmp_arg1 = popvalue<typename GetReplaceType<Arg1>::type>(L);
		Arg1 arg1 = GetRawValue<typename GetReplaceType<Arg1>::type>(tmp_arg1);
		Arg2 arg2 = GetRawValue<typename GetReplaceType<Arg2>::type>(tmp_arg2);
        return doCall<Ret>(L,cla,arg1,arg2,Int2Type<isVoid<Ret>::is_Void>());
    }

private:

    typedef Ret(Cla::*__func)(Arg1,Arg2);

    template<typename Result>
    static int doCall(lua_State *L,Cla *cla,Arg1 arg1,Arg2 arg2,Int2Type<false>)
    {
        push_obj<Result>(L,(cla->*_GETFUNC)(arg1,arg2));
        return 1;
    }

    template<typename Result>
    static int doCall(lua_State *L,Cla *cla,Arg1 arg1,Arg2 arg2,Int2Type<true>)
    {
        (cla->*_GETFUNC)(arg1,arg2);
        return 0;
    }
};

template<typename Ret,typename Arg1,typename Arg2,typename Arg3,typename Cla>
class memberfunbinder<Ret(Cla::*)(Arg1,Arg2,Arg3)>
{
public:
	typedef Cla CLASS_TYPE;
    static int lua_cfunction(lua_State *L)
    {
        objUserData<Cla> *obj = objUserData<Cla>::checkobjuserdata(L,1);
        Cla *cla = obj->ptr;
        typename GetReplaceType<Arg3>::type tmp_arg3 = popvalue<typename GetReplaceType<Arg3>::type>(L);
        typename GetReplaceType<Arg2>::type tmp_arg2 = popvalue<typename GetReplaceType<Arg2>::type>(L);
        typename GetReplaceType<Arg1>::type tmp_arg1 = popvalue<typename GetReplaceType<Arg1>::type>(L);
		Arg1 arg1 = GetRawValue<typename GetReplaceType<Arg1>::type>(tmp_arg1);
		Arg2 arg2 = GetRawValue<typename GetReplaceType<Arg2>::type>(tmp_arg2);
		Arg3 arg3 = GetRawValue<typename GetReplaceType<Arg3>::type>(tmp_arg3);
        return doCall<Ret>(L,cla,arg1,arg2,arg3,Int2Type<isVoid<Ret>::is_Void>());
    }

private:

    typedef Ret(Cla::*__func)(Arg1,Arg2,Arg3);

    template<typename Result>
    static int doCall(lua_State *L,Cla *cla,Arg1 arg1,Arg2 arg2,Arg3 arg3,Int2Type<false>)
    {
        push_obj<Result>(L,(cla->*_GETFUNC)(arg1,arg2,arg3));
        return 1;
    }

    template<typename Result>
    static int doCall(lua_State *L,Cla *cla,Arg1 arg1,Arg2 arg2,Arg3 arg3,Int2Type<true>)
    {
        (cla->*_GETFUNC)(arg1,arg2,arg3);
        return 0;
    }
};

template<typename Ret,typename Arg1,typename Arg2,typename Arg3,typename Arg4,typename Cla>
class memberfunbinder<Ret(Cla::*)(Arg1,Arg2,Arg3,Arg4)>
{
public:

    static int lua_cfunction(lua_State *L)
    {
        objUserData<Cla> *obj = objUserData<Cla>::checkobjuserdata(L,1);
        Cla *cla = obj->ptr;
        typename GetReplaceType<Arg4>::type tmp_arg4 = popvalue<typename GetReplaceType<Arg4>::type>(L);
        typename GetReplaceType<Arg3>::type tmp_arg3 = popvalue<typename GetReplaceType<Arg3>::type>(L);
        typename GetReplaceType<Arg2>::type tmp_arg2 = popvalue<typename GetReplaceType<Arg2>::type>(L);
        typename GetReplaceType<Arg1>::type tmp_arg1 = popvalue<typename GetReplaceType<Arg1>::type>(L);
		Arg1 arg1 = GetRawValue<typename GetReplaceType<Arg1>::type>(tmp_arg1);
		Arg2 arg2 = GetRawValue<typename GetReplaceType<Arg2>::type>(tmp_arg2);
		Arg3 arg3 = GetRawValue<typename GetReplaceType<Arg3>::type>(tmp_arg3);
		Arg4 arg4 = GetRawValue<typename GetReplaceType<Arg4>::type>(tmp_arg4);
        return doCall<Ret>(L,cla,arg1,arg2,arg3,arg4,Int2Type<isVoid<Ret>::is_Void>());
    }

private:

    typedef Ret(Cla::*__func)(Arg1,Arg2,Arg3,Arg4);

    template<typename Result>
    static int doCall(lua_State *L,Cla *cla,Arg1 arg1,Arg2 arg2,Arg3 arg3,Arg4 arg4,Int2Type<false>)
    {
        push_obj<Result>(L,(cla->*_GETFUNC)(arg1,arg2,arg3,arg4));
        return 1;
    }

	template<typename Result>
    static int doCall(lua_State *L,Cla *cla,Arg1 arg1,Arg2 arg2,Arg3 arg3,Arg4 arg4,Int2Type<true>)
    {
        (cla->*_GETFUNC)(arg1,arg2,arg3,arg4);
        return 0;
    }
};

template<typename Parent,typename T>
void DefParent(Int2Type<false>)
{
     std::map<std::string,memberfield<Parent> > &parent_fields =  luaClassWrapper<Parent>::GetAllFields();
     std::map<std::string,memberfield<T> > &filds = luaClassWrapper<T>::GetAllFields();
     typename std::map<std::string,memberfield<Parent> >::iterator it = parent_fields.begin();
     typename std::map<std::string,memberfield<Parent> >::iterator end = parent_fields.end();
     for( ; it != end; ++it)
         filds.insert(std::make_pair(it->first,it->second));
}

template<typename Parent,typename T>
void DefParent(Int2Type<true>){}



template<typename T>
class class_def
{
private:
	class construct_function0
	{
	public:
		static void lua_cfunction(lua_State *L)
		{
			objUserData<T> *obj = (objUserData<T> *)lua_newuserdata(L, sizeof(objUserData<T>));
			obj->construct_by_lua = true;
			obj->ptr = new T;
			ptrToUserData[(void*)obj->ptr] = obj;
	    	userdataSet.insert((void*)obj);
            set_userdata(L,(void*)obj,-1);
			return;
		}
	};

	template<typename Arg1>
	class construct_function1
	{
	public:
		static void lua_cfunction(lua_State *L)
		{
			typename GetReplaceType<Arg1>::type tmp_arg1 = popvalue<typename GetReplaceType<Arg1>::type>(L);
			Arg1 arg1 = GetRawValue<typename GetReplaceType<Arg1>::type>(tmp_arg1);
			objUserData<T> *obj = (objUserData<T> *)lua_newuserdata(L, sizeof(objUserData<T>));
			obj->construct_by_lua = true;
			obj->ptr = new T(arg1);
			ptrToUserData[(void*)obj->ptr] = obj;
	    	userdataSet.insert((void*)obj);
            set_userdata(L,(void*)obj,-1);
		}
	};

	template<typename Arg1,typename Arg2>
	class construct_function2
	{
	public:
		static void lua_cfunction(lua_State *L)
		{
			typename GetReplaceType<Arg1>::type tmp_arg1 = popvalue<typename GetReplaceType<Arg1>::type>(L);
			Arg1 arg1 = GetRawValue<typename GetReplaceType<Arg1>::type>(tmp_arg1);
			typename GetReplaceType<Arg2>::type tmp_arg2 = popvalue<typename GetReplaceType<Arg2>::type>(L);
			Arg2 arg2 = GetRawValue<typename GetReplaceType<Arg2>::type>(tmp_arg2);
			objUserData<T> *obj = (objUserData<T> *)lua_newuserdata(L, sizeof(objUserData<T>));
			obj->construct_by_lua = true;
			obj->ptr = new T(arg1,arg2);
			ptrToUserData[(void*)obj->ptr] = obj;
	    	userdataSet.insert((void*)obj);
            set_userdata(L,(void*)obj,-1);
		}
	};

	template<typename Arg1,typename Arg2,typename Arg3>
	class construct_function3
	{
	public:
		static void lua_cfunction(lua_State *L)
		{
			typename GetReplaceType<Arg1>::type tmp_arg1 = popvalue<typename GetReplaceType<Arg1>::type>(L);
			Arg1 arg1 = GetRawValue<typename GetReplaceType<Arg1>::type>(tmp_arg1);
			typename GetReplaceType<Arg2>::type tmp_arg2 = popvalue<typename GetReplaceType<Arg2>::type>(L);
			Arg2 arg2 = GetRawValue<typename GetReplaceType<Arg2>::type>(tmp_arg2);
			typename GetReplaceType<Arg3>::type tmp_arg3 = popvalue<typename GetReplaceType<Arg3>::type>(L);
			Arg3 arg3 = GetRawValue<typename GetReplaceType<Arg3>::type>(tmp_arg3);
			objUserData<T> *obj = (objUserData<T> *)lua_newuserdata(L, sizeof(objUserData<T>));
			obj->construct_by_lua = true;
			obj->ptr = new T(arg1,arg2,arg3);
			ptrToUserData[(void*)obj->ptr] = obj;
	    	userdataSet.insert((void*)obj);
            set_userdata(L,(void*)obj,-1);
		}
	};

	template<typename Arg1,typename Arg2,typename Arg3,typename Arg4>
	class construct_function4
	{
	public:
		static void lua_cfunction(lua_State *L)
		{
			typename GetReplaceType<Arg1>::type tmp_arg1 = popvalue<typename GetReplaceType<Arg1>::type>(L);
			Arg1 arg1 = GetRawValue<typename GetReplaceType<Arg1>::type>(tmp_arg1);
			typename GetReplaceType<Arg2>::type tmp_arg2 = popvalue<typename GetReplaceType<Arg2>::type>(L);
			Arg2 arg2 = GetRawValue<typename GetReplaceType<Arg2>::type>(tmp_arg2);
			typename GetReplaceType<Arg3>::type tmp_arg3 = popvalue<typename GetReplaceType<Arg3>::type>(L);
			Arg3 arg3 = GetRawValue<typename GetReplaceType<Arg3>::type>(tmp_arg3);
			typename GetReplaceType<Arg4>::type tmp_arg4 = popvalue<typename GetReplaceType<Arg4>::type>(L);
			Arg4 arg4 = GetRawValue<typename GetReplaceType<Arg4>::type>(tmp_arg4);
			objUserData<T> *obj = (objUserData<T> *)lua_newuserdata(L, sizeof(objUserData<T>));
			obj->construct_by_lua = true;
			obj->ptr = new T(arg1,arg2,arg3,arg4);
			ptrToUserData[(void*)obj->ptr] = obj;
	    	userdataSet.insert((void*)obj);
            set_userdata(L,(void*)obj,-1);
		}
	};
    template<typename Arg1,typename Arg2,typename Arg3,typename Arg4,typename Arg5>
    class construct_function5
    {
    public:
        static void lua_cfunction(lua_State *L)
        {
            typename GetReplaceType<Arg1>::type tmp_arg1 = popvalue<typename GetReplaceType<Arg1>::type>(L);
            Arg1 arg1 = GetRawValue<typename GetReplaceType<Arg1>::type>(tmp_arg1);
            typename GetReplaceType<Arg2>::type tmp_arg2 = popvalue<typename GetReplaceType<Arg2>::type>(L);
            Arg2 arg2 = GetRawValue<typename GetReplaceType<Arg2>::type>(tmp_arg2);
            typename GetReplaceType<Arg3>::type tmp_arg3 = popvalue<typename GetReplaceType<Arg3>::type>(L);
            Arg3 arg3 = GetRawValue<typename GetReplaceType<Arg3>::type>(tmp_arg3);
            typename GetReplaceType<Arg4>::type tmp_arg4 = popvalue<typename GetReplaceType<Arg4>::type>(L);
            Arg4 arg4 = GetRawValue<typename GetReplaceType<Arg4>::type>(tmp_arg4);
            typename GetReplaceType<Arg5>::type tmp_arg5 = popvalue<typename GetReplaceType<Arg5>::type>(L);
            Arg4 arg5 = GetRawValue<typename GetReplaceType<Arg5>::type>(tmp_arg5);
            objUserData<T> *obj = (objUserData<T> *)lua_newuserdata(L, sizeof(objUserData<T>));
            obj->construct_by_lua = true;
            obj->ptr = new T(arg1,arg2,arg3,arg4,arg5);
            ptrToUserData[(void*)obj->ptr] = obj;
            userdataSet.insert((void*)obj);
            set_userdata(L,(void*)obj,-1);
        }
    };

    template<typename Arg1,typename Arg2,typename Arg3,typename Arg4,typename Arg5,typename Arg6,typename Arg7>
    class construct_function7
    {
    public:
        static void lua_cfunction(lua_State *L)
        {
            typename GetReplaceType<Arg1>::type tmp_arg1 = popvalue<typename GetReplaceType<Arg1>::type>(L);
            Arg1 arg1 = GetRawValue<typename GetReplaceType<Arg1>::type>(tmp_arg1);
            typename GetReplaceType<Arg2>::type tmp_arg2 = popvalue<typename GetReplaceType<Arg2>::type>(L);
            Arg2 arg2 = GetRawValue<typename GetReplaceType<Arg2>::type>(tmp_arg2);
            typename GetReplaceType<Arg3>::type tmp_arg3 = popvalue<typename GetReplaceType<Arg3>::type>(L);
            Arg3 arg3 = GetRawValue<typename GetReplaceType<Arg3>::type>(tmp_arg3);
            typename GetReplaceType<Arg4>::type tmp_arg4 = popvalue<typename GetReplaceType<Arg4>::type>(L);
            Arg4 arg4 = GetRawValue<typename GetReplaceType<Arg4>::type>(tmp_arg4);
            typename GetReplaceType<Arg5>::type tmp_arg5 = popvalue<typename GetReplaceType<Arg5>::type>(L);
            Arg4 arg5 = GetRawValue<typename GetReplaceType<Arg5>::type>(tmp_arg5);
            typename GetReplaceType<Arg6>::type tmp_arg6 = popvalue<typename GetReplaceType<Arg6>::type>(L);
            Arg4 arg6 = GetRawValue<typename GetReplaceType<Arg6>::type>(tmp_arg6);
            typename GetReplaceType<Arg7>::type tmp_arg7 = popvalue<typename GetReplaceType<Arg7>::type>(L);
            Arg4 arg7 = GetRawValue<typename GetReplaceType<Arg7>::type>(tmp_arg7);

            objUserData<T> *obj = (objUserData<T> *)lua_newuserdata(L, sizeof(objUserData<T>));
            obj->construct_by_lua = true;
            obj->ptr = new T(arg1,arg2,arg3,arg4,arg5,arg6,arg7);
            ptrToUserData[(void*)obj->ptr] = obj;
            userdataSet.insert((void*)obj);
            set_userdata(L,(void*)obj,-1);
        }
    };
    template<typename Arg1,typename Arg2,typename Arg3,typename Arg4,typename Arg5,typename Arg6,typename Arg7,typename Arg8>
    class construct_function8
    {
    public:
        static void lua_cfunction(lua_State *L)
        {
            typename GetReplaceType<Arg1>::type tmp_arg1 = popvalue<typename GetReplaceType<Arg1>::type>(L);
            Arg1 arg1 = GetRawValue<typename GetReplaceType<Arg1>::type>(tmp_arg1);
            typename GetReplaceType<Arg2>::type tmp_arg2 = popvalue<typename GetReplaceType<Arg2>::type>(L);
            Arg2 arg2 = GetRawValue<typename GetReplaceType<Arg2>::type>(tmp_arg2);
            typename GetReplaceType<Arg3>::type tmp_arg3 = popvalue<typename GetReplaceType<Arg3>::type>(L);
            Arg3 arg3 = GetRawValue<typename GetReplaceType<Arg3>::type>(tmp_arg3);
            typename GetReplaceType<Arg4>::type tmp_arg4 = popvalue<typename GetReplaceType<Arg4>::type>(L);
            Arg4 arg4 = GetRawValue<typename GetReplaceType<Arg4>::type>(tmp_arg4);
            typename GetReplaceType<Arg5>::type tmp_arg5 = popvalue<typename GetReplaceType<Arg5>::type>(L);
            Arg4 arg5 = GetRawValue<typename GetReplaceType<Arg5>::type>(tmp_arg5);
            typename GetReplaceType<Arg6>::type tmp_arg6 = popvalue<typename GetReplaceType<Arg6>::type>(L);
            Arg4 arg6 = GetRawValue<typename GetReplaceType<Arg6>::type>(tmp_arg6);
            typename GetReplaceType<Arg7>::type tmp_arg7 = popvalue<typename GetReplaceType<Arg7>::type>(L);
            Arg4 arg7 = GetRawValue<typename GetReplaceType<Arg7>::type>(tmp_arg7);
            typename GetReplaceType<Arg8>::type tmp_arg8 = popvalue<typename GetReplaceType<Arg8>::type>(L);
            Arg4 arg8 = GetRawValue<typename GetReplaceType<Arg8>::type>(tmp_arg8);

            objUserData<T> *obj = (objUserData<T> *)lua_newuserdata(L, sizeof(objUserData<T>));
            obj->construct_by_lua = true;
            obj->ptr = new T(arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8);
            ptrToUserData[(void*)obj->ptr] = obj;
            userdataSet.insert((void*)obj);
            set_userdata(L,(void*)obj,-1);
        }
    };

public:
	class_def(lua_State *L):L(L){}

	template<typename property_type>
	class_def<T>& property(const char *name,property_type (T::*property))
	{
		memberfield<T> mf;
		mf.tt  = MEMBER_FIELD;
		mf.getter = GetProperty<T,property_type>;
		mf.property = (void*(T::*))property;
		mf.setter = SetProperty<T,property_type>;
		luaClassWrapper<T>::InsertFields(name,mf);
		return *this;
	}

	//class member method
	template<typename FUNTOR>
	class_def<T>& method(const char *name,FUNTOR _func)
	{
		memberfield<T> mf;
		mf.tt = MEMBER_FUNCTION;
		mf.mfunction = (void(T::*)())_func;
		mf.mlua_func = memberfunbinder<FUNTOR>::lua_cfunction;
		luaClassWrapper<T>::InsertFields(name,mf);
		return *this;
	}

	//class static method
	template<typename Ret>
	class_def<T>& method(const char *name,Ret(*_func)())
	{
		memberfield<T> mf;
		mf.tt = STATIC_FUNCTION;
		mf.sfunction = (void (*)())_func;
		mf.slua_func = funbinder<Ret(*)()>::lua_cfunction;
		luaClassWrapper<T>::InsertFields(name,mf);
		return *this;		
	}

	template<typename Ret,typename Arg1>
	class_def<T>& method(const char *name,Ret(*_func)(Arg1))
	{
		memberfield<T> mf;
		mf.tt = STATIC_FUNCTION;
		mf.sfunction = (void (*)())_func;
		mf.slua_func = funbinder<Ret(*)(Arg1)>::lua_cfunction;
		luaClassWrapper<T>::InsertFields(name,mf);
		return *this;		
	}

	template<typename Ret,typename Arg1,typename Arg2>
	class_def<T>& method(const char *name,Ret(*_func)(Arg1,Arg2))
	{
		memberfield<T> mf;
		mf.tt = STATIC_FUNCTION;
		mf.sfunction = (void (*)())_func;
		mf.slua_func = funbinder<Ret(*)(Arg1,Arg2)>::lua_cfunction;
		luaClassWrapper<T>::InsertFields(name,mf);
		return *this;		
	}

	template<typename Ret,typename Arg1,typename Arg2,typename Arg3>
	class_def<T>& method(const char *name,Ret(*_func)(Arg1,Arg2,Arg3))
	{
		memberfield<T> mf;
		mf.tt = STATIC_FUNCTION;
		mf.sfunction = (void (*)())_func;
		mf.slua_func = funbinder<Ret(*)(Arg1,Arg2,Arg3)>::lua_cfunction;
		luaClassWrapper<T>::InsertFields(name,mf);
		return *this;		
	}								 


	template<typename Ret,typename Arg1,typename Arg2,typename Arg3,typename Arg4>
	class_def<T>& method(const char *name,Ret(*_func)(Arg1,Arg2,Arg3,Arg4))
	{
		memberfield<T> mf;
		mf.tt = STATIC_FUNCTION;
		mf.sfunction = (void (*)())_func;
		mf.slua_func = funbinder<Ret(*)(Arg1,Arg2,Arg3,Arg4)>::lua_cfunction;
		luaClassWrapper<T>::InsertFields(name,mf);
		return *this;		
	}

	template<typename ARG1>
	class_def<T>& constructor()
	{
		_constructor<ARG1>(Int2Type<isVoid<ARG1>::is_Void == 1>());
		return *this;
	}

	template<typename ARG1,typename ARG2>
	class_def<T>& constructor()
	{
		memberfield<T> mf;
		mf.tt = CONSTRUCTOR;
		mf.constructor = construct_function2<ARG1,ARG2>::lua_cfunction;
		_constructor(2,mf);
		return *this;
	}

	template<typename ARG1,typename ARG2,typename ARG3>
	class_def<T>& constructor()
	{
		memberfield<T> mf;
		mf.tt = CONSTRUCTOR;
		mf.constructor = construct_function3<ARG1,ARG2,ARG3>::lua_cfunction;
		_constructor(3,mf);
		return *this;
	}

	template<typename ARG1,typename ARG2,typename ARG3,typename ARG4>
	class_def<T>& constructor()
	{
		memberfield<T> mf;
		mf.tt = CONSTRUCTOR;
		mf.constructor = construct_function4<ARG1,ARG2,ARG3,ARG4>::lua_cfunction;
		_constructor(4,mf);
		return *this;
	}
    template<typename ARG1,typename ARG2,typename ARG3,typename ARG4,typename ARG5>
    class_def<T>& constructor()
    {
        memberfield<T> mf;
        mf.tt = CONSTRUCTOR;
        mf.constructor = construct_function5<ARG1,ARG2,ARG3,ARG4,ARG5>::lua_cfunction;
        _constructor(5,mf);
        return *this;
    }
    template<typename ARG1,typename ARG2,typename ARG3,typename ARG4,typename ARG5,typename ARG6,typename ARG7>
    class_def<T>& constructor()
    {
        memberfield<T> mf;
        mf.tt = CONSTRUCTOR;
        mf.constructor = construct_function7<ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7>::lua_cfunction;
        _constructor(7,mf);
        return *this;
    }
    template<typename ARG1,typename ARG2,typename ARG3,typename ARG4,typename ARG5,typename ARG6,typename ARG7,typename ARG8>
    class_def<T>& constructor()
    {
        memberfield<T> mf;
        mf.tt = CONSTRUCTOR;
        mf.constructor = construct_function8<ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,ARG8>::lua_cfunction;
        _constructor(8,mf);
        return *this;
    }
private:
	template<typename ARG1>
	void _constructor(Int2Type<true>)
	{
		memberfield<T> mf;
		mf.tt = CONSTRUCTOR;
		mf.constructor = construct_function0::lua_cfunction;
		_constructor(0,mf);
	}

	template<typename ARG1>
	void _constructor(Int2Type<false>)
	{
		memberfield<T> mf;
		mf.tt = CONSTRUCTOR;
		mf.constructor = construct_function1<ARG1>::lua_cfunction;
		_constructor(1,mf);
	}

	void _constructor(int arg_size,memberfield<T> &mf)
	{
		if(1 == luaClassWrapper<T>::InsertConstructors(arg_size,mf))
		{
            lua_pushcfunction(L,objUserData<T>::Construct);
            lua_setglobal(L,luaRegisterClass<T>::GetClassName());
		}
	}
	lua_State *L;

};

template<typename T,typename Parent1 = void,typename Parent2 = void,typename Parent3 = void,typename Parent4 = void>
struct reg_cclass
{
	static class_def<T> _reg(lua_State *L,const char *name)
	{
		luaRegisterClass<T>::SetClassName(name);
		luaClassWrapper<T>::luaopen_objlib(L);
		luaRegisterClass<T>::SetRegister();
		DefParent<Parent1,T>(Int2Type<isVoid<Parent1>::is_Void>());
		DefParent<Parent2,T>(Int2Type<isVoid<Parent2>::is_Void>());
		DefParent<Parent3,T>(Int2Type<isVoid<Parent3>::is_Void>());
		DefParent<Parent4,T>(Int2Type<isVoid<Parent4>::is_Void>());
		return class_def<T>(L);
	}
};

}
#endif
