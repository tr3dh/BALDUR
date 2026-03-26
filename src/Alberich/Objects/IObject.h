#pragma once

// File enthält die Deklaration des Interfaces, dass für die polymorphe Sprachstruktur erforderlich ist
// Interfache :                 IObject
// Polymorphe Nutzung über :    IObject*, unique_ptr<IObject>

#include "defines.h"

// Typeindex über den registrierte Interface Erben von IObject identifiziert werden können
typedef uint16_t TypeIndex;

constexpr TypeIndex INVALID_TYPE_INDEX = std::numeric_limits<TypeIndex>::max();

// fwddecl
struct Variable;

class IObject{

public:

    static TypeIndex ARBITATRY_TYPE;
    static TypeIndex ARGS_TYPE;

    virtual ~IObject() = default;
    virtual void print() const = 0;
    virtual TypeIndex getTypeIndex() const = 0;
    virtual const std::string& getTypeKeyword() const = 0;
    virtual size_t getSize() const = 0;
    virtual std::unique_ptr<IObject> clone() = 0;

    virtual Variable* getAttrib(const std::string& attribLabel){ return nullptr; }

    virtual bool containsVariable(Variable* variablePtr){ return false; }
    virtual std::pair<bool, Variable*> containsDataReference(IObject* dataPtr){

        return std::make_pair(false, nullptr);
    }

    virtual std::pair<bool, Variable*> containsDataVariableOrReference(IObject* dataPtr){

        return std::make_pair(false, nullptr);
    }

    virtual bool isUniform(){ return false; }
    virtual bool isTrivial(){ return true; }

    virtual void clear(){};

    virtual void cloneMember(std::unique_ptr<IObject>& other){ RETURNING_ASSERT(TRIGGER_ASSERT, "clone Member nicht implementiert",); };
    virtual void moveMember(std::unique_ptr<IObject>& other){ RETURNING_ASSERT(TRIGGER_ASSERT, "mov Member nicht implementiert",); };
    virtual void swapMembers(std::unique_ptr<IObject>& other){ RETURNING_ASSERT(TRIGGER_ASSERT, "swap Members nicht implementiert",); };
};

std::ostream& operator<<(std::ostream& os, IObject* obj);