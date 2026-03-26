#include "BOOL.h"

namespace types{

    bool BOOL::setUpClass(){

        // register in TypeRegister
        if(!init("bool", [](){ return new BOOL(); })){ return false; }

        // Registrierung von Keyword zur rvalue Konstruktion von bools
        registerKeyword("true", new BOOL(true));
        registerKeyword("false", new BOOL(false));

        //
        registerStaticFunction(BOOL::typeIndex, "crInstance", {},
            [__functionLabel__ = "crInstance", __numArgs__ = 0](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_RETURN(BOOL, 0);

                // schreiben in returns
                ret0->getMember() = false;
        },
        {BOOL::typeIndex});

        // Konstruktoren
        registerFunction("bool", {},
            [__functionLabel__ = "bool", __numArgs__ = 0](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_RETURN(BOOL, 0);

                // schreiben in returns
                ret0->getMember() = false;
        },
        {BOOL::typeIndex});

        // Konstruktoren
        registerFunction("bool", {BOOL::typeIndex},
            [__functionLabel__ = "bool", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_RETURN(BOOL, 0);
                GET_ARG(BOOL, 0);

                // schreiben in returns
                ret0->getMember() = arg0->getMember();
        },
        {BOOL::typeIndex});

        // Operatoren
        registerFunction("__equal__", {BOOL::typeIndex, BOOL::typeIndex},
            [__functionLabel__ = "__equal__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
               
                //
                returns.emplace_back();
                returns[returns.size() - 1].constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                BOOL* ret0 = static_cast<BOOL*>(returns[returns.size()-1].getVariableRef().getData());
                GET_ARG(BOOL, 0); GET_ARG(BOOL, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() == arg1->getMember();
        },
        {BOOL::typeIndex});

        // Operatoren
        registerFunction("__notEqual__", {BOOL::typeIndex, BOOL::typeIndex},
            [__functionLabel__ = "__notEqual__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
               
                //
                returns.emplace_back();
                returns[returns.size() - 1].constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                BOOL* ret0 = static_cast<BOOL*>(returns[returns.size()-1].getVariableRef().getData());

                GET_ARG(BOOL, 0); GET_ARG(BOOL, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() != arg1->getMember();
        },
        {BOOL::typeIndex});

        // Operatoren
        registerFunction("__andAssign__", {BOOL::typeIndex, BOOL::typeIndex},
            [__functionLabel__ = "__andAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_ARG(BOOL, 0); GET_ARG(BOOL, 1);

                // schreiben in returns
                arg0->getMember() &= arg1->getMember();
        },
        {});

        //
        registerFunction("__orAssign__", {BOOL::typeIndex, BOOL::typeIndex},
            [__functionLabel__ = "__orAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_ARG(BOOL, 0); GET_ARG(BOOL, 1);

                // schreiben in returns
                arg0->getMember() |= arg1->getMember();
        },
        {});

        //
        registerFunction("__xorAssign__", {BOOL::typeIndex, BOOL::typeIndex},
            [__functionLabel__ = "__xorAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_ARG(BOOL, 0); GET_ARG(BOOL, 1);

                // schreiben in returns
                arg0->getMember() ^= arg1->getMember();
        },
        {});

        //
        registerFunction("__nandAssign__", {BOOL::typeIndex, BOOL::typeIndex},
            [__functionLabel__ = "__nandAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_ARG(BOOL, 0); GET_ARG(BOOL, 1);

                // schreiben in returns
                arg0->getMember() = !(arg0->getMember() & arg1->getMember());
        },
        {});

        //
        registerFunction("__norAssign__", {BOOL::typeIndex, BOOL::typeIndex},
            [__functionLabel__ = "__norAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_ARG(BOOL, 0); GET_ARG(BOOL, 1);

                // schreiben in returns
                arg0->getMember() = !(arg0->getMember() | arg1->getMember());
        },
        {});

        //
        registerFunction("__nxorAssign__", {BOOL::typeIndex, BOOL::typeIndex},
            [__functionLabel__ = "__nxorAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_ARG(BOOL, 0); GET_ARG(BOOL, 1);

                // schreiben in returns
                arg0->getMember() = !(arg0->getMember() ^ arg1->getMember());
        },
        {});

        //
        registerFunction("__negate__", {BOOL::typeIndex},
            [__functionLabel__ = "__negate__", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                // PREPARE_RETURNS;

                //
                returns.emplace_back();
                returns.back().constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                GET_LBRETURN(BOOL, 0, returns.size() - 1); GET_ARG(BOOL, 0);
                
                // schreiben in returns
                ret0->getMember() = !arg0->getMember();
        },
        {BOOL::typeIndex});

        //
        registerFunction("if", {IObject::ARGS_TYPE},
            [__functionLabel__ = "if", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                // ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                RETURNING_ASSERT(inputs.size() == 1, "If Statement erwartet genau eine bedingung",);
                RETURNING_ASSERT(inputs[0]->getData()->getTypeIndex() == BOOL::typeIndex, "If Statement erwartet Bool input",);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_RETURN(BOOL, 0);
                GET_ARG(BOOL, 0);

                ret0->getMember() = arg0->getMember();
        },
        {BOOL::typeIndex});

        //
        registerFunction("If", {IObject::ARGS_TYPE},
            [__functionLabel__ = "If", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                // ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                RETURNING_ASSERT(inputs.size() == 1, "If Statement erwartet genau eine bedingung",);
                RETURNING_ASSERT(inputs[0]->getData()->getTypeIndex() == BOOL::typeIndex, "If Statement erwartet Bool input",);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_RETURN(BOOL, 0);
                GET_ARG(BOOL, 0);

                ret0->getMember() = arg0->getMember();
        },
        {BOOL::typeIndex});

        //
        registerFunction("nIf", {IObject::ARGS_TYPE},
            [__functionLabel__ = "nIf", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                // ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_RETURN(BOOL, 0);

                //
                for(size_t argIdx = 0; argIdx < inputs.size(); argIdx++){

                    RETURNING_ASSERT(inputs[argIdx]->getData()->getTypeIndex() == BOOL::typeIndex,
                                     "if Statement benötigt Bools als inputs",);

                    bool member = static_cast<BOOL*>(inputs[argIdx]->getVariableRef().getData())->getMember();

                    if(argIdx == 0){ ret0->getMember() = member; }
                    else{ ret0->getMember() &= member; }
                }
        },
        {BOOL::typeIndex});

        //
        registerFunction("rIf", {IObject::ARGS_TYPE},
            [__functionLabel__ = "rIf", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                // ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_RETURN(BOOL, 0);

                //
                for(size_t argIdx = 0; argIdx < inputs.size(); argIdx++){

                    RETURNING_ASSERT(inputs[argIdx]->getData()->getTypeIndex() == BOOL::typeIndex,
                                     "if Statement benötigt Bools als inputs",);

                    bool member = static_cast<BOOL*>(inputs[argIdx]->getVariableRef().getData())->getMember();

                    if(argIdx == 0){ ret0->getMember() = member; }
                    else{ ret0->getMember() |= member; }
                }
        },
        {BOOL::typeIndex});

        //
        registerFunction("xIf", {IObject::ARGS_TYPE},
            [__functionLabel__ = "xIf", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                // ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_RETURN(BOOL, 0);

                //
                for(size_t argIdx = 0; argIdx < inputs.size(); argIdx++){

                    RETURNING_ASSERT(inputs[argIdx]->getData()->getTypeIndex() == BOOL::typeIndex,
                                     "if Statement benötigt Bools als inputs",);

                    bool member = static_cast<BOOL*>(inputs[argIdx]->getVariableRef().getData())->getMember();

                    if(argIdx == 0){ ret0->getMember() = member; }
                    else{ ret0->getMember() ^= member; }
                }
        },
        {BOOL::typeIndex});

        return true;
    }
};