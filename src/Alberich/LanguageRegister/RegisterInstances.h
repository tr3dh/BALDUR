#pragma once

#include "TypeRegister.h"
#include "FunctionRegister.h"
#include "KeywordRegister.h"

// globales TypeRegister das Direkt von IObjectWrappern verwendet wird >> keine konkrete Angabe des Registers bei
// Typedefinition nötig
extern TypeRegister g_TypeRegister;

// globales FunctionRegister das Direkt von IObjectWrappern verwendet wird >> keine konkrete Angabe des Registers bei
// Typedefinition nötig
extern FunctionRegister g_FunctionRegister;

// globales FunctionRegister das Direkt von IObjectWrappern verwendet wird >> keine konkrete Angabe des Registers bei
// Typedefinition nötig
// jeweils eines für Member und statics Funktionen
extern std::map<TypeIndex, FunctionRegister> g_MemberFunctionRegisters;
extern std::map<TypeIndex, FunctionRegister> g_StaticFunctionRegisters;

// Keyword Register für nicht Kontruktionskeywords, die Werte zurückgeben soll >> zb true, false, nullptr, nullstr, ...
extern KeywordRegister g_KeywordRegister;

// Keyword Register für nicht Kontruktionskeywords, die Werte zurückgeben soll >> zb true, false, nullptr, nullstr, ...
extern std::map<TypeIndex, std::unique_ptr<IObject>> g_nullRefs;

// statische und Attribut Keyword Register analog zu den Funktionen implementieren
// ...
// extern std::map<TypeIndex, KeywordRegister> g_MemberAttribRegister;
// extern std::map<TypeIndex, KeywordRegister> g_StaticAttribRegister;

//
void registerFunction(const std::string& functionLabel, const std::vector<TypeIndex>& functionArgsTypes, const IObjectFunction& func, const std::vector<TypeIndex>& functionReturnTypes);
void registerMemberFunction(TypeIndex tpIdx, const std::string& functionLabel, const std::vector<TypeIndex>& functionArgsTypes, const IObjectFunction& func, const std::vector<TypeIndex>& functionReturnTypes);
void registerStaticFunction(TypeIndex tpIdx, const std::string& functionLabel, const std::vector<TypeIndex>& functionArgsTypes, const IObjectFunction& func, const std::vector<TypeIndex>& functionReturnTypes);

//
void callFunction(const std::string& functionLabel, FREG_DEFAULT_CALLARGS);
void callMemberFunction(const std::string& functionLabel, FREG_CALLARGS_WITH_MEMBER);
void callStaticFunction(const std::string& typeLabel, const std::string& functionLabel, FREG_DEFAULT_CALLARGS);
void callStaticFunction(TypeIndex tpIdx, const std::string& functionLabel, FREG_DEFAULT_CALLARGS);

//
TypeIndex registerType(const std::string& keyword, const std::function<IObject*()>& initConstructor);
IObject* constructRegisteredType(const std::string& keyword);
IObject* constructRegisteredType(TypeIndex typeIndex);
bool typeForKeywordExists(const std::string& keyword);
TypeIndex getTypeIndexByKeyword(const std::string& keyword);
const std::string& getKeywordByTypeIndex(TypeIndex typeIndex);

//
bool valueForKeywordExists(const std::string& keyword);
void registerKeyword(const std::string& keyword, IObject* object);
IObject* constructFromKeyword(const std::string& keyword);

// Wrapper für static and attribut keywords
// >> static attribute über Keyword Register map mit TypeIndex als schlüssel
// bei oop greift die map dann auf die unter den keys hinterlegten attributnamen zu