#pragma once

#include "defines.h"

enum class Type : uint8_t{

    NONE,
    BOOL,
    INT,
    UNSIGNED_INT,
    FLOAT,
    DOUBLE,
    STRING,
    VECTOR,
    MAP
};

std::ostream& operator<<(std::ostream& os, const Type& tp);