#include "RegisterInstances.h"

// Typen Register
// >> enthält für keyword : zugehörige TypeIdx, nullConstructor
TypeRegister g_TypeRegister;

// Funktionsregister die gesamte Funktionsstruktur des Programms speichern
// enthalten Funktionen, Memberfunktionen, statische Klassenfunktionen
FunctionRegister g_FunctionRegister;
std::map<TypeIndex, FunctionRegister> g_MemberFunctionRegisters;
std::map<TypeIndex, FunctionRegister> g_StaticFunctionRegisters;

//
KeywordRegister g_KeywordRegister;

// Keyword Register für nicht Kontruktionskeywords, die Werte zurückgeben soll >> zb true, false, nullptr, nullstr, ...
std::map<TypeIndex, std::unique_ptr<IObject>> g_nullRefs;

// Register Funktionen
void registerFunction(const std::string& functionLabel, const std::vector<TypeIndex>& functionArgsTypes, const IObjectFunction& func, const std::vector<TypeIndex>& functionReturnTypes){

    g_FunctionRegister.registerFunction(functionLabel, functionArgsTypes, func, functionReturnTypes);
}

void registerMemberFunction(TypeIndex tpIdx, const std::string& functionLabel, const std::vector<TypeIndex>& functionArgsTypes, const IObjectFunction& func, const std::vector<TypeIndex>& functionReturnTypes){

    //
    RETURNING_ASSERT(tpIdx != INVALID_TYPE_INDEX, "Übergebener TypeIndex ist invalide",);

    RETURNING_ASSERT(g_MemberFunctionRegisters.contains(tpIdx), "Funktionsregister für TypeIndex nicht konstruiert",);
    g_MemberFunctionRegisters[tpIdx].registerFunction(functionLabel, functionArgsTypes, func, functionReturnTypes);
}

void registerStaticFunction(TypeIndex tpIdx, const std::string& functionLabel, const std::vector<TypeIndex>& functionArgsTypes, const IObjectFunction& func, const std::vector<TypeIndex>& functionReturnTypes){

    //
    RETURNING_ASSERT(tpIdx != INVALID_TYPE_INDEX, "Übergebener TypeIndex ist invalide",);

    RETURNING_ASSERT(g_StaticFunctionRegisters.contains(tpIdx), "Funktionsregister für TypeIndex nicht konstruiert",);
    g_StaticFunctionRegisters[tpIdx].registerFunction(functionLabel, functionArgsTypes, func, functionReturnTypes);
}

// Call Funktionen
void callFunction(const std::string& functionLabel, FREG_CALLARGS){

    g_FunctionRegister.callFunction(functionLabel, returns, functionParams, returnToScope, nullptr);
}

void callMemberFunction(const std::string& functionLabel, FREG_CALLARGS_WITH_MEMBER){

    TypeIndex tpIdx = member->getVariableRef().getData()->getTypeIndex();
    RETURNING_ASSERT(tpIdx != INVALID_TYPE_INDEX, "Übergebener TypeIndex ist invalide",);

    RETURNING_ASSERT(g_MemberFunctionRegisters.contains(tpIdx),
        "Memberfunktionen Register Map enthält keinen Eintrag für ID " + std::to_string(tpIdx),);

    g_MemberFunctionRegisters[tpIdx].callFunction(functionLabel, returns, functionParams, returnToScope, member);
}

void callStaticFunction(const std::string& typeLabel, const std::string& functionLabel, FREG_CALLARGS){

    RETURNING_ASSERT(g_TypeRegister.typeIndices.contains(typeLabel), "TypeRegister enthält keinen Typ für keyword " + typeLabel,);

    TypeIndex tpIdx = g_TypeRegister.typeIndices[typeLabel];
    callStaticFunction(tpIdx, functionLabel, returns, functionParams, returnToScope);
}

void callStaticFunction(TypeIndex tpIdx, const std::string& functionLabel, FREG_CALLARGS){

    RETURNING_ASSERT(tpIdx != INVALID_TYPE_INDEX, "Übergebener TypeIndex ist invalide",);
    
    RETURNING_ASSERT(g_StaticFunctionRegisters.contains(tpIdx),
        "Statische funktionen Register Map enthält keinen Eintrag für ID " + std::to_string(tpIdx),);
    
    g_StaticFunctionRegisters[tpIdx].callFunction(functionLabel, returns, functionParams, returnToScope, nullptr);
}

//
TypeIndex registerType(const std::string& keyword, const std::function<IObject*()>& initConstructor){

    return g_TypeRegister.registerType(keyword, initConstructor);
}

IObject* constructRegisteredType(const std::string& keyword){

    return g_TypeRegister.constructRegisteredType(keyword);
}

IObject* constructRegisteredType(TypeIndex typeIndex){

    return g_TypeRegister.constructRegisteredType(typeIndex);
}

bool typeForKeywordExists(const std::string& keyword){

    return g_TypeRegister.contains(keyword);
}

bool valueForKeywordExists(const std::string& keyword){

    return g_KeywordRegister.contains(keyword);
}

void registerKeyword(const std::string& keyword, IObject* object){

    g_KeywordRegister.registerKeyword(keyword, object);
}

IObject* constructFromKeyword(const std::string& keyword){

    return g_KeywordRegister.constructFromKeyword(keyword);
}

TypeIndex getTypeIndexByKeyword(const std::string& keyword){
    
    RETURNING_ASSERT(g_TypeRegister.typeIndices.contains(keyword), "Für Keyword ist kein Type registriert", 0);
    return g_TypeRegister.typeIndices[keyword];
}

const std::string& getKeywordByTypeIndex(TypeIndex typeIndex){

    RETURNING_ASSERT(g_TypeRegister.typeInfos.contains(typeIndex), "Unter Index ist kein Type registriert", "");
    return g_TypeRegister.typeInfos[typeIndex].keyword;
}