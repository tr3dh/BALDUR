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
            [__functionLabel__ = "string", __numArgs__ = 0](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

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
            [__functionLabel__ = "string", __numArgs__ = 1](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

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

        // Operatoren
        registerFunction("equals", {STRING::typeIndex, STRING::typeIndex},
            [__functionLabel__ = "equals", __numArgs__ = 2](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_RETURN(BOOL, 0);
                GET_ARG(STRING, 0); GET_ARG(STRING, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() == arg1->getMember();
        },
        {BOOL::typeIndex});

        // Member
        registerMemberFunction(STRING::typeIndex, "size", {},
            [__functionLabel__ = "size", __numArgs__ = 0](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

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

        return true;
    }
};