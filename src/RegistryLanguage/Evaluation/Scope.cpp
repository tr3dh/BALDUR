#include "Scope.h"

//
Scope::~Scope(){
    
    if(parent == nullptr){
        return;   
    }

    bool refExists;
    Variable* refPtr;

    for(auto& [label, var] : variableTable){
        
        if(var.isReference()){ continue; }

        std::tie(refExists, refPtr) = parent->containsVariableReference(&var);
    
        if(refExists){

            refPtr->reference(&g_nullRefs[refPtr->getData()->getTypeIndex()]);

            _ERROR << "Bei Scope Löschung existiert externe Referenz lokaler Variable " << label << endl;
            _ERROR << "entsprechende Referenz wird dereferenziert" << endl;
        }
    }

    // Löschung nach return dieser Funktion
    // ...
}

//
bool Scope::containsVariable(Variable* variablePtr){

    for(auto& [name, var] : variableTable){

        if(variablePtr == &var){
            return true;
        }
    }

    if(parent != nullptr){

        return parent->containsVariable(variablePtr);
    }

    return false;
}

//
std::pair<bool, Variable*> Scope::containsVariableReference(Variable* variablePtr){

    if(variablePtr->isReference()){
        return std::make_pair(false, nullptr);
    }

    //
    return containsDataReference(variablePtr->getData());
}

//
std::pair<bool, Variable*> Scope::containsDataReference(IObject* dataPtr){

    //
    for(auto& [lb, var] : variableTable){

        if(!var.isReference()){ continue; }

        if(var.getData() == dataPtr){
            return std::make_pair(true, &var);
        }
    }

    if(parent != nullptr){

        return parent->containsDataReference(dataPtr);
    }

    return std::make_pair(false, nullptr);
}

//
std::pair<bool, Variable*> Scope::containsDataVariableOrReference(IObject* dataPtr){

    //
    for(auto& [lb, var] : variableTable){

        if(var.getData() == dataPtr){
            return std::make_pair(true, &var);
        }
    }

    if(parent != nullptr){

        return parent->containsDataVariableOrReference(dataPtr);
    }

    return std::make_pair(false, nullptr);
}