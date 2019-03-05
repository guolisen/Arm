#ifndef DEFAULTFILEOBJECT_H
#define DEFAULTFILEOBJECT_H

#include "ifileObject.h"

namespace fileIdentifier {

class DefaultFileObject: public IFileObject
{
public:
    DefaultFileObject();

    virtual bool isMyType(const std::string& fileName) override
    {
        return false;
    }
    virtual const std::string getTypeName() override;
    virtual DateEntry getLineTime(const std::string &lineStr) override;
    virtual ElementEntry getElement(const std::string &lineStr) override;
};

}
#endif // DEFAULTFILEOBJECT_H
