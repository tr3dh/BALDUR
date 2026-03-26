#include "IObject.h"

TypeIndex IObject::ARBITATRY_TYPE = 0;
TypeIndex IObject::ARGS_TYPE = 1;

std::ostream& operator<<(std::ostream& os, IObject* obj){

    obj->print();
    return os;
}