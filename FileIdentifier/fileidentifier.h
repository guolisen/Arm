#ifndef FILEIDENTIFIER_H
#define FILEIDENTIFIER_H

#include "ifileidentifier.h"
#include "ifiletypecontainer.h"

namespace fileIdentifier {

class FileIdentifier: public IFileIdentifier
{
public:
    explicit FileIdentifier(FileTypeContainerPtr fileContainer);

    virtual FileObjectPtr checkFileType(const std::string& fileName);
private:
    FileTypeContainerPtr fileContainer_;
};
}
#endif // FILEIDENTIFIER_H
