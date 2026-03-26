#pragma once

#include "../Objects/Variable.h"
#include "../LanguageRegister/RegisterInstances.h"

typedef std::map<std::string, Variable> VariableTable;

extern std::map<TypeIndex, Scope> g_staticScopes;

struct Scope {

    //
    VariableTable variableTable = {};

    //
    Scope* parent = nullptr;

    //
    ~Scope();

    //
    void copyFrom(Scope& other);

    //
    bool IsRootScope();
    Scope* getRootScope();

    //
    void constructVariable(const std::string& variableName, const std::string& typeKeyword){

        if(getVariable(variableName) != nullptr){

            _ERROR << "Variable " << variableName << " ist bereits im Scope vorhanden" << endl;
        }

        variableTable.try_emplace(variableName);
        variableTable[variableName].constructByObject(constructRegisteredType(typeKeyword));
    }

    //
    void constructVariable(const std::string& variableName, TypeIndex typeIndex){

        if(variableTable.contains(variableName)){

            _ERROR << "Variable " << variableName << " ist bereits im Scope vorhanden" << endl;
        }

        variableTable.try_emplace(variableName);
        variableTable[variableName].constructByObject(constructRegisteredType(typeIndex));
    }

    Variable* constructAndReturnVariable(const std::string& variableName){

        if(variableTable.contains(variableName)){

            _ERROR << "Variable " << variableName << " ist bereits im Scope vorhanden" << endl;
        }

        variableTable.try_emplace(variableName);
        return getVariable(variableName);
    }

    void setVariable(const std::string& variableName, IObject* member){

        Variable* variablePtr = getVariable(variableName);

        if(variablePtr == nullptr){

            _ERROR << "Variable " << variableName << " wurde in vorhandener Scope Hierarchie nicht gefunden" << endl;
            return;
        }

        variablePtr->constructByObject(member);
    }

    //
    bool containsVariable(const std::string& variableName){

        return getVariable(variableName) != nullptr;
    }

    //
    bool containsVariableInline(const std::string& variableName){

        return variableTable.contains(variableName);
    }

    //
    bool containsVariable(Variable* variablePtr);

    //
    std::pair<bool, Variable*> containsVariableReference(Variable* variablePtr);

    //
    std::pair<bool, Variable*> containsDataReference(IObject* dataPtr);

    //
    std::pair<bool, Variable*> containsDataVariableOrReference(IObject* dataPtr);

    //
    Variable* getVariable(const std::string& variableName){

        //
        if(variableTable.contains(variableName)){

            return &variableTable[variableName];
        }
        else if(parent != nullptr){

            return parent->getVariable(variableName);
        }

        return nullptr;
    }

    bool eraseVariable(const std::string& variableName) {
    
        if (variableTable.contains(variableName)) {
            
            variableTable.erase(variableName);
            return true;
        }
        else if (parent != nullptr) {
            
            return parent->eraseVariable(variableName);
        }
        
        return false;
    }

    bool eraseVariable(Variable* ptr) {
        
        for (auto it = variableTable.begin(); it != variableTable.end(); ++it) {
            
            if (&it->second == ptr) {
                
                variableTable.erase(it);
                return true;
            }
        }
        
        if (parent != nullptr) {
            
            return parent->eraseVariable(ptr);
        }
        
        return false;
    }

    //
    friend std::ostream& operator<<(std::ostream& os, const Scope& scope){

        os << scope.variableTable;
        return os;
    }
};