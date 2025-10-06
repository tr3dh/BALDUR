#pragma once

#include "IObject.h"

struct Variable{

    std::unique_ptr<IObject> ownedObject = nullptr;
    IObject* referencedObject = nullptr;

    Variable() = default;

    void constructByObject(IObject* object) {

        ownedObject.reset(object);              // Ownership übernehmen
        referenceSelf();
    }

    void constructByUniquePtr(std::unique_ptr<IObject> uniqueObjectPtr){

        ownedObject = std::move(uniqueObjectPtr);
        referenceSelf();
    }

    // selbst Referenzierung
    // in Folge können Referenzen und Owner in gleicher Art und Weise auf den owned Value (unique_ptr) zugreifen
    // macht Auswertung einfach konsistenter
    void referenceSelf(){

        RETURNING_ASSERT(isValid(), "MM Semantik für Invalide Variable aufgerufen",);
        RETURNING_ASSERT(ownedObject != nullptr, "Selbstreferenzierung ohne Ownership eines Wertes versucht",);
        referencedObject = ownedObject.get();
    }

    void dereferenceSelf(){

        referencedObject = nullptr;
    }

    void move(Variable& other){
        
        RETURNING_ASSERT(other.isValid(), "MM Semantik für Invalide Variable aufgerufen",);
        RETURNING_ASSERT(!isReference() && !other.isReference(), "Move von Referenz Objekten versucht",);
        ownedObject = std::move(other.ownedObject);

        other.dereferenceSelf();
        referenceSelf();
    }

    void swap(Variable& other){
        
        RETURNING_ASSERT(isValid() && other.isValid(), "MM Semantik für Invalide Variable aufgerufen",);
        RETURNING_ASSERT(!isReference() && !other.isReference(), "Move von Referenz Objekten versucht",);
        std::swap(ownedObject, other.ownedObject);

        referenceSelf();
        other.referenceSelf();
    }

    void clone(const Variable& other){

        RETURNING_ASSERT(other.isValid(), "MM Semantik für Invalide Variable aufgerufen",);
        ownedObject = other.getData()->clone();
        referenceSelf();
    }

    void reference(const Variable& other){
        
        RETURNING_ASSERT(other.isValid(), "MM Semantik für Invalide Variable aufgerufen",);
        referencedObject = other.getData();
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

    IObject* getData() const{

        // nicht nötig aufgrund von Eigenref
        // if(!isReference()){
        //     return ownedObject.get();
        // }

        return referencedObject;
    }

    friend std::ostream& operator<<(std::ostream& os, const Variable& var){

        if(!var.isValid()){ 

            LOG << "INVALID_VARIABLE";
            return os;
        }

        LOG << "[" << var.referencedObject->getTypeIndex();

        if(var.isReference()){

            LOG << "Ref] ";
        }
        else{

            LOG << "   ] ";
        }

        var.getData()->print();

        return os;
    }
};