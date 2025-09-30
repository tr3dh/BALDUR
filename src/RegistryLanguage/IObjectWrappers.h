#pragma once

#include "TypeRegister.h"
#include "FunctionRegister.h"

// globales TypeRegister das Direkt von IObjectWrappern verwendet wird >> keine konkrete Angabe des Registers bei
// Typedefinition nötig
extern TypeRegister g_TypeRegister;

// globales FunctionRegister das Direkt von IObjectWrappern verwendet wird >> keine konkrete Angabe des Registers bei
// Typedefinition nötig
extern FunctionRegister g_FunctionRegister;

// globales FunctionRegister das Direkt von IObjectWrappern verwendet wird >> keine konkrete Angabe des Registers bei
// Typedefinition nötig
// hier das pair für Member und statics Funktionen
// extern std::map<___, std::pair<FunctionRegister, FunctionRegister>> g_MemberFunctionRegisters;

// // Der Tag dient hier als eine Art Signatur
// // Dadurch das er gedefaultet ist kann die Klasse verwendet werden ohne einen eigenen Tag zu erstellen
// // Wenn aber ein zwei Native Objects für das selbe T benötigt werden, und mit unterschiedlicher TypeId
// // kann durch den Tag das erzeuge einer zweiten template Ausführung erzwungen werden
// template<typename T, typename Tag=void>
// class NativeObjectBase : public ObjectBase{

// public:

//     static TypeIndex typeIndex;

//     T member;
//     T* memberPtr;

//     bool isReference(){

//         return memberPtr != nullptr; 
//     }

//     T& getMember(){

//         return isReference() ? *memberPtr : member;
//     }
    
//     const T& getMember() const{

//         return isReference() ? *memberPtr : member;
//     }

//     static bool init(const std::string& keyword, const std::function<ObjectBase*()>& initConstructor, TypeRegister& tpReg = g_TypeRegister){

//         typeIndex = tpReg.registerType<NativeObjectBase<T>>(keyword, initConstructor);

//         LOG << "NativeObject wird unter TypeID " << typeIndex << " mit Keyword '" << keyword << "' registriert" << endl;
//         return typeIndex != -1;
//     }

//     TypeIndex getTypeIndex() override{
//         return typeIndex;
//     }
// };

// template<typename T, typename Tag>
// TypeIndex NativeObjectBase<T, Tag>::typeIndex = -1;

// class StrObject : public NativeObjectBase<std::string>{

// public:

//     // Existiert nur damit bei Zuweisung initialisierungsfunktionen ausgeführt werden
//     static bool initialized;
// };

// bool StrObject::initialized = StrObject::init("str", [](){ return new StrObject(); });