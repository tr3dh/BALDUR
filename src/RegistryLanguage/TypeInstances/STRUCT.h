#pragma once

#include "gatheredInstances.h"

class STRUCT : public IObject{

    TypeIndex typeIndex;

    // auf eintrag in static map umstellen
    std::string keyword;

    TypeIndex getTypeIndex() const override{

        return typeIndex;
    }

    const std::string& getTypeKeyword() const override{
        
        return keyword; 
    };

    size_t getSize() const override{

        return 0;
    }

    void print() const override{

    }

    //
    std::unique_ptr<IObject> clone() const override {

        // Standard
        return std::make_unique<types::VOID>();
        
    }

    //
    // static bool init(const std::string& keyword, const std::function<IObject*()>& initConstructor){

    //     typeIndex = registerType(keyword, initConstructor);

    //     if(typeIndex == INVALID_TYPE_INDEX){
            
    //         _ERROR << "Type Registrierung von Type '" <<  keyword << "' fehlgeschlagen" << endl;
    //         return false;
    //     }

    //     // Plätze in statics und memberfunktionsregistermaps anlegen
    //     g_StaticFunctionRegisters.emplace(typeIndex, FunctionRegister());
    //     g_MemberFunctionRegisters.emplace(typeIndex, FunctionRegister());
        
    //     //
    //     g_nullRefs.emplace(typeIndex, std::make_unique<Tag>());

    //     //
    //     return true;
    // }
};