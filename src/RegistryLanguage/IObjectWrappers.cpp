#include "IObjectWrappers.h"

// Typen Register
// >> enthält für keyword : zugehörige TypeIdx, nullConstructor
TypeRegister g_TypeRegister;

// Funktionsregister die gesamte Funktionsstruktur des Programms speichern
// enthalten Funktionen, Memberfunktionen, statische Klassenfunktionen
FunctionRegister g_FunctionRegister;
std::map<TypeIndex, FunctionRegister> g_MemberFunctionRegisters;
std::map<TypeIndex, FunctionRegister> g_StaticFunctionRegisters;

// Register Funktionen
void registerFunction(const std::string& functionLabel, const std::vector<TypeIndex>& functionArgsTypes, const IObjectFunction& func){

    g_FunctionRegister.registerFunction(functionLabel, functionArgsTypes, func);
}

void registerMemberFunction(TypeIndex tpIdx, const std::string& functionLabel, const std::vector<TypeIndex>& functionArgsTypes, const IObjectFunction& func){

    //
    RETURNING_ASSERT(tpIdx != INVALID_TYPE_INDEX, "Übergebener TypeIndex ist invalide",);

    RETURNING_ASSERT(g_MemberFunctionRegisters.contains(tpIdx), "Funktionsregister für TypeIndex nicht konstruiert",);
    g_MemberFunctionRegisters[tpIdx].registerFunction(functionLabel, functionArgsTypes, func);
}

void registerStaticFunction(TypeIndex tpIdx, const std::string& functionLabel, const std::vector<TypeIndex>& functionArgsTypes, const IObjectFunction& func){

    //
    RETURNING_ASSERT(tpIdx != INVALID_TYPE_INDEX, "Übergebener TypeIndex ist invalide",);

    RETURNING_ASSERT(g_StaticFunctionRegisters.contains(tpIdx), "Funktionsregister für TypeIndex nicht konstruiert",);
    g_StaticFunctionRegisters[tpIdx].registerFunction(functionLabel, functionArgsTypes, func);
}

// Call Funktionen
void callFunction(const std::string& functionLabel, std::vector<IObject*>& returns, const std::vector<IObject*>& functionParams){

    g_FunctionRegister.callFunction(functionLabel, returns, functionParams, nullptr);
}

void callMemberFunction(const std::string& functionLabel, std::vector<IObject*>& returns, const std::vector<IObject*>& functionParams, IObject* member){

    TypeIndex tpIdx = member->getTypeIndex();
    RETURNING_ASSERT(tpIdx != INVALID_TYPE_INDEX, "Übergebener TypeIndex ist invalide",);

    RETURNING_ASSERT(g_MemberFunctionRegisters.contains(tpIdx),
        "Memberfunktionen Register Map enthält keinen Eintrag für ID " + std::to_string(tpIdx),);

    g_MemberFunctionRegisters[tpIdx].callFunction(functionLabel, returns, functionParams, member);
}

void callStaticFunction(const std::string& typeLabel, const std::string& functionLabel, std::vector<IObject*>& returns, const std::vector<IObject*>& functionParams){

    RETURNING_ASSERT(g_TypeRegister.typeInfos.contains(typeLabel), "TypeRegister enthält keinen Typ für keyword " + typeLabel,);

    TypeIndex tpIdx = g_TypeRegister.typeInfos[typeLabel].typeIndex;
    callStaticFunction(tpIdx, functionLabel, returns, functionParams);
}

void callStaticFunction(TypeIndex tpIdx, const std::string& functionLabel, std::vector<IObject*>& returns, const std::vector<IObject*>& functionParams){

    RETURNING_ASSERT(tpIdx != INVALID_TYPE_INDEX, "Übergebener TypeIndex ist invalide",);
    
    RETURNING_ASSERT(g_StaticFunctionRegisters.contains(tpIdx),
        "Statische funktionen Register Map enthält keinen Eintrag für ID " + std::to_string(tpIdx),);
    
    g_StaticFunctionRegisters[tpIdx].callFunction(functionLabel, returns, functionParams, nullptr);
}