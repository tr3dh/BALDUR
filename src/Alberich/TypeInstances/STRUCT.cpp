#include "STRUCT.h"

std::map<TypeIndex, Scope> STRUCT::attribScopes = {};

bool STRUCT::containsVariable(Variable* variablePtr){

    return attribScope.containsVariable(variablePtr);
}

std::pair<bool, Variable*> STRUCT::containsDataReference(IObject* dataPtr){

    return attribScope.containsDataReference(dataPtr);
}

std::pair<bool, Variable*> STRUCT::containsDataVariableOrReference(IObject* dataPtr){

    return attribScope.containsDataVariableOrReference(dataPtr);
}