#include "BOOL.h"

namespace types{

    bool BOOL::setUpClass(){

        // register in TypeRegister
        if(!init("bool", [](){ return new BOOL(); })){ return false; }

        // Registrierung von Keyword zur rvalue Konstruktion von bools
        registerKeyword("true", new BOOL(true));
        registerKeyword("false", new BOOL(false));

        // Konstruktoren
        registerFunction("bool", {},
            [__functionLabel__ = "bool", __numArgs__ = 0](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

                //
                PREPARE_RETURNS;

                // Returns
                GET_RETURN(BOOL, 0);

                // schreiben in returns
                ret0->getMember() = false;
        },
        {BOOL::typeIndex});

        // Konstruktoren
        registerFunction("bool", {BOOL::typeIndex},
            [__functionLabel__ = "bool", __numArgs__ = 1](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

                //
                PREPARE_RETURNS;

                // Returns
                GET_RETURN(BOOL, 0);

                //
                GET_ARG(BOOL, 0);

                // schreiben in returns
                ret0->getMember() = arg0->getMember();
        },
        {BOOL::typeIndex});

        // Operatoren
        registerFunction("equals", {BOOL::typeIndex, BOOL::typeIndex},
            [__functionLabel__ = "equals", __numArgs__ = 2](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

                //
                PREPARE_RETURNS;

                // Returns
                GET_RETURN(BOOL, 0);

                // Inputs
                GET_ARG(BOOL, 0);
                GET_ARG(BOOL, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() == arg1->getMember();
        },
        {BOOL::typeIndex});

        return true;
    }
};