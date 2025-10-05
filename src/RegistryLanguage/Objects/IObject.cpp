#include "IObject.h"

TypeIndex IObject::ARBITATRY_TYPE = 0;

std::ostream& operator<<(std::ostream& os, IObject* obj){

    obj->print();
    return os;
}