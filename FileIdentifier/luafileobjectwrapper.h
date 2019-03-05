#ifndef LUAFILEOBJECTWRAPPER_H
#define LUAFILEOBJECTWRAPPER_H

#include <string>
#include <functional>
#include <memory>
#include <Script/IScriptCenter.h>
#include "ifileObject.h"

namespace fileIdentifier
{

class LuaFileObjectWrapper: public ILuaFileObjectWrapper
{
public:
    LuaFileObjectWrapper(const std::string& luaFileName,
                         script::ScriptCenterPtr script);
    static LuaFileObjectWrapper::Factory getFactory()
    {
        return [](const std::string& luaFileName,
                  script::ScriptCenterPtr script) -> FileObjectPtr
        {
            return std::make_shared<LuaFileObjectWrapper>(luaFileName, script);
        }; // NOLINT
    }

    virtual bool isMyType(const std::string& fileName) override;
    virtual const std::string getTypeName() override;
    virtual DateEntry getLineTime(const std::string &lineStr) override;
    virtual ElementEntry getElement(const std::string &lineStr) override;

private:
    std::string typeTag_;
    std::string luaFileName_;
    script::ScriptCenterPtr script_;
};
}
#endif // LUAFILEOBJECTWRAPPER_H
