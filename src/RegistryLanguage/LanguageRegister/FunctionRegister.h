#pragma once

#include "../Objects/IObject.h"

// Nutzung : <functionlabel, {paramIndices}>
//    z.B  : <"add" , {0,0}> | 0 TypeIndex IntegerObject 
typedef std::pair<std::string, std::vector<TypeIndex>> FunctionRegisterKey;

// Parameter : void({outputs}, {inputs}, MemberRef)
typedef std::function<void(std::vector<IObject*>&, const std::vector<IObject*>&, IObject*)> IObjectFunction;

// Kombination in Map
typedef std::map<FunctionRegisterKey, IObjectFunction> FunctionMap;

//
class FunctionRegister {

public:

    FunctionMap functions;

    // Registrierung mit expliziten Typen
    void registerFunction(const std::string& functionLabel, const std::vector<TypeIndex>& functionArgsTypes, const IObjectFunction& func) {

        FunctionRegisterKey key = {functionLabel, functionArgsTypes};
        functions[key] = func;
    }

    std::vector<TypeIndex> getArgTypes(const std::vector<IObject*>& params) {

        std::vector<TypeIndex> types = {};

        for(const auto& ptr : params){
            types.emplace_back(ptr->getTypeIndex());
        }

        return types;
    }

    void callFunction(const std::string& functionLabel, std::vector<IObject*>& returns, const std::vector<IObject*>& functionParams, IObject* member = nullptr) {

        auto fIt = functions.find({functionLabel, getArgTypes(functionParams)});

        RETURNING_ASSERT(fIt != functions.end(), "Function für angegebenes Label " + functionLabel + " konnte nicht gefunden werden",);

        if(fIt != functions.end()){
            fIt->second(returns, functionParams, member);
        }
        else{
            _ERROR << "Function " << functionLabel << " für angegebene Typen nicht gefunden" << endl;
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const FunctionRegister& reg){

        LOG << "Register mit " << reg.functions.size() << " registrierten Funktionen" << endl;

        for(const auto& [k, f] : reg.functions){
            
            os << "function '" << k.first << "'; params {";

            for(const auto& p : k.second){
                os << p << ", ";
            }

            os << "};" << endl;
        }
        return os;
    }
};

// Verwendung des Registers
// ImGerister können Funktionen unter einem Schlüssel bestehend aus einem Keyword und einer Idx Liste die Funktion hinterlegt werden
// 
// g_FunctionRegister.registerFunction("add", {Str().getTypeIndex(), Str().getTypeIndex()},
//     [__functionLabel__ = "add", __numArgs__ = 2](std::vector<IObject*>& returns, const std::vector<IObject*>& inputs, IObject* member){

//         // Asserts
//         ASSERT_IS_NO_MEMBER_FUNCTION;
//         ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

//         // Casts
//         Str* str1 = static_cast<Str*>(inputs[0]);
//         Str* str2 = static_cast<Str*>(inputs[1]);

//         //
//         str1->ping();
// });

// Die folgenden Makros übernehmen dabei mehrere Asserts
#define L_ASSERT_IS_NO_MEMBER_FUNCTION(FunctionLabel)\
    RETURNING_ASSERT(member == nullptr, "Die Nicht-Member-Funktion " + std::string(FunctionLabel) + " bekommt einen validen Member übergeben",);

#define ASSERT_IS_NO_MEMBER_FUNCTION\
    L_ASSERT_IS_NO_MEMBER_FUNCTION(__functionLabel__)

#define L_ASSERT_IS_MEMBER_FUNCTION(FunctionLabel)\
    RETURNING_ASSERT(member != nullptr, "Die Member-Funktion " + std::string(FunctionLabel) + " bekommt keinen validen Member übergeben",);

#define ASSERT_IS_MEMBER_FUNCTION\
    L_ASSERT_IS_MEMBER_FUNCTION(__functionLabel__)

#define L_ASSERT_HAS_N_INPUT_ARGS(FunctionLabel, numInputArgs)\
    RETURNING_ASSERT(inputs.size() == numInputArgs, std::string(FunctionLabel) + " Funktion bekommt != " + std::to_string(numInputArgs) + " input Parameter übergeben",);

#define ASSERT_HAS_N_INPUT_ARGS(numInputArgs)\
    L_ASSERT_HAS_N_INPUT_ARGS(__functionLabel__, numInputArgs)

#define L_ASSERT_HAS_LESS_THAN_N_INPUT_ARGS(FunctionLabel, numInputArgs)\
    RETURNING_ASSERT(inputs.size() < numInputArgs, std::string(FunctionLabel) + " Funktion bekommt >= " + std::to_string(numInputArgs) + " input Parameter übergeben",);

#define ASSERT_HAS_LESS_THAN_N_INPUT_ARGS(numInputArgs)\
    L_ASSERT_HAS_LESS_THAN_N_INPUT_ARGS(__functionLabel__)

#define L_ASSERT_HAS_MORE_THAN_N_INPUT_ARGS(FunctionLabel, numInputArgs)\
    RETURNING_ASSERT(inputs.size() > numInputArgs, std::string(FunctionLabel) + " Funktion bekommt >= " + std::to_string(numInputArgs) + " input Parameter übergeben",);

#define ASSERT_HAS_MORE_THAN_N_INPUT_ARGS(numInputArgs)\
    L_ASSERT_HAS_MORE_THAN_N_INPUT_ARGS(__functionLabel__)

#define AGRESSIVLY_RESIZE_RETURNS(numInputArgs)\
    returns.resize(numInputArgs);

#define PERMISSIVLY_RESIZE_RETURNS(numInputArgs)\
    if(returns.size() != numInputArgs){\
        AGRESSIVLY_RESIZE_RETURNS(numInputArgs)\
    }
    
#define CLEAR_RETURNS returns.clear();