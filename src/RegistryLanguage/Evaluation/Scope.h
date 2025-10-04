#pragma once

#include "../Objects/Variable.h"
#include "../LanguageRegister/RegisterInstances.h"

typedef std::map<std::string, Variable> VariableTable;

struct Scope {

    //
    VariableTable variableTable = {};

    //
    void constructVariable(const std::string& variableName, const std::string& typeKeyword){

        if(getVariable(variableName) != nullptr){

            _ERROR << "Variable " << variableName << " ist bereits im Scope vorhanden" << endl;
        }

        variableTable.try_emplace(variableName);
        variableTable[variableName].constructByObject(constructRegisteredType(typeKeyword));
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

    //
    Scope* parent = nullptr;
    std::vector<Scope> children;

    //
    friend std::ostream& operator<<(std::ostream& os, const Scope& scope){

        os << scope.variableTable;
        return os;
    }
};