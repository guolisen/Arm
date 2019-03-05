#ifndef FILECONTAINER_H
#define FILECONTAINER_H

#include <map>
#include <string>
#include <Script/IScriptCenter.h>
#include "ifiletypecontainer.h"
#include "ifileObject.h"

namespace fileIdentifier {

class FileTypeContainer: public IFileTypeContainer
{
public:
    FileTypeContainer(ILuaFileObjectWrapper::Factory factory,
                  script::ScriptCenterPtr scriptCenter);
    virtual void registerFileType(FileObjectPtr object) override;
    virtual void registerLuaFileType(const std::string& luaFileName) override;
    virtual bool createFileObjectList() override;
    virtual FileObjectList& getFileObjectList() override
    {
        return fileObjectList_;
    }

private:
    FileObjectList fileObjectList_;
    ILuaFileObjectWrapper::Factory factory_;
    script::ScriptCenterPtr scriptCenter_;
};

}
#endif // FILECONTAINER_H
