#pragma once

#include "../Objects/IObject.h"

// Idee hinter dem TypeRegister
//
// Jeder zu implementierende Type also ein Erbe von ObjectBase wird in einem Register hinterlegt
// über dieses wird ihm zudem eine eindeutige ID vergeben
// Diese ID wird intern in der Childklasse gespeichert und kann über die virtuelle Elternfunktion getID || ... abgerufen werden
// damit wird in dem Funktionsregister die Überladung geregelt
// >> für jede Funktion sind dort die IDs der erwarteten Types hinterlegt und wenn die Funktion aufgerufen werden soll
//    checkt das register ob für den Name und für die übergebenen Parameter eine passende Funktion hinterlegt ist
// >> Dieser abgleich findet über die IDs statt

// Bereitstellen der ID in der Childklasse
// je nachdem ob der Type nativ (aus dem C++ Backend) oder per OOP vom User definiert worden ist unterscheidet sich die
// Bereitstellung
//
// . native Typen : ID kann als statisches Attribut des Childs hinterlegt werden
// . user Typen (OOP) : jeglicher OOP Type basiert auf einer Mapartigen struktur, also einem Child der Object Base,
//                      das bedeutet jeder Member einer User Klasse, damit also auch ein Member der map OOP Child Klasse
//                      muss die ID selbst enthalten
//
// Type Registry
// . zudem muss im TypeRegister ein Keyword verzeichnis geführt werden
// . Standard typen wie int, double, str geben per default den richtigen Child zurück also IntObject oder so
// . Dann gibt es ein Funktion registry dieses enthält neben überladenen Funktionen wie add, subtract, etc die Konstruktoren
//   als Funktionen die entsprechende Childs zurückgeben
// . Die Member Funktionen Register Strucktur enthält dann für jeden im TypeRegister registrierten Type wieder ein Funktionenregister
//   das die Funktionen der Klasse (evtl auch statisch mit Kennzeichnung) enthält
// . Dabei muss dann der Member (oder die Klasse) als ausführende Umgebung an die Funktion übergeben werden oder es gibt eine Logik
//   die im Hintergrund wirklich die entsprechende Member Funktion aufruft
//
// TypeRegister
// . beinhaltet ID counter der dynamisch IDs zuweisen kann
// . beinhaltet Speicherstruktur (Map) die ID und Infos wie Keyword, etc enthält
// . Mit Register aufruf eines Types wird ID zugewiesen, Infosatz gespeichert
//
// ObjectBase Child
// . Mit Register aufruf des Types wird ID erhalten und im Funktionsregister Konstruktoren unter dem Keyword
//   hinterlegt
// . hinterlegt Memberfunktionen und statische Funktionen im Memberfunktionen register hinterlegt
// >> Aufruf statics über Keyword des Childs, aufruf Memberfunc über <child>.<func>
//
// Aufbau von ObjectBase Child
// . liegt in variableTable mit <std::string, std::unique_ptr<ObjectBase>>
//   >> für polymorphie benötigt
// . Child enthält dann direkt den Data Member
// >> also kein Ptr/unique_ptr/share_ptr
// . durch Lagerung als unique_ptr "gehärt" dem variableTable pair der Child Member
// >> Operationen mov, copy, ref
// . copy ist deepcopy des Childs
// . ref referenziert den child
// . mov verschiebt den inhalt einer anderen Variable in sich selbst
// >> also am besten unique_ptr mit std::move bewegen und an alter stelle neu initialisieren
// Referenzierung über ptr auf den anderen child oder durch raw ptr auf verwalteten Speicherplatz
//
// >> am besten interface für objectklassen implementieren sodass childs direkt von interface<dataType> erben können
//    und getData und getRef implementieren können
//
// IObjectBase {};                              // Polymorpher Typ
// ObjectBase<dataType> : IObjectBase{};        // Interface das Aufbau implementiert wie getData(), ref Ptr, etc
// Child : ObjectBase<int>{};                   // implementiert konkreten Typ

struct TypeInfo{

    std::string keyword;
    std::function<IObject*()> initConstructor;
    TypeIndex typeIndex;
};

// Decl des TypeRegisters
struct TypeRegister{

    // Arbitary Type is 0
    TypeIndex typeCounter = 2;

    std::map<TypeIndex, TypeInfo> typeInfos = {};
    std::map<std::string, TypeIndex> typeIndices = {};

    TypeIndex registerType(const std::string& keyword, const std::function<IObject*()>& initConstructor){

        RETURNING_ASSERT(!typeIndices.contains(keyword), "Type Register hat Typ " + keyword + " bereits registriert", INVALID_TYPE_INDEX);

        auto [it, itSucces] = typeInfos.emplace(typeCounter, TypeInfo{keyword, initConstructor, typeCounter});
        RETURNING_ASSERT(it != typeInfos.end(), "Type " + keyword + " konnte nicht registriert werden", INVALID_TYPE_INDEX);

        auto [itIdx, itIdxSucces] = typeIndices.emplace(keyword, typeCounter);
        RETURNING_ASSERT(itIdx != typeIndices.end(), "Type " + keyword + " konnte nicht registriert werden", INVALID_TYPE_INDEX);

        return typeCounter++;
    }

    bool contains(const std::string& keyword){
        
        return typeIndices.contains(keyword);
    }

    IObject* constructRegisteredType(const std::string& keyword){

        RETURNING_ASSERT(typeIndices.contains(keyword),
            "Für keyword '" + keyword + "' sind keine Typinformationen hinterlegt", nullptr);

        return typeInfos[typeIndices[keyword]].initConstructor();
    }

    IObject* constructRegisteredType(TypeIndex typeIndex){

        RETURNING_ASSERT(typeInfos.contains(typeIndex),
            "Für index '" + std::to_string(typeIndex) + "' sind keine Typinformationen hinterlegt", nullptr);

        return typeInfos[typeIndex].initConstructor();
    }

    friend std::ostream& operator<<(std::ostream& os, const TypeRegister& reg){

        LOG << "Register mit " << reg.typeInfos.size() << " registrierten Typen" << endl;

        for(const auto& [k, f] : reg.typeInfos){
            
            os << "type " << k << "; keyword '" << f.keyword << "'" << endl;
        }
        return os;
    }
};