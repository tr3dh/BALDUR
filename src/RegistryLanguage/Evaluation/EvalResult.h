#pragma once

#include "../Objects/Variable.h"
#include "../LanguageRegister/RegisterInstances.h"

struct EvalResult {

    //
    const static EvalResult nullRef;

    //
    Variable variable;
    Variable* variablePtr = nullptr;

    //
    EvalResult() = default;

    //
    void moveIntoRValue(Variable& varIn){

        variable.move(varIn);
    }

    //
    void cloneIntoRValue(const Variable& varIn){

        variable.clone(varIn);
    }

    //
    void constructRValueByObject(IObject* object){

        variable.constructByObject(object);
    }

    //
    void setLValue(Variable* ptrIn){

        variablePtr = ptrIn;
    }

    //
    bool isLValue() const{

        return variablePtr != nullptr;
    }

    bool isRValue() const{

        return !isLValue();
    }

    Variable& getVariableRef(){

        return isLValue() ? *variablePtr : variable;
    }

    const Variable& getVariableRef() const{

        return isLValue() ? *variablePtr : variable;
    }

    bool isValid() const{

        return variablePtr != nullptr || variable.isValid();
    }

    friend std::ostream& operator<<(std::ostream& os, EvalResult& res){

        if(!res.isValid()){
            
            os << "Invalid EvalRes";
            return os;
        }

        os << (res.isLValue() ? "Lvalue" : "Rvalue") << " Variable : " << res.getVariableRef();
        return os;
    }
};