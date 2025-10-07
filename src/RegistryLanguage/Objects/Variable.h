#pragma once

#include "IObject.h"

struct Variable{

    std::unique_ptr<IObject> ownedObject = nullptr;
    std::unique_ptr<IObject>* referencedObject = nullptr;

    Variable() = default;

    void constructByObject(IObject* object) {

        ownedObject.reset(object);
    }

    void constructByUniquePtr(std::unique_ptr<IObject> uniqueObjectPtr){

        ownedObject = std::move(uniqueObjectPtr);
    }

    void move(Variable& other){
        
        *getUniqueData() = std::move(*other.getUniqueData());
    }

    void swap(Variable& other){

        std::swap(*getUniqueData(), *other.getUniqueData());
    }

    void clone(Variable& other){

        RETURNING_ASSERT(other.isValid(), "MM Semantik für Invalide Variable aufgerufen",);
        *getUniqueData() = other.getData()->clone();
    }

    void reference(Variable& other){
        
        reference(other.getUniqueData());
    }

    void reference(std::unique_ptr<IObject>* other){
        
        referencedObject = other;
    }

    bool isValid() const {

        return !(ownedObject == nullptr && referencedObject == nullptr);
    }

    void inValidate(){

        ownedObject.reset(nullptr);
    }

    bool isReference() const {
        
        if(!isValid()){
            return false;
        }

        return ownedObject == nullptr;
    }

    // Für nicht-const
    std::unique_ptr<IObject>* getUniqueData() {

        return isReference() ? referencedObject : &ownedObject;
    }

    IObject* getData() {

        return getUniqueData()->get();
    }

    // Für const-Variable
    const std::unique_ptr<IObject>* getUniqueData() const {

        return isReference() ? referencedObject : &ownedObject;
    }

    const IObject* getData() const {

        return getUniqueData()->get();
    }

    friend std::ostream& operator<<(std::ostream& os, const Variable& var){

        if(!var.isValid()){ 

            LOG << "INVALID_VARIABLE";
            return os;
        }

        LOG << "[tID " << var.getData()->getTypeIndex();

        if(var.isReference()){

            LOG << " Ref] ";
        }
        else{

            LOG << "    ] ";
        }

        var.getData()->print();

        return os;
    }
};