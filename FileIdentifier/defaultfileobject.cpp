#include "defaultfileobject.h"

namespace fileIdentifier {


DefaultFileObject::DefaultFileObject()
{

}

const std::string DefaultFileObject::getTypeName()
{
    return "Default";
}

DateEntry DefaultFileObject::getLineTime(const std::string &lineStr)
{
    return DateEntry();
}

ElementEntry DefaultFileObject::getElement(const std::string &lineStr)
{
    return ElementEntry();
}

}
