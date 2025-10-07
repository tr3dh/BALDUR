#include "INT.h"

namespace types{

    int INT::setUpClass(){

        // register in TypeRegister
        if(!init("int", [](){ return new INT(); })){ return false; }

        // Registrierung von Keyword zur rvalue Konstruktion von ints
        // ...

        //
        // Konstruktoren
        registerFunction("int", {},
            [__functionLabel__ = "int", __numArgs__ = 0](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_RETURN(INT, 0);

                // schreiben in returns
                ret0->getMember() = 0;
        },
        {INT::typeIndex});

        // Konstruktoren
        registerFunction("int", {INT::typeIndex},
            [__functionLabel__ = "int", __numArgs__ = 1](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_RETURN(INT, 0);
                GET_ARG(INT, 0);

                // schreiben in returns
                ret0->getMember() = arg0->getMember();
        },
        {INT::typeIndex});

        // Operatoren
        registerFunction("equals", {INT::typeIndex, INT::typeIndex},
            [__functionLabel__ = "equals", __numArgs__ = 2](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_RETURN(BOOL, 0);
                GET_ARG(INT, 0); GET_ARG(INT, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() == arg1->getMember();
        },
        {INT::typeIndex});

        return true;
    }
};