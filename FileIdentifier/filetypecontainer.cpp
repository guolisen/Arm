#include "filetypecontainer.h"
#include "defaultfileobject.h"

namespace fileIdentifier {


FileTypeContainer::FileTypeContainer(ILuaFileObjectWrapper::Factory factory,
                             script::ScriptCenterPtr scriptCenter):
    factory_(factory), scriptCenter_(scriptCenter)
{
    registerFileType(std::make_shared<DefaultFileObject>());
}

void FileTypeContainer::registerFileType(FileObjectPtr object)
{
    // TODO: check exist
    fileObjectList_.push_back(object);
}

void FileTypeContainer::registerLuaFileType(const std::string &luaFileName)
{
    // TODO: check exist
    fileObjectList_.push_back(factory_(luaFileName, scriptCenter_));
}

bool FileTypeContainer::createFileObjectList()
{

    return true;
}

}
