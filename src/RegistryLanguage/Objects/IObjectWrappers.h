#pragma once

#include "../LanguageRegister/RegisterInstances.h"

// Interface die Indizierung eines TypeFrontends festlegt
// 
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

    const std::string& getTypeKeyword() const override{
        
        return getKeywordByTypeIndex(typeIndex); 
    };

    size_t getSize() const override{

        return 0;
    }

    static bool init(const std::string& keyword, const std::function<IObject*()>& initConstructor){

        typeIndex = registerType(keyword, initConstructor);

        if(typeIndex == INVALID_TYPE_INDEX){
            
            _ERROR << "Type Registrierung von Type '" <<  keyword << "' fehlgeschlagen" << endl;
            return false;
        }

        // Plätze in statics und memberfunktionsregistermaps anlegen
        g_StaticFunctionRegisters.emplace(typeIndex, FunctionRegister());
        g_MemberFunctionRegisters.emplace(typeIndex, FunctionRegister());
        
        //
        g_nullRefs.emplace(typeIndex, std::make_unique<Tag>());

        //
        return true;
    }
};

template<typename Tag>
TypeIndex IIndexedObject<Tag>::typeIndex = INVALID_TYPE_INDEX;

// # Wrapper Interface für native Typen
// - Interface für Wrap konkret EINER Instanz (T) eines Backendtypen fürs Frontend 
// - Hierbei kann Tag entweder eine Signatur sein (dummy) oder der tatsächliche Erbe
// - Die virtuelle clone Funktion geht hierbei davon aus, dass der Tag die valide Childklasse ist
// - ist dies nicht der Fall muss die clone Funktion in der Childklasse zwangsläufig überschrieben werden
template<typename Tag, typename T>
class INativeObject : public IIndexedObject<Tag>{

public:

    // Helper, Prüft ob T ein Container mit nicht kopierbaren Elementen ist
    template<typename U>
    struct is_problematic_container : std::false_type {};
    
    template<typename... Args>
    struct is_problematic_container<std::vector<Args...>> : std::true_type {};
    
    template<typename... Args>
    struct is_problematic_container<std::map<Args...>> : std::true_type {};
    
    template<typename U, std::size_t N>
    struct is_problematic_container<std::array<U, N>> : std::true_type {};

    bool IsManagingMemory = true;
    T* member;
    
    INativeObject(){
        member = new T();
    }

    INativeObject(const T& memberIn){
        member = new T(memberIn);
    }

    INativeObject(T* memberPtrIn) : member(memberPtrIn){
        IsManagingMemory = false;
    }

    ~INativeObject(){

        if(IsManagingMemory){
            delete member;
        }
    }

    T& getMember(){ return  *member; }
    const T& getMember() const{ return *member; }

    // virtual ist redundant, die print bleibt überscheibbar
    void print() const override{

        LOG << *member;
    }

    size_t getSize() const override{

        return sizeof(*member);
    }

    bool isUniform() override {

        return !IsManagingMemory;
    }

    //
    std::unique_ptr<IObject> clone() override {

        if constexpr (!is_problematic_container<T>::value) {

            // Standard
            // INativeObject<Tag, T>* newObj = new INativeObject<Tag, T>(*this);
            Tag* newObj = new Tag(*static_cast<Tag*>(this));

            newObj->IsManagingMemory = true;
            newObj->member = new T(*member);

            return std::unique_ptr<IObject>(newObj);
        }
        else {
            
            // Container muss manuell überschrieben werden
            RETURNING_ASSERT(TRIGGER_ASSERT, "Clone Funktion muss für diesen Typ manuell implementiert werden", nullptr);
        }
    }
};