#include "fileidentifier.h"
#include "filetypecontainer.h"

namespace fileIdentifier {

FileIdentifier::FileIdentifier(FileTypeContainerPtr fileContainer):
    fileContainer_(fileContainer)
{
}

FileObjectPtr FileIdentifier::checkFileType(const std::string& fileName)
{
    auto objList = fileContainer_->getFileObjectList();
    for (auto object : objList)
    {
        if (object->isMyType(fileName))
            return object;
    }
    return FileObjectPtr();
}

}
