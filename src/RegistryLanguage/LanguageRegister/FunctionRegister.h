#pragma once

#include "../Objects/IObject.h"
#include "../Evaluation/EvalResult.h"

// Nutzung : <functionlabel, {paramIndices}>
//    z.B  : <"add" , {0,0}> | 0 TypeIndex IntegerObject 
typedef std::pair<std::string, std::vector<TypeIndex>> FunctionRegisterKey;

// Parameter : void({outputs}, {inputs}, MemberRef)
typedef std::function<void(std::vector<std::shared_ptr<EvalResult>>&, const std::vector<std::shared_ptr<EvalResult>>&, const std::vector<TypeIndex>&, std::shared_ptr<EvalResult>)> IObjectFunction;

//
typedef std::pair<IObjectFunction, std::vector<TypeIndex>> FunctionRegisterValue;

//
struct FunctionRegisterKeyComparator {

    bool operator()(const FunctionRegisterKey& lhs, const FunctionRegisterKey& rhs) const {

        // Sortierung nach Funktionslabel
        if (lhs.first < rhs.first) return true;
        if (lhs.first > rhs.first) return false;

        // Sortierung nach Argument Idx Vektor
        const auto& lvec = lhs.second;
        const auto& rvec = rhs.second;

        size_t n = std::min(lvec.size(), rvec.size());
        for (size_t i = 0; i < n; ++i) {

            //
            if (lvec[i] == rvec[i]) continue;

            // Sortierung der Null Fälle nach hinten damit Verhalten für Arbitary Types überschrieben werden können
            // und die Funktion beim Loop durchs Register zuerst gefunden wird
            if (lvec[i] == 0 && rvec[i] != 0) return false;
            if (rvec[i] == 0 && lvec[i] != 0) return true;

            //
            return lvec[i] < rvec[i];
        }

        // kürzerer Vektor ist kleiner (wie Standard)
        return lvec.size() < rvec.size();
    }
};

// Kombination in Map
// Einkommentieren für Sortierung der Arbitaryalternativen nach hinten
// >> Hilfreich für Funktionsüberladung und allgemeingültige Funktionen
typedef std::map<FunctionRegisterKey, FunctionRegisterValue/*, FunctionRegisterKeyComparator*/> FunctionMap;

//
class FunctionRegister {

public:

    FunctionMap functions;

    // Registrierung mit expliziten Typen
    void registerFunction(const std::string& functionLabel, const std::vector<TypeIndex>& functionArgsTypes,
        const IObjectFunction& func, const std::vector<TypeIndex>& functionReturnTypes) {

        FunctionRegisterKey key = {functionLabel, functionArgsTypes};
        functions[key] = {func,functionReturnTypes};
    }

    std::vector<TypeIndex> getArgTypes(const std::vector<std::shared_ptr<EvalResult>>& params) {

        std::vector<TypeIndex> types = {};

        for(const auto& ptr : params){
            types.emplace_back(ptr->getVariableRef().getData()->getTypeIndex());
        }

        return types;
    }

    void callFunction(const std::string& functionLabel, std::vector<std::shared_ptr<EvalResult>>& returns, const std::vector<std::shared_ptr<EvalResult>>& functionParams, std::shared_ptr<EvalResult> member = nullptr) {

        auto fIt = functions.find({functionLabel, getArgTypes(functionParams)});

        if(fIt != functions.end()){

            fIt->second.first(returns, functionParams, fIt->second.second, member);
            return;
        }
        
        auto nullfIt = functions.find({functionLabel, std::vector<TypeIndex>(functionParams.size(), IObject::ARBITATRY_TYPE)});

        if(nullfIt != functions.end()){

            nullfIt->second.first(returns, functionParams, nullfIt->second.second, member);
            return;
        }

        RETURNING_ASSERT(TRIGGER_ASSERT, "Function für angegebenes Label " + functionLabel + " konnte nicht gefunden werden",);
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
    
#define PREPARE_RETURNS \
    \
    ASSERT(returns.empty(), "return Vaktor enthält bereits Elemente"); \
    PERMISSIVLY_RESIZE_RETURNS(1); \
    \
    for(size_t retIdx = 0; retIdx < functionReturnTypes.size(); retIdx++){ \
        \
        if(returns[retIdx] == nullptr){ \
            \
            returns[retIdx] = std::make_shared<EvalResult>(); \
            returns[retIdx]->constructRValueByObject(constructRegisteredType(functionReturnTypes[retIdx])); \
        } \
    }

#define CLEAR_RETURNS returns.clear();

#define GET_RETURN(CastType, Position) \
    CastType* ret##Position = static_cast<CastType*>(returns[Position]->getVariableRef().getData())

#define GET_ARG(Casttype, Position) \
    Casttype* arg##Position = static_cast<Casttype*>(inputs[Position]->getVariableRef().getData())

#define END_OF_FUNCTION_REG_FILE