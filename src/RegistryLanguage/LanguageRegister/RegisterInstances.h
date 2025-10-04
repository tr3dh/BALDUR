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

//
void registerFunction(const std::string& functionLabel, const std::vector<TypeIndex>& functionArgsTypes, const IObjectFunction& func);
void registerMemberFunction(TypeIndex tpIdx, const std::string& functionLabel, const std::vector<TypeIndex>& functionArgsTypes, const IObjectFunction& func);
void registerStaticFunction(TypeIndex tpIdx, const std::string& functionLabel, const std::vector<TypeIndex>& functionArgsTypes, const IObjectFunction& func);

//
void callFunction(const std::string& functionLabel, std::vector<IObject*>& returns, const std::vector<IObject*>& functionParams);
void callMemberFunction(const std::string& functionLabel, std::vector<IObject*>& returns, const std::vector<IObject*>& functionParams, IObject* member);
void callStaticFunction(const std::string& typeLabel, const std::string& functionLabel, std::vector<IObject*>& returns, const std::vector<IObject*>& functionParams);
void callStaticFunction(TypeIndex tpIdx, const std::string& functionLabel, std::vector<IObject*>& returns, const std::vector<IObject*>& functionParams);

//
TypeIndex registerType(const std::string& keyword, const std::function<IObject*()>& initConstructor);
IObject* constructRegisteredType(const std::string& keyword);
bool typeForKeywordExists(const std::string& keyword);

bool valueForKeywordExists(const std::string& keyword);
void registerKeyword(const std::string& keyword, IObject* object);
IObject* constructFromKeyword(const std::string& keyword);