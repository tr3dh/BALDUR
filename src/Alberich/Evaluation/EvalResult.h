#pragma once

#include "../Objects/Variable.h"

struct EvalResult {

    //
    const static EvalResult nullRef;

    //
    Variable variable;
    Variable* variablePtr = nullptr;

    //
    EvalResult() = default;

    //
    EvalResult(IObject* objectPtr){

        variable.constructByUniquePtr(objectPtr->clone());
    }

    void copy(EvalResult& other){

        RETURNING_ASSERT(other.isValid(), "",);

        if(other.isRValue()){
            cloneIntoRValue(other.getVariableRef());
        }
        else if(other.isLValue()){
            setLValue(&other.getVariableRef());
        }
    }

    void reference(EvalResult& other){

        RETURNING_ASSERT(other.isValid(), "",);

        // 
        if(other.isRValue()){ // && other.getVariableRef().isReference()){
            
            variable.reference(other.getVariableRef());
        }
        //
        else if(other.isLValue()){
            setLValue(&other.getVariableRef());
        }
    }

    //
    void moveIntoRValue(Variable& varIn){

        variable.move(varIn);
    }

    //
    void cloneIntoRValue(Variable& varIn){

        variable.clone(varIn);
    }

    //
    void constructRValueByObject(IObject* object){

        variable.constructByObject(object);
    }

    // //
    // void constructRValueByUniqueObject(std::unique_ptr<IObject> object){

    //     variable.constructByUniquePtr(std::move(object));
    // }

    //
    void constructRValueByContainedLValue(){

        if(isRValue()){
            return;
        }

        cloneIntoRValue(*variablePtr);
        variablePtr = nullptr;
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

    TypeIndex getTypeIndex(){

        return getVariableRef().getData()->getTypeIndex();
    }

    IObject* getData(){

        return getVariableRef().getData();
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