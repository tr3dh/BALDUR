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

    static TypeIndex ARBITATRY_TYPE;
    static TypeIndex ARGS_TYPE;

    virtual ~IObject() = default;
    virtual void print() const = 0;
    virtual TypeIndex getTypeIndex() const = 0;
    virtual const std::string& getTypeKeyword() const = 0;
    virtual size_t getSize() const = 0;
    virtual std::unique_ptr<IObject> clone() const = 0;
};

std::ostream& operator<<(std::ostream& os, IObject* obj);