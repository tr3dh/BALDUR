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

        refExists = true;
        while(refExists){

            std::tie(refExists, refPtr) = parent->containsVariableReference(&var);
    
            if(refExists){

                _ERROR << "Bei Scope Löschung existiert externe Referenz lokaler Variable " << label << endl;
                _ERROR << "entsprechende Referenz wird dereferenziert" << endl;

                refPtr->forceReference(&g_nullRefs[refPtr->getData()->getTypeIndex()]);
            }
        }
    }

    for(auto& [idx, scope] : g_staticScopes){

        for(auto& [label, var] : scope.variableTable){
        
            if(var.isReference()){ continue; }

            refExists = true;
            while(refExists){

                std::tie(refExists, refPtr) = parent->containsVariableReference(&var);
        
                if(refExists){

                    refPtr->reference(&g_nullRefs[refPtr->getData()->getTypeIndex()]);

                    _ERROR << "Bei Scope Löschung existiert externe Referenz lokaler Variable " << label << endl;
                    _ERROR << "entsprechende Referenz wird dereferenziert" << endl;
                }
            }
        }
    }

    // Löschung nach return dieser Funktion
    // ...
}

bool Scope::IsRootScope(){
   
    return parent == nullptr;
}

Scope* Scope::getRootScope(){

    // init ptr
    Scope* root = this;

    //
    while(!root->IsRootScope()){ root = root->parent; }

    return root;
}

//
void Scope::copyFrom(Scope& other){

    //
    parent = other.parent;

    //
    for(auto& [label, var] : other.variableTable){

        //
        RETURNING_ASSERT(var.isValid(), "Invalide Variable in Scope der kopiert werden soll",);

        //
        variableTable.try_emplace(label);

        if(var.isReference()){ variableTable[label].reference(var); }
        else{ variableTable[label].clone(var); }
    }
}

//
bool Scope::containsVariable(Variable* variablePtr){

    for(auto& [name, var] : variableTable){

        if(variablePtr == &var){
            return true;
        }

        if(var.getData()->containsVariable(variablePtr)){
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

    //
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

        if(var.getData()->isTrivial() && !var.isReference()){ continue; }

        if(var.getData() == dataPtr){

            return std::make_pair(true, &var);
        }

        auto pair = var.getData()->containsDataReference(dataPtr);
        if(pair.first){

            LOG << lb << " is containing "; dataPtr->print(); LOG << " at " << *pair.second << endl;
            return pair;
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
            return std::make_pair(true, &variableTable[lb]);
        }

        auto pair = var.getData()->containsDataVariableOrReference(dataPtr);
        if(pair.first){
            return pair;
        }
    }

    if(parent != nullptr){

        return parent->containsDataVariableOrReference(dataPtr);
    }

    return std::make_pair(false, nullptr);
}