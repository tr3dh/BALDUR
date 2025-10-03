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
// jeweils eines für Member und statics Funktionen
extern std::map<TypeIndex, FunctionRegister> g_MemberFunctionRegisters;
extern std::map<TypeIndex, FunctionRegister> g_StaticFunctionRegisters;

//
void registerFunction(const std::string& functionLabel, const std::vector<TypeIndex>& functionArgsTypes, const IObjectFunction& func);
void registerMemberFunction(TypeIndex tpIdx, const std::string& functionLabel, const std::vector<TypeIndex>& functionArgsTypes, const IObjectFunction& func);
void registerStaticFunction(TypeIndex tpIdx, const std::string& functionLabel, const std::vector<TypeIndex>& functionArgsTypes, const IObjectFunction& func);

//
void callFunction(const std::string& functionLabel, std::vector<IObject*>& returns, const std::vector<IObject*>& functionParams);
void callMemberFunction(const std::string& functionLabel, std::vector<IObject*>& returns, const std::vector<IObject*>& functionParams, IObject* member);
void callStaticFunction(const std::string& typeLabel, const std::string& functionLabel, std::vector<IObject*>& returns, const std::vector<IObject*>& functionParams);
void callStaticFunction(TypeIndex tpIdx, const std::string& functionLabel, std::vector<IObject*>& returns, const std::vector<IObject*>& functionParams);

// Der Tag dient hier als eine Art Signatur
// Dadurch das er gedefaultet ist kann die Klasse verwendet werden ohne einen eigenen Tag zu erstellen
// Wenn aber ein zwei Native Objects für das selbe T benötigt werden, und mit unterschiedlicher TypeId
// kann durch den Tag das erzeuge einer zweiten template Ausführung erzwungen werden
template<typename Tag>
class IIndexedObject : public IObject{

public:

    static TypeIndex typeIndex;

    TypeIndex getTypeIndex() const override{
        return typeIndex;
    }

    static bool init(const std::string& keyword, const std::function<IObject*()>& initConstructor, TypeRegister& tpReg = g_TypeRegister){

        typeIndex = tpReg.registerType<IIndexedObject<Tag>>(keyword, initConstructor);

        if(typeIndex == INVALID_TYPE_INDEX){
            
            _ERROR << "Type Registrierung von Type '" <<  keyword << "' fehlgeschlagen" << endl;
            return false;
        }
        else{
            
            LOG << "TypeRegistrierung für Type '" << keyword << "' vorgenommen, zugewiesenen TypeID : " << typeIndex << endl;
        }

        // Plätze in statics und memberfunktionsregistermaps anlegen
        g_StaticFunctionRegisters.emplace(typeIndex, FunctionRegister());
        g_MemberFunctionRegisters.emplace(typeIndex, FunctionRegister());
        
        //
        return true;
    }
};

template<typename Tag>
TypeIndex IIndexedObject<Tag>::typeIndex = INVALID_TYPE_INDEX;

// Natives Object
template<typename Tag, typename T>
class INativeObject : public IIndexedObject<Tag>{

public:

    // Helper: Prüft ob T ein Container mit nicht-kopierbaren Elementen ist
    template<typename U>
    struct is_problematic_container : std::false_type {};
    
    template<typename... Args>
    struct is_problematic_container<std::vector<Args...>> : std::true_type {};
    
    template<typename... Args>
    struct is_problematic_container<std::map<Args...>> : std::true_type {};
    
    template<typename U, std::size_t N>
    struct is_problematic_container<std::array<U, N>> : std::true_type {};

    T member;
    
    INativeObject() = default;

    INativeObject(const T& memberIn) : member(memberIn){

    }

    T& getMember(){ return  member; }
    const T& getMember() const{ return member; }

    // virtual ist redundant, die print bleibt überscheibbar
    void print() const override{
        LOG << member;
    }

    //
    std::unique_ptr<IObject> clone() const override {

        if constexpr (!is_problematic_container<T>::value) {

            // Standard
            return std::make_unique<Tag>(static_cast<const Tag&>(*this));
        }
        else {
            
            // Container muss manuell überschrieben werden
            RETURNING_ASSERT(TRIGGER_ASSERT, "Clone Funktion muss für diesen Typ manuell implementiert werden", nullptr);
        }
    }
};