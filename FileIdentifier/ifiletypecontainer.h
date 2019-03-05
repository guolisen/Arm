#ifndef IFILECONTAINER_H
#define IFILECONTAINER_H

#include <list>
#include <memory>
#include "ifileObject.h"

namespace fileIdentifier {

typedef std::list<FileObjectPtr> FileObjectList;

class IFileTypeContainer
{
public:
    virtual ~IFileTypeContainer() = default;
    virtual void registerFileType(FileObjectPtr object) = 0;
    virtual void registerLuaFileType(const std::string& luaFileName) = 0;
    virtual bool createFileObjectList() = 0;
    virtual FileObjectList& getFileObjectList() = 0;
};

typedef std::shared_ptr<IFileTypeContainer> FileTypeContainerPtr;

}
#endif // IFILECONTAINER_H
