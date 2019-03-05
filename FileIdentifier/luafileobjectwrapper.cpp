#include "luafileobjectwrapper.h"

namespace fileIdentifier
{
typedef luacpp::luatable luatable;
LuaFileObjectWrapper::LuaFileObjectWrapper(const std::string& luaFileName,
                                           script::ScriptCenterPtr script): typeTag_(""),
                                           luaFileName_(luaFileName), script_(script)
{
    script_->execute(luaFileName_);

    std::string::size_type startPos = luaFileName_.find_last_of("/") + 1;
    std::string::size_type endPos = luaFileName_.find_last_of(".");
    typeTag_ = luaFileName_.substr(startPos, endPos - startPos);

    luacpp::reg_cclass<DateEntry>::_reg(script_->luaState(), "DateEntry")
        .constructor<void>()
        .constructor<bool, int, int, int, int, int, int, int>()
        .property("exist",&DateEntry::exist)
        .property("year",&DateEntry::year)
        .property("month",&DateEntry::month)
        .property("day",&DateEntry::day)
        .property("hour",&DateEntry::hour)
        .property("minute",&DateEntry::minute)
        .property("second",&DateEntry::second)
        .property("mSecond",&DateEntry::mSecond);

    luacpp::reg_cclass<ElementEntry>::_reg(script_->luaState(), "ElementEntry")
        .constructor<void>()
        .property("moduleName",&ElementEntry::moduleName)
        .property("debugLevel",&ElementEntry::debugLevel)
        .property("threadId",&ElementEntry::threadId)
        .property("functionName",&ElementEntry::functionName)
        .property("sourceFilePath",&ElementEntry::sourceFilePath)
        .property("logString",&ElementEntry::logString);
}

bool LuaFileObjectWrapper::isMyType(const std::string &fileName)
{
    try
    {
        std::string funcName = typeTag_ + "_isMyType";
        return luacpp::call<bool>(script_->luaState(), funcName.c_str(), fileName);
    }
    catch(std::string &err)
    {
        std::cout << "cannot find " << err << std::endl;
    }

    return false;
}

const std::string LuaFileObjectWrapper::getTypeName()
{

    try
    {
        std::string funcName = typeTag_ + "_getTypeName";
        return luacpp::call<std::string>(script_->luaState(), funcName.c_str());
    }
    catch(std::string &err)
    {
        std::cout << "cannot find " << err << std::endl;
    }
    return "";
}

DateEntry LuaFileObjectWrapper::getLineTime(const std::string &lineStr)
{
    try
    {
        std::string funcName = typeTag_ + "_getLineTime";
        return luacpp::call<DateEntry>(script_->luaState(), funcName.c_str(), lineStr);
    }
    catch(std::string &err)
    {
        std::cout << "cannot find " << err << std::endl;
    }
    return DateEntry();
}

ElementEntry LuaFileObjectWrapper::getElement(const std::string &lineStr)
{
    try
    {
        std::string funcName = typeTag_ + "_getElement";
        return luacpp::call<ElementEntry>(script_->luaState(), funcName.c_str(), lineStr);
    }
    catch(std::string &err)
    {
        std::cout << "cannot find " << err << std::endl;
    }
    return ElementEntry();
}

}
