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

        // Konstruktoren
        registerFunction("double", {INT::typeIndex},
            [__functionLabel__ = "double", __numArgs__ = 1](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_RETURN(DOUBLE, 0);
                GET_ARG(INT, 0);

                // schreiben in returns
                ret0->getMember() = static_cast<double>(arg0->getMember());
        },
        {DOUBLE::typeIndex});

        // Konstruktoren
        registerFunction("int", {DOUBLE::typeIndex},
            [__functionLabel__ = "int", __numArgs__ = 1](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_RETURN(INT, 0);
                GET_ARG(DOUBLE, 0);

                // schreiben in returns
                ret0->getMember() = static_cast<int>(arg0->getMember());
        },
        {INT::typeIndex});

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

        //
        registerFunction("__addAssign__", {DOUBLE::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__addAssign__", __numArgs__ = 2](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_ARG(DOUBLE, 0); GET_ARG(INT, 1);

                // schreiben in returns
                arg0->getMember() += arg1->getMember();
        },
        {});

        //
        registerFunction("__addAssign__", {INT::typeIndex, DOUBLE::typeIndex},
            [__functionLabel__ = "__addAssign__", __numArgs__ = 2](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

                // Asserts
                ASSERT(TRIGGER_ASSERT, "Narrowing Conversion for Int to Double");
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                //
                RECAST_INPUT_INPLACE(DOUBLE, INT, 0);

                // Returns | Inputs
                GET_ARG(DOUBLE, 0); GET_ARG(DOUBLE, 1);

                // schreiben in returns
                
                arg0->getMember() += arg1->getMember();
        },
        {});

        //
        registerFunction("__addAssign__", {DOUBLE::typeIndex, DOUBLE::typeIndex},
            [__functionLabel__ = "__addAssign__", __numArgs__ = 2](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_ARG(DOUBLE, 0); GET_ARG(DOUBLE, 1);

                // schreiben in returns
                arg0->getMember() += arg1->getMember();
        },
        {});

        return true;
    }
};