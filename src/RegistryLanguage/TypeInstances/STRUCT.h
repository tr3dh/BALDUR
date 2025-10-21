#pragma once

#include "gatheredInstances.h"

class STRUCT : public IObject{

public:

    static std::map<TypeIndex, Scope> staticScopes;
    static std::map<TypeIndex, Scope> attribScopes;

    static void cleanUp(){

        staticScopes.clear();
        attribScopes.clear();
    }

    static Scope& emplaceScopes(TypeIndex tpIdx, Scope& scope){
        
        staticScopes.try_emplace(tpIdx);
        Scope& staticScope = staticScopes[tpIdx];
        staticScope.parent = &scope;

        STRUCT::attribScopes.try_emplace(tpIdx);
        Scope& attribScope = attribScopes[tpIdx];
        attribScope.parent = &staticScope;

        return attribScope;
    }

    static void registerStruct(const std::string& keyword, TypeIndex tpIdx){

        //
        RETURNING_ASSERT(staticScopes.contains(tpIdx) && attribScopes.contains(tpIdx),
            "für TypeIndex hat noch kein Scope emplace stattgefunden",);

        //
        TypeIndex registeredTypeIdx = registerType(keyword, [keyword, tpIdx](){

            //
            STRUCT* member = new STRUCT(); 

            // attribScope in member kopieren
            member->typeIndex = tpIdx;
            member->attribScope.copyFrom(attribScopes[tpIdx]);
            
            //
            return member;
        });

        RETURNING_ASSERT(registeredTypeIdx == tpIdx, "",);

        // emplace in die nullRefs
        g_nullRefs.try_emplace(registeredTypeIdx).first->second.reset(constructRegisteredType(registeredTypeIdx));
    }

    // attribs
    
    // gespeicherter TypeIndex
    TypeIndex typeIndex;

    // eigentliche Attribs des StructMembers
    Scope attribScope;

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

        //
        auto it = &g_TypeRegister.typeInfos[typeIndex];

        //
        LOG << "STRUCT Object : '" << it->keyword << "' | TypeIdx : <" << typeIndex << "> | Attribs : " << attribScope;
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