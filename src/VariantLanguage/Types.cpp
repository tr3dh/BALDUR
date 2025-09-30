#include "Types.h"

std::ostream& operator<<(std::ostream& os, const Type& tp) {

    os << magic_enum::enum_name(tp);
    return os;
}