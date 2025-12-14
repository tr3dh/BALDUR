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
                returns.emplace_back();
                returns.back();

                returns.back().reference(mb->getMember()[arg0->getMember()]);
        },
        {IObject::ARBITATRY_TYPE});

        return true;
    }
}