#pragma once

// File enthält die Deklaration des Interfaces, dass für die polymorphe Sprachstruktur erforderlich ist
// Interfache :                 IObject
// Polymorphe Nutzung über :    IObject*, unique_ptr<IObject>

#include "defines.h"

// Typeindex über den registrierte Interface Erben von IObject identifiziert werden können
typedef uint16_t TypeIndex;

constexpr TypeIndex INVALID_TYPE_INDEX = std::numeric_limits<uint16_t>::max();

class IObject{

public:

    virtual ~IObject() = default;
    virtual void print() const = 0;
    virtual TypeIndex getTypeIndex() const = 0;
    virtual std::unique_ptr<IObject> clone() const = 0;
};