#include "STRING.h"

namespace types{

    bool STRING::setUpClass(){

        // register in TypeRegister
        if(!init("string", [](){ return new STRING(); })){ return false; }

        // Registrierung von Keyword zur rvalue Konstruktion von strings
        // ...

        //
        // Konstruktoren
        registerFunction("string", {},
            [__functionLabel__ = "string", __numArgs__ = 0](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_RETURN(STRING, 0);

                // schreiben in returns
                ret0->getMember() = "__NULLSTR__";
        },
        {STRING::typeIndex});

        // Konstruktoren
        registerFunction("string", {STRING::typeIndex},
            [__functionLabel__ = "string", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_RETURN(STRING, 0);
                GET_ARG(STRING, 0);

                // schreiben in returns
                ret0->getMember() = inputs[0]->isRValue() ? std::move(arg0->getMember()) : arg0->getMember();
        },
        {STRING::typeIndex});

        // Member
        registerMemberFunction(STRING::typeIndex, "size", {},
            [__functionLabel__ = "size", __numArgs__ = 0](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_RETURN(INT, 0);
                GET_MEMBER(STRING);

                // schreiben in returns
                ret0->getMember() = mb->getMember().size();
        },
        {INT::typeIndex});

        // Operatoren
        registerFunction("__equal__", {STRING::typeIndex, STRING::typeIndex},
            [__functionLabel__ = "__equal__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                
                returns.emplace_back();
                returns[returns.size() - 1].constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                BOOL* ret0 = static_cast<BOOL*>(returns[returns.size()-1].getVariableRef().getData());

                GET_ARG(STRING, 0); GET_ARG(STRING, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() == arg1->getMember();
        },
        {BOOL::typeIndex});

        // Operatoren
        registerFunction("__notEqual__", {STRING::typeIndex, STRING::typeIndex},
            [__functionLabel__ = "__notEqual__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                
                returns.emplace_back();
                returns[returns.size() - 1].constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                BOOL* ret0 = static_cast<BOOL*>(returns[returns.size()-1].getVariableRef().getData());

                GET_ARG(STRING, 0); GET_ARG(STRING, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() != arg1->getMember();
        },
        {BOOL::typeIndex});

        return true;
    }
};