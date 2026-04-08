#pragma once

#include "TypeInstances/gatheredInstances.h"
#include "nlohmann/json.hpp"

struct LSPData;

extern LSPData g_LSPData;
extern std::string g_lspEncoderKey;

struct LSPData{

    // LSP Data enthält
    // . Typenindizes und keywords
    // . Funktionen (eigenständige, Attributs- und statische)
    // . Variablen (aus dekonstruierten Scopes)

    std::map<std::string, TypeIndex> types = {};
    std::map<FunctionRegisterKey, std::string> functions = {};
    std::map<std::pair<std::string, TypeIndex>, std::string> variables = {};

    void addTypes(){

        types = g_TypeRegister.typeIndices;
    }

    void addFReg(const FunctionRegister& freg){

        for(const auto& [key, val] : freg.functions){

            functions.try_emplace(key);
        }
    }

    void addAllFRegs(){
        
        // Alle Funktionen eintragen
        addFReg(g_FunctionRegister);

        for(const auto& [key, freg] : g_MemberFunctionRegisters){
            
            addFReg(freg);
        }

        for(const auto& [key, freg] : g_StaticFunctionRegisters){
            
            addFReg(freg);
        }
    }

    void addAll(){

        addTypes();
        addAllFRegs();
    }

    void addScope(Scope* scope){

        for(const auto& [label, var] : scope->variableTable){

            variables.try_emplace(std::make_pair(label, var.getData()->getTypeIndex()));
        }
    }
};

template<typename first, typename second>
inline void toByteSequence(const std::pair<first, second>& member, ByteSequence& seq) {

    seq.insertMultiple(member.first, member.second);
}

template<typename first, typename second>
inline void fromByteSequence(std::pair<first, second>& member, ByteSequence& seq) {

    seq.extractMultipleReversed(member.first, member.second);
}

// //
// template<typename first, typename second>
// inline std::ostream& operator<<(std::ostream& os, const std::pair<first, second>& pair) {

//     os << "Pair [ " << pair.first << " ; " << pair.second << " ]" << endln; 
//     return os;
// }

void defaultSetupLexicalInstances();

std::vector<std::unique_ptr<IObject>> executeDistroProgram(const std::string& scriptPath);