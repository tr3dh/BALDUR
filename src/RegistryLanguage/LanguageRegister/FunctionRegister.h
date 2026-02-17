#pragma once

#include "../Objects/IObject.h"
#include "../Evaluation/EvalResult.h"

//
typedef std::vector<EvalResult> EvalResultVec;
typedef std::vector<EvalResult*> EvalResultPtrVec;

//
typedef EvalResultVec& FunctionReturns;
typedef EvalResultPtrVec FunctionParams;
typedef EvalResult* TypeMember;

// Nutzung : <functionlabel, {paramIndices}>
//    z.B  : <"add" , {0,0}> | 0 TypeIndex IntegerObject 
typedef std::pair<std::string, std::vector<TypeIndex>> FunctionRegisterKey;

struct Scope;

#define FREG_DEFAULT_CALLARGS \
    FunctionReturns returns, const FunctionParams& functionParams, Scope& returnToScope

#define FREG_CALLARGS \
    FunctionReturns returns, const FunctionParams& functionParams, Scope& returnToScope

#define FREG_DEFAULT_CALLARGS_WITH_MEMBER \
    FREG_DEFAULT_CALLARGS, TypeMember member = nullptr

#define FREG_CALLARGS_WITH_MEMBER \
    FREG_CALLARGS, TypeMember member

#define FREG_ARGS \
    FunctionReturns returns, const FunctionParams& inputs, Scope& returnToScope, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member

//
#define FREG_ARG_TYPES \
    FunctionReturns, const FunctionParams&, Scope&, const std::vector<TypeIndex>&, TypeMember

// Parameter : void({outputs}, {inputs}, MemberRef)
typedef std::function<void(FREG_ARG_TYPES)> IObjectFunction;

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

        // auto [it, inserted] = functions.try_emplace(key, std::make_pair(func,functionReturnTypes));
        // RETURNING_ASSERT(inserted, "Funktion " + functionLabel + " wurde bereits definiert, neue Definition wird ignoriert",);
    }

    std::vector<TypeIndex> getArgTypes(const FunctionParams& params) {

        std::vector<TypeIndex> types = {};

        for(auto& ptr : params){

            RETURNING_ASSERT(ptr->getVariableRef().isValid(), "param ptr is not valid",{});
            types.emplace_back(ptr->getVariableRef().getData()->getTypeIndex());
        }

        return types;
    }

    void callFunction(const std::string& functionLabel, FREG_DEFAULT_CALLARGS_WITH_MEMBER) {

        auto fIt = functions.find({functionLabel, getArgTypes(functionParams)});

        if(fIt != functions.end()){

            fIt->second.first(returns, functionParams, returnToScope, fIt->second.second, member);
            return;
        }
        
        auto nullfIt = functions.find({functionLabel, std::vector<TypeIndex>(functionParams.size(), IObject::ARBITATRY_TYPE)});

        if(nullfIt != functions.end()){

            nullfIt->second.first(returns, functionParams, returnToScope, nullfIt->second.second, member);
            return;
        }

        auto argfIt = functions.find({functionLabel, {IObject::ARGS_TYPE}});

        if(argfIt != functions.end()){

            argfIt->second.first(returns, functionParams, returnToScope, argfIt->second.second, member);
            return;
        }

        RETURNING_ASSERT(TRIGGER_ASSERT, "Function für angegebenes Label " + functionLabel + " mit " + std::to_string(functionParams.size()) + " args konnte nicht gefunden werden",);
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
    PERMISSIVLY_RESIZE_RETURNS(functionReturnTypes.size()); \
    \
    for(size_t retIdx = 0; retIdx < functionReturnTypes.size(); retIdx++){ \
        \
        returns[retIdx].constructRValueByObject(constructRegisteredType(functionReturnTypes[retIdx])); \
    }

#define APPEND_RETURNS \
    \
    returns.resize(returns.size() + functionReturnTypes.size()); \
    \
    for(size_t retIdx = 0; retIdx < functionReturnTypes.size(); retIdx++){ \
        \
        returns[returns.size() - functionReturnTypes.size() + retIdx].constructRValueByObject(constructRegisteredType(functionReturnTypes[retIdx])); \
    }

#define CLEAR_RETURNS returns.clear();

// #define GET_RETURN(CastType, Position) \
//     CastType* ret##Position = nullptr; \
//     if(returns.size() == functionReturnTypes.size()){ \
//         CastType* ret##Position = static_cast<CastType*>(returns[Position].getVariableRef().getData()); \
//     } \
//     else { \
//         CastType* ret##Position = static_cast<CastType*>(returns[returns.size() - functionReturnTypes.size() + Position].getVariableRef().getData()); \
//     }

#define GET_RETURN(CastType, Position) \
    CastType* ret##Position =  static_cast<CastType*>(returns[Position].getVariableRef().getData());

#define GET_LBRETURN(CastType, PseudoPos, Position) \
    CastType* ret##PseudoPos =  static_cast<CastType*>(returns[Position].getVariableRef().getData());

#define GET_ARG(CastType, Position) \
    CastType* arg##Position = static_cast<CastType*>(inputs[Position]->getVariableRef().getData()); \

#define GET_MEMBER(CastType) \
    CastType* mb = static_cast<CastType*>(member->getVariableRef().getData());

// speichert sich Member des alten Inputs
// rekonstruiert Input mit erforderlichem Typ und setzt Member über cast des gespeicherten vormaligen Members

// für IObject, das Member und nicht Memberptr enthält
//
// #define RECAST_INPUT_INPLACE(CastToType, CastFromType, Position) \
//     decltype(CastFromType::member) formerMb##Position = static_cast<CastFromType*>(inputs[Position]->getData())->getMember(); \
//     inputs[Position]->getVariableRef().constructByObject(constructRegisteredType(CastToType::typeIndex)); \
//     static_cast<CastToType*>(inputs[Position]->getData())->getMember() = static_cast<decltype(CastToType::member)>(formerMb##Position);

#define RECAST_INPUT_INPLACE(CastToType, CastFromType, Position) \
    auto formerMb##Position = static_cast<CastFromType*>(inputs[Position]->getData())->getMember(); \
    inputs[Position]->getVariableRef().constructByObject(constructRegisteredType(CastToType::typeIndex)); \
    static_cast<CastToType*>(inputs[Position]->getData())->getMember() = formerMb##Position; \

#define END_OF_FUNCTION_REG_FILE