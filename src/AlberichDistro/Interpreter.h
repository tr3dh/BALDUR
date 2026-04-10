#pragma once

#include "TypeInstances/gatheredInstances.h"
#include "nlohmann/json.hpp"

std::string getExecutablePath();
std::string getExecutableDir();

extern std::string g_lspEncoderKey;

struct LSPData{

    // LSP Data enthält
    // . Typenindizes und keywords
    // . Funktionen (eigenständige, Attributs- und statische)
    // . Variablen (aus dekonstruierten Scopes)

    std::vector<std::string> keywords = {};
    std::map<std::pair<std::string, TypeIndex>, std::pair<std::string, std::string>> constKeywords = {};
    std::vector<std::string> operators = {};
    std::map<std::string, TypeIndex> types = {};
    std::map<FunctionRegisterKey, std::pair<std::string, std::string>> functions = {};
    std::map<std::pair<std::string, TypeIndex>, std::pair<std::string, std::string>> variables = {};

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
        // addFReg(g_FunctionRegister);
        for(const auto& [key, val] : g_FunctionRegister.functions){

            std::string shortDetail = "[" + fprintPlainVector(key.second, [](const TypeIndex& elem){ if(elem > 0){ return getKeywordByTypeIndex(elem); } else{ return std::string("arbitary"); } }, false) + "]";
            
            std::string detail = key.first;
            detail += fprintPlainVector(key.second, [](const TypeIndex& elem){ if(elem > 0){ return getKeywordByTypeIndex(elem); } else{ return std::string("arbitary"); } });
            
            functions.try_emplace(key, std::make_pair(shortDetail, detail));
        }

        for(const auto& [idx, freg] : g_MemberFunctionRegisters){
            
            for(const auto& [key, val] : freg.functions){

                std::string shortDetail = getKeywordByTypeIndex(idx) + " -> [" + fprintPlainVector(key.second, [](const TypeIndex& elem){ if(elem > 0){ return getKeywordByTypeIndex(elem); } else{ return std::string("arbitary"); } }, false) + "]";

                std::string detail = getKeywordByTypeIndex(idx) + " -> " + key.first;
                detail += fprintPlainVector(key.second, [](const TypeIndex& elem){ if(elem > 0){ return getKeywordByTypeIndex(elem); } else{ return std::string("arbitary"); } });
                
                functions.try_emplace(key, std::make_pair(shortDetail, detail));
            }
        }

        for(const auto& [idx, freg] : g_StaticFunctionRegisters){
            
            for(const auto& [key, val] : freg.functions){

                std::string shortDetail = getKeywordByTypeIndex(idx) + " >> [" + fprintPlainVector(key.second, [](const TypeIndex& elem){ if(elem > 0){ return getKeywordByTypeIndex(elem); } else{ return std::string("arbitary"); } }, false) + "]";

                std::string detail = getKeywordByTypeIndex(idx) + " >> " + key.first;
                detail += fprintPlainVector(key.second, [](const TypeIndex& elem){ if(elem > 0){ return getKeywordByTypeIndex(elem); } else{ return std::string("arbitary"); } });
                
                functions.try_emplace(key, std::make_pair(shortDetail, detail));
            }
        }
    }

    void addConstKeywords(){

        for(const auto& [label, object] : g_KeywordRegister.keywordObjects){
            
            
            std::string shortDetail = "const " + object->getTypeKeyword();
            std::string detail = "const " + object->getTypeKeyword() + " " + label;
        
            constKeywords.try_emplace(std::make_pair(label, object->getTypeIndex()), std::make_pair(shortDetail, detail));
        }
    }

    void addAll(){

        operators = g_UsedOperators;

        addTypes();
        addAllFRegs();
        addConstKeywords();
    }

    void addScope(Scope* scope){

        for(auto& [label, var] : scope->variableTable){

            std::string shortDetail = var.getData()->getTypeKeyword();

            std::string detail = var.getData()->isUniform() ? "uniform " : "";
            detail += var.getData()->getTypeKeyword() + " ";
            detail += var.isReference() ?  "ref " : "instc ";
            detail += label;

            variables.try_emplace(std::make_pair(label, var.getData()->getTypeIndex()), std::make_pair(shortDetail, detail));
        }
    }

    bool isEmpty(){

        return keywords.empty() && constKeywords.empty() && operators.empty() &&
                types.empty() && functions.empty() && variables.empty();
    }
};

LSPData getLSPData(const std::string& path);
void saveLSPData(const LSPData& data, const std::string& path);

struct LspState {

    std::unordered_set<std::string> keywords;
    std::unordered_set<std::string> typeKeywords;
    std::unordered_set<std::string> variables;
    std::unordered_set<std::string> functions;

    std::unordered_map<std::string, std::string> documents;

    void applyLSPData(const LSPData& data){

        //
        typeKeywords.clear();
        for(const auto& [name, typeIndex] : data.types)
            typeKeywords.insert(name);

        //
        functions.clear();
        for(const auto& [key, info] : data.functions)
            functions.insert(key.first);

        //
        variables.clear();
        for(const auto& [key, info] : data.variables)
            variables.insert(key.first);
    }
};

// Serealisierung der ByteSequence für beliebige std::pair
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