#include "ARGS.h"

namespace types{

    std::pair<bool, Variable*> ARGS::containsDataReference(IObject* dataPtr) {

        //
        for(auto& var : getMember()){

            if(var.getData() == dataPtr){
                
                return std::make_pair(true, &var.getVariableRef());
            }

            auto pair = var.getData()->containsDataVariableOrReference(dataPtr);
            if(pair.first){

                return pair;
            }
        }

        return std::make_pair(false, nullptr);
    }

    std::pair<bool, Variable*> ARGS::containsDataVariableOrReference(IObject* dataPtr){

        return std::make_pair(false, nullptr);
    }

    bool ARGS::setUpClass(){

        // register in TypeRegister
        if(!init("args", [](){ return new ARGS(); })){ return false; }

        //
        IObject::ARGS_TYPE = ARGS::typeIndex;

        // Registrierung von Keyword zur rvalue Konstruktion von bools
        registerKeyword("nullArgs", new ARGS(EvalResultPtrVec()));

        //
        registerFunction("args", {IObject::ARGS_TYPE},
            [__functionLabel__ = "args", __numArgs__ = 0](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                PREPARE_RETURNS;

                GET_RETURN(ARGS, 0);
                ret0->moveFrom(inputs);
        },
        {IObject::ARGS_TYPE});

        return true;
    }

    bool ARGS::emplaceOperations(){

        // Member
        registerMemberFunction(ARGS::typeIndex, "size", {},
            [__functionLabel__ = "size", __numArgs__ = 0](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // // Returns | Inputs
                GET_RETURN(INT, 0);
                GET_MEMBER(ARGS);

                // schreiben in returns
                ret0->getMember() = mb->getMember().size();
        },
        {INT::typeIndex});

        // Member
        registerMemberFunction(ARGS::typeIndex, "get", {INT::typeIndex},
            [__functionLabel__ = "get", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

                // Returns | Inputs
                GET_ARG(INT, 0);
                GET_MEMBER(ARGS);

                //
                RETURNING_ASSERT(arg0->getMember() < mb->getMember().size(), "...",);

                // schreiben in returns
                returns.emplace_back().reference(mb->getMember()[arg0->getMember()]);
        },
        {IObject::ARBITATRY_TYPE});

        // Member
        registerMemberFunction(ARGS::typeIndex, "chainByFunction", {STRING::typeIndex},
            [__functionLabel__ = "chainByFunction", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

                // Returns | Inputs
                GET_ARG(STRING, 0);
                GET_MEMBER(ARGS);

                //
                auto& argVec = mb->getMember();

                //
                RETURNING_ASSERT(!argVec.empty(), "",);

                // schreiben in returns
                returns.emplace_back();
                returns.back().getVariableRef().clone(argVec[0].getVariableRef());

                //
                if(argVec.size() < 2){ return; }

                //
                EvalResultVec tmpRes;

                //
                for(size_t i = 1; i < argVec.size(); i++){

                    //
                    callFunction(arg0->getMember(), tmpRes, {&returns.back(), &argVec[i]}, returnToScope);
                }
        },
        {IObject::ARBITATRY_TYPE});

        // Member
        registerMemberFunction(ARGS::typeIndex, "chainByOperator", {STRING::typeIndex},
            [__functionLabel__ = "chainByOperator", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

                // Returns | Inputs
                GET_ARG(STRING, 0);
                GET_MEMBER(ARGS);

                //
                auto& argVec = mb->getMember();

                //
                RETURNING_ASSERT(!argVec.empty(), "",);

                // schreiben in returns
                returns.emplace_back();
                returns.back().getVariableRef().clone(argVec[0].getVariableRef());

                //
                if(argVec.size() < 2){ return; }

                //
                RETURNING_ASSERT(g_TwoArgOperations.contains(arg0->getMember()), "g_TwoArgOperations enthält Operator " + arg0->getMember() + " nicht",);
                const std::string funcName = g_TwoArgOperations[arg0->getMember()];

                //
                EvalResultVec tmpRes;

                //
                for(size_t i = 1; i < argVec.size(); i++){

                    //
                    callFunction(funcName, tmpRes, {&returns.back(), &argVec[i]}, returnToScope);
                }
        },
        {IObject::ARBITATRY_TYPE});

        // Member
        registerFunction("combineByFunction", {ARGS::typeIndex, ARGS::typeIndex, STRING::typeIndex},
            [__functionLabel__ = "combineByFunction", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                // ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

                // Returns | Inputs
                GET_ARG(ARGS, 0); GET_ARG(ARGS, 1); GET_ARG(STRING, 2);

                auto& argVec0 = arg0->getMember();
                auto& argVec1 = arg1->getMember();

                //
                RETURNING_ASSERT(!argVec0.empty() && !argVec1.empty(), "",);

                // //
                // RETURNING_ASSERT(g_TwoArgOperations.contains(arg2->getMember()), "g_TwoArgOperations enthält Operator " + arg0->getMember() + " nicht",);

                //
                EvalResultVec tmpRes;

                //
                returns.reserve(argVec0.size() * argVec1.size());

                //
                for(size_t i = 0; i < argVec0.size(); i++){
                    
                    for(size_t j = 0; j < argVec1.size(); j++){

                        returns.emplace_back().getVariableRef().clone(argVec0[i].getVariableRef());
                        callFunction(arg2->getMember(), tmpRes, {&returns.back(), &argVec1[j]}, returnToScope);
                    }
                }
        },
        {IObject::ARGS_TYPE});

        // Member
        registerFunction("combineByOperator", {ARGS::typeIndex, ARGS::typeIndex, STRING::typeIndex},
            [__functionLabel__ = "combineByOperator", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                // ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

                // Returns | Inputs
                GET_ARG(ARGS, 0); GET_ARG(ARGS, 1); GET_ARG(STRING, 2);

                auto& argVec0 = arg0->getMember();
                auto& argVec1 = arg1->getMember();

                //
                RETURNING_ASSERT(!argVec0.empty() && !argVec1.empty(), "",);

                //
                RETURNING_ASSERT(g_TwoArgOperations.contains(arg2->getMember()), "g_TwoArgOperations enthält Operator " + arg2->getMember() + " nicht",);
                const std::string funcName = g_TwoArgOperations[arg2->getMember()];

                //
                EvalResultVec tmpRes;

                //
                returns.reserve(argVec0.size() * argVec1.size());

                //
                for(size_t i = 0; i < argVec0.size(); i++){
                    
                    for(size_t j = 0; j < argVec1.size(); j++){

                        returns.emplace_back().getVariableRef().clone(argVec0[i].getVariableRef());
                        callFunction(funcName, tmpRes, {&returns.back(), &argVec1[j]}, returnToScope);
                    }
                }
        },
        {IObject::ARGS_TYPE});

        // Member
        registerFunction("combineInplaceByFunction", {ARGS::typeIndex, ARGS::typeIndex, STRING::typeIndex, STRING::typeIndex},
            [__functionLabel__ = "combineByFunction", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                // ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

                // Returns | Inputs
                GET_ARG(ARGS, 0); GET_ARG(ARGS, 1); GET_ARG(STRING, 2); GET_ARG(STRING, 3);

                auto& argVec0 = arg0->getMember();
                auto& argVec1 = arg1->getMember();
                const std::string& mode = arg3->getMember();

                //
                RETURNING_ASSERT(!argVec0.empty() && !argVec1.empty(), "",);

                //
                RETURNING_ASSERT(mode == "rowmajor" || mode == "colmajor", 
                                "Mode muss 'rowmajor' oder 'colmajor' sein, nicht: " + mode,);

                //
                EvalResultVec tmpRes;

                //
                if(mode == "rowmajor"){
                    // i außen, j innen
                    for(size_t i = 0; i < argVec0.size(); i++){
                        for(size_t j = 0; j < argVec1.size(); j++){
                            callFunction(arg2->getMember(), tmpRes, {&argVec0[i], &argVec1[j]}, returnToScope);
                        }
                    }
                } else { // colmajor
                    // j außen, i innen
                    for(size_t j = 0; j < argVec1.size(); j++){
                        for(size_t i = 0; i < argVec0.size(); i++){
                            callFunction(arg2->getMember(), tmpRes, {&argVec0[i], &argVec1[j]}, returnToScope);
                        }
                    }
                }
        },
        {IObject::ARGS_TYPE});

        // Member
        registerFunction("combineInplaceByOperator", {ARGS::typeIndex, ARGS::typeIndex, STRING::typeIndex, STRING::typeIndex},
            [__functionLabel__ = "combineByOperator", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                // ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

                // Returns | Inputs
                GET_ARG(ARGS, 0); GET_ARG(ARGS, 1); GET_ARG(STRING, 2); GET_ARG(STRING, 3);

                auto& argVec0 = arg0->getMember();
                auto& argVec1 = arg1->getMember();
                const std::string& mode = arg3->getMember();

                //
                RETURNING_ASSERT(!argVec0.empty() && !argVec1.empty(), "",);

                //
                RETURNING_ASSERT(g_TwoArgOperations.contains(arg2->getMember()), 
                                "g_TwoArgOperations enthält Operator " + arg2->getMember() + " nicht",);
                
                //
                RETURNING_ASSERT(mode == "rowmajor" || mode == "colmajor", 
                                "Mode muss 'rowmajor' oder 'colmajor' sein, nicht: " + mode,);

                const std::string funcName = g_TwoArgOperations[arg2->getMember()];

                //
                EvalResultVec tmpRes;

                //
                if(mode == "rowmajor"){

                    // i außen, j innen
                    for(size_t i = 0; i < argVec0.size(); i++){
                        for(size_t j = 0; j < argVec1.size(); j++){
                            callFunction(funcName, tmpRes, {&argVec0[i], &argVec1[j]}, returnToScope);
                        }
                    }
                } else { // colmajor
                    
                    // j außen, i innen
                    for(size_t j = 0; j < argVec1.size(); j++){
                        for(size_t i = 0; i < argVec0.size(); i++){
                            callFunction(funcName, tmpRes, {&argVec0[i], &argVec1[j]}, returnToScope);
                        }
                    }
                }
        },
        {IObject::ARGS_TYPE});

        // Member
        registerMemberFunction(ARGS::typeIndex, "emplace", {IObject::ARGS_TYPE},
            [__functionLabel__ = "emplace"](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;

                // Returns | Inputs
                GET_MEMBER(ARGS);
                mb->emplace(inputs);
        },
        {});

        return true;
    }
}