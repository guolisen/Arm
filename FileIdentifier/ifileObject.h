#ifndef IFILEOBJECT_H
#define IFILEOBJECT_H

#include <functional>
#include <memory>
#include <string>
#include <Script/IScriptCenter.h>

namespace fileIdentifier {

struct ColumnEntry
{
    bool exist;
    int startPos;
    int endPos;
    std::string entryStr;
    ColumnEntry():exist(false), startPos(-1),
        endPos(-1), entryStr("") {}
};
struct DateEntry
{
    bool exist;
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int mSecond;
    DateEntry(): exist(false), year(1970), month(1), day(1),
        hour(1), minute(1), second(1), mSecond(1){}

    DateEntry(bool exist, int year, int month,
              int day, int hour, int minute, int second, int mSecond):
              exist(exist), year(year), month(month), day(day),
              hour(hour), minute(minute), second(second), mSecond(mSecond){}
};

struct ElementEntry
{
    std::string moduleName;
    std::string debugLevel;
    std::string threadId;
    std::string functionName;
    std::string sourceFilePath;
    std::string logString;

};

class IFileObject
{
    typedef std::function<std::shared_ptr<IFileObject>(const std::string& luaFileName,
                                        script::ScriptCenterPtr script)> Factory;
public:
    virtual ~IFileObject() = default;
    virtual bool isMyType(const std::string& fileName) = 0;
    virtual const std::string getTypeName() = 0;
    virtual DateEntry getLineTime(const std::string &lineStr) = 0;
    virtual ElementEntry getElement(const std::string &lineStr) = 0;
};

typedef std::shared_ptr<IFileObject> FileObjectPtr;

class ILuaFileObjectWrapper: public IFileObject
{
public:
    typedef std::function<std::shared_ptr<IFileObject>(const std::string& luaFileName,
                                        script::ScriptCenterPtr script)> Factory;

    virtual ~ILuaFileObjectWrapper() = default;

};


}
#endif // IFILEOBJECT_H
