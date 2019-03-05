//
// Created by Lewis on 2017/3/7.
//

#ifndef ELFBOX_ISCRIPTCENTER_H
#define ELFBOX_ISCRIPTCENTER_H

#include <memory>
#include <QObject>
#include <Thirdparty/LuaWrapper/luaWrapper.h>

namespace script
{
class IScriptCenter: public QObject
{
    Q_OBJECT
public:
    virtual ~IScriptCenter() = default;
    virtual bool initialize() = 0;
    virtual bool execute(const std::string &fileName) = 0;
    virtual bool executeString(const std::string &codeStr) = 0;
    virtual luacpp::luaWrapper& luaState() = 0;
};

typedef std::shared_ptr<IScriptCenter> ScriptCenterPtr;

}

#endif //ELFBOX_ISCRIPTCENTER_H
