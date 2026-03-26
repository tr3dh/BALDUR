#include "INT.h"

constexpr void intPow(int& base, int exp) {

    RETURNING_ASSERT(exp >= 0, "negativer Exponent an powert übergeben",);

    int result = 1;
    while (exp > 0) {

        if (exp & 1)
            result *= base;
        
            base *= base;
        exp >>= 1;
    }
    base = result;
}

namespace types{

    bool INT::setUpClass(){

        // register in TypeRegister
        if(!init("int", [](){ return new INT(); })){ return false; }

        // Registrierung von Keyword zur rvalue Konstruktion von ints
        // ...

        //
        // Konstruktoren
        registerFunction("int", {},
            [__functionLabel__ = "int", __numArgs__ = 0](FREG_ARGS){

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
            [__functionLabel__ = "int", __numArgs__ = 1](FREG_ARGS){

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
        registerFunction("__addAssign__", {INT::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__addAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_ARG(INT, 0); GET_ARG(INT, 1);

                // schreiben in returns
                arg0->getMember() += arg1->getMember();
        },
        {});

        // Operatoren
        registerFunction("__subAssign__", {INT::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__subAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_ARG(INT, 0); GET_ARG(INT, 1);

                // schreiben in returns
                arg0->getMember() -= arg1->getMember();
        },
        {});

        // Operatoren
        registerFunction("__mulAssign__", {INT::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__mulAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_ARG(INT, 0); GET_ARG(INT, 1);

                // schreiben in returns
                arg0->getMember() *= arg1->getMember();
        },
        {});

        // DivAssign benötigt inplace cast zu double

        //
        registerFunction("__expAssign__", {INT::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__expAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_ARG(INT, 0); GET_ARG(INT, 1);

                // schreiben in returns
                intPow(arg0->getMember(), arg1->getMember());
        },
        {});

        registerFunction("__equal__", {INT::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__equal__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_RETURN(BOOL, 0);
                GET_ARG(INT, 0); GET_ARG(INT, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() == arg1->getMember();
        },
        {BOOL::typeIndex});

        registerFunction("__notEqual__", {INT::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__notEqual__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

                returns.emplace_back();
                returns[returns.size() - 1].constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                BOOL* ret0 = static_cast<BOOL*>(returns[returns.size()-1].getVariableRef().getData());
                GET_ARG(INT, 0); GET_ARG(INT, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() != arg1->getMember();
        },
        {BOOL::typeIndex});

        registerFunction("__bigger__", {INT::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__bigger__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

                returns.emplace_back();
                returns[returns.size() - 1].constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                BOOL* ret0 = static_cast<BOOL*>(returns[returns.size()-1].getVariableRef().getData());
                GET_ARG(INT, 0); GET_ARG(INT, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() > arg1->getMember();
        },
        {BOOL::typeIndex});

        registerFunction("__biggerEqual__", {INT::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__biggerEqual__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

                returns.emplace_back();
                returns[returns.size() - 1].constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                BOOL* ret0 = static_cast<BOOL*>(returns[returns.size()-1].getVariableRef().getData());
                GET_ARG(INT, 0); GET_ARG(INT, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() >= arg1->getMember();
        },
        {BOOL::typeIndex});

        registerFunction("__smaller__", {INT::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__smaller__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

                returns.emplace_back();
                returns[returns.size() - 1].constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                BOOL* ret0 = static_cast<BOOL*>(returns[returns.size()-1].getVariableRef().getData());
                GET_ARG(INT, 0); GET_ARG(INT, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() < arg1->getMember();
        },
        {BOOL::typeIndex});

        registerFunction("__smallerEqual__", {INT::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__smallerEqual__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

                returns.emplace_back();
                returns[returns.size() - 1].constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                BOOL* ret0 = static_cast<BOOL*>(returns[returns.size()-1].getVariableRef().getData());
                GET_ARG(INT, 0); GET_ARG(INT, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() <= arg1->getMember();
        },
        {BOOL::typeIndex});

        //
        registerFunction("__negate__", {INT::typeIndex},
            [__functionLabel__ = "__negate__", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                // PREPARE_RETURNS;

                // Returns | Inputs
                returns.emplace_back();
                returns.back().constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                GET_LBRETURN(INT, 0, returns.size() - 1); GET_ARG(INT, 0);

                // schreiben in returns
                ret0->getMember() = -arg0->getMember();
        },
        {INT::typeIndex});

        //
        registerFunction("__increment__", {INT::typeIndex},
            [__functionLabel__ = "__increment__", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_RETURN(INT, 0);
                GET_ARG(INT, 0);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() + 1;
                arg0->getMember() += 1;
        },
        {INT::typeIndex});

        //
        registerFunction("__decrement__", {INT::typeIndex},
            [__functionLabel__ = "__decrement__", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_RETURN(INT, 0);
                GET_ARG(INT, 0);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() - 1;
                arg0->getMember() -= 1;
        },
        {INT::typeIndex});

        //
        registerFunction("sleep", {INT::typeIndex},
            [__functionLabel__ = "sleep", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_ARG(INT, 0);

                // schreiben in returns
                std::this_thread::sleep_for(std::chrono::seconds(arg0->getMember()));
        },
        {});

        //
        registerFunction("__modulo__", {INT::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__modulo__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                // PREPARE_RETURNS;

                // Returns | Inputs
                returns.emplace_back();
                returns[returns.size() - 1].constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                INT* ret0 = static_cast<INT*>(returns[returns.size()-1].getVariableRef().getData());
                GET_ARG(INT, 0); GET_ARG(INT, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() % arg1->getMember();
        },
        {INT::typeIndex});

        registerFunction("calledAs", {INT::typeIndex},
            [__functionLabel__ = "calledAs", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                //
                GET_RETURN(BOOL, 0);
                GET_ARG(INT, 0);
                INT* calledAs = static_cast<INT*>(returnToScope.getVariable("__ScriptCalledAs__")->getData());

                RETURNING_ASSERT(calledAs != nullptr, "Variable __ScriptCalledAs__ wurde im Scope nicht gefunde",);

                // schreiben in returns
                ret0->getMember() = calledAs->getMember() == arg0->getMember();
        },
        {BOOL::typeIndex});

        return true;
    }
};