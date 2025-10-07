#include "DOUBLE.h"

namespace types{

    double DOUBLE::setUpClass(){

        // register in TypeRegister
        if(!init("double", [](){ return new DOUBLE(); })){ return false; }

        // Registrierung von Keyword zur rvalue Konstruktion von doubles
        // ...

        //
        // Konstruktoren
        registerFunction("double", {},
            [__functionLabel__ = "double", __numArgs__ = 0](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_RETURN(DOUBLE, 0);

                // schreiben in returns
                ret0->getMember() = 0;
        },
        {DOUBLE::typeIndex});

        // Konstruktoren
        registerFunction("double", {DOUBLE::typeIndex},
            [__functionLabel__ = "double", __numArgs__ = 1](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_RETURN(DOUBLE, 0);
                GET_ARG(DOUBLE, 0);

                // schreiben in returns
                ret0->getMember() = arg0->getMember();
        },
        {DOUBLE::typeIndex});

        // Operatoren
        registerFunction("equals", {DOUBLE::typeIndex, DOUBLE::typeIndex},
            [__functionLabel__ = "equals", __numArgs__ = 2](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_RETURN(BOOL, 0);
                GET_ARG(DOUBLE, 0); GET_ARG(DOUBLE, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() == arg1->getMember();
        },
        {DOUBLE::typeIndex});

        return true;
    }
};