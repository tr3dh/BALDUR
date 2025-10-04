#include "IObject.h"

std::ostream& operator<<(std::ostream& os, IObject* obj){

    os << "Type : " << obj->getTypeIndex() << " | Value <"; 
    obj->print();
    os << ">";
    return os;
}