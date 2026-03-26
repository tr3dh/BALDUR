#pragma once

#include "gatheredInstances.h"

class STRUCT : public IObject{

public:

    static std::map<TypeIndex, Scope> attribScopes;

    static void cleanUp(){

        attribScopes.clear();
    }

    static Scope& emplaceScopes(TypeIndex tpIdx, Scope& scope){
        
        g_staticScopes.try_emplace(tpIdx);
        Scope& staticScope = g_staticScopes[tpIdx];
        staticScope.parent = &scope;

        STRUCT::attribScopes.try_emplace(tpIdx);
        Scope& attribScope = attribScopes[tpIdx];
        attribScope.parent = &staticScope;

        g_StaticFunctionRegisters.try_emplace(tpIdx);
        g_MemberFunctionRegisters.try_emplace(tpIdx);

        g_StaticFunctionRegisters[tpIdx] = FunctionRegister();
        g_MemberFunctionRegisters[tpIdx] = FunctionRegister();

        return attribScope;
    }

    static void registerStruct(const std::string& keyword, TypeIndex tpIdx){

        //
        RETURNING_ASSERT(g_staticScopes.contains(tpIdx) && attribScopes.contains(tpIdx),
            "für TypeIndex hat noch kein Scope emplace stattgefunden",);

        //
        TypeIndex registeredTypeIdx = registerType(keyword, [keyword, tpIdx](){

            //
            STRUCT* member = new STRUCT(); 

            // attribScope in member kopieren
            member->typeIndex = tpIdx;
            member->tpInf = &g_TypeRegister.typeInfos[tpIdx];
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
    TypeInfo* tpInf = nullptr;

    STRUCT() = default;

    STRUCT(STRUCT& other){

        typeIndex = other.typeIndex;
        tpInf = other.tpInf;

        attribScope.copyFrom(other.attribScope);
    }

    TypeIndex getTypeIndex() const override{

        return typeIndex;
    }

    const std::string& getTypeKeyword() const override{
        
        return tpInf->keyword; 
    };

    size_t getSize() const override{

        return 0;
    }

    void print() const override{

        //
        auto it = &g_TypeRegister.typeInfos[typeIndex];

        //
        LOG << "STRUCT Object : '" << it->keyword << "' | TypeIdx : <" << typeIndex << "> | Attribs : {" << attribScope << "}";
    }

    //
    std::unique_ptr<IObject> clone() override {

        return std::make_unique<STRUCT>(*this); 
    }

    Variable* getAttrib(const std::string& attribLabel) override{

        //
        RETURNING_ASSERT(attribScope.containsVariableInline(attribLabel),
            "Struct " + tpInf->keyword + " stellt kein Attrib " + attribLabel+ " bereit", nullptr);

        return &attribScope.variableTable[attribLabel];
    }

    bool containsVariable(Variable* variablePtr) override;

    std::pair<bool, Variable*> containsDataReference(IObject* dataPtr) override;

    std::pair<bool, Variable*> containsDataVariableOrReference(IObject* dataPtr) override;

    bool isTrivial() override { return false; }
};