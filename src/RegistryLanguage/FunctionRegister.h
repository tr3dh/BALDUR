#pragma once

#include "IObject.h"

class FunctionRegister {

public:

    // Nutzung : <functionlabel, {paramIndices}>
    //    z.B  : <"add" , {0,0}> | 0 TypeIndex IntegerObject 
    typedef std::pair<std::string, std::vector<TypeIndex>> FunctionRegisterKey;

    // Parameter : void({outputs}, {inputs}, MemberRef)
    typedef std::function<void(std::vector<IObject*>&, const std::vector<IObject*>&, IObject*)> IObjectFunction;

    // Kombination in Map
    typedef std::map<FunctionRegisterKey, IObjectFunction> FunctionMap;

    FunctionMap functions;

    // Registrierung mit expliziten Typen
    void registerFunction(const std::string& functionLabel, const std::vector<TypeIndex>& functionArgsTypes, const IObjectFunction& func) {

        FunctionRegisterKey key = {functionLabel, functionArgsTypes};
        functions[key] = func;
        
        LOG << "Register unter \"" << functionLabel << "\" mit " << functionArgsTypes.size() << " Parameter Funktion befüllt" << endl;
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
};