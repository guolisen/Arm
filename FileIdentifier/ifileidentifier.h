#ifndef IFILEIDENTIFIER_H
#define IFILEIDENTIFIER_H

#include <memory>
#include <QObject>
#include "ifileObject.h"

namespace fileIdentifier {

class IFileIdentifier: public QObject
{
    Q_OBJECT
public:
    virtual ~IFileIdentifier() = default;
    virtual FileObjectPtr checkFileType(const std::string& fileName) = 0;
};

typedef std::shared_ptr<IFileIdentifier> FileIdentifierPtr;

}
#endif // IFILEIDENTIFIER_H
