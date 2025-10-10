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
                PREPARE_RETURNS;

                // Returns | Inputs
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
            [__functionLabel__ = "__equal__", __numArgs__ = 2](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

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
            [__functionLabel__ = "__notEqual__", __numArgs__ = 2](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

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
            [__functionLabel__ = "__andAssign__", __numArgs__ = 2](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

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
            [__functionLabel__ = "__orAssign__", __numArgs__ = 2](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

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
            [__functionLabel__ = "__xorAssign__", __numArgs__ = 2](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

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
            [__functionLabel__ = "__nandAssign__", __numArgs__ = 2](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

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
            [__functionLabel__ = "__norAssign__", __numArgs__ = 2](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

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
            [__functionLabel__ = "__nxorAssign__", __numArgs__ = 2](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

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
            [__functionLabel__ = "__negate__", __numArgs__ = 1](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_ARG(BOOL, 0);

                // schreiben in returns
                arg0->getMember() = !arg0->getMember();
        },
        {});

        return true;
    }
};