#pragma once

#include "Value.h"

typedef std::map<std::string, Value> VariableTable;

struct Scope {

    //
    VariableTable variableTable = {};

    //
    void constructVariable(const std::string& variableName, Type tp){

        if(getVariable(variableName) != nullptr){

            _ERROR << "Variable " << variableName << " ist bereits im Scope vorhanden" << endl;
        }

        variableTable.try_emplace(variableName, tp);
    }

    template<typename T>
    void setVariable(const std::string& variableName, const T& member){

        Value* variablePtr = getVariable(variableName);

        if(variablePtr == nullptr){

            _ERROR << "Variable " << variableName << " wurde in vorhandener Scope Hierarchie nicht gefunden" << endl;
            return;
        }

        *variablePtr = member;
    }

    //
    Value* getVariable(const std::string& variableName){

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