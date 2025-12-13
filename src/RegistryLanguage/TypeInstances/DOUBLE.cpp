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
            [__functionLabel__ = "double", __numArgs__ = 0](FREG_ARGS){

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
            [__functionLabel__ = "double", __numArgs__ = 1](FREG_ARGS){

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
            [__functionLabel__ = "double", __numArgs__ = 1](FREG_ARGS){

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
            [__functionLabel__ = "int", __numArgs__ = 1](FREG_ARGS){

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

        //
        registerFunction("__addAssign__", {DOUBLE::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__addAssign__", __numArgs__ = 2](FREG_ARGS){

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
            [__functionLabel__ = "__addAssign__", __numArgs__ = 2](FREG_ARGS){

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
            [__functionLabel__ = "__addAssign__", __numArgs__ = 2](FREG_ARGS){

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

        //
        registerFunction("__subAssign__", {DOUBLE::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__subAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_ARG(DOUBLE, 0); GET_ARG(INT, 1);

                // schreiben in returns
                arg0->getMember() -= arg1->getMember();
        },
        {});

        //
        registerFunction("__subAssign__", {INT::typeIndex, DOUBLE::typeIndex},
            [__functionLabel__ = "__subAssign__", __numArgs__ = 2](FREG_ARGS){

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
                
                arg0->getMember() -= arg1->getMember();
        },
        {});

        //
        registerFunction("__subAssign__", {DOUBLE::typeIndex, DOUBLE::typeIndex},
            [__functionLabel__ = "__subAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_ARG(DOUBLE, 0); GET_ARG(DOUBLE, 1);

                // schreiben in returns
                arg0->getMember() -= arg1->getMember();
        },
        {});

        //
        registerFunction("__mulAssign__", {DOUBLE::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__mulAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_ARG(DOUBLE, 0); GET_ARG(INT, 1);

                // schreiben in returns
                arg0->getMember() *= arg1->getMember();
        },
        {});

        //
        registerFunction("__mulAssign__", {INT::typeIndex, DOUBLE::typeIndex},
            [__functionLabel__ = "__mulAssign__", __numArgs__ = 2](FREG_ARGS){

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
                
                arg0->getMember() *= arg1->getMember();
        },
        {});

        //
        registerFunction("__mulAssign__", {DOUBLE::typeIndex, DOUBLE::typeIndex},
            [__functionLabel__ = "__mulAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_ARG(DOUBLE, 0); GET_ARG(DOUBLE, 1);

                // schreiben in returns
                arg0->getMember() *= arg1->getMember();
        },
        {});

        //
        registerFunction("__divAssign__", {INT::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__divAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT(TRIGGER_ASSERT, "Narrowing Conversion for Int to Double");
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                //
                RECAST_INPUT_INPLACE(DOUBLE, INT, 0);

                // Returns | Inputs
                GET_ARG(DOUBLE, 0); GET_ARG(INT, 1);

                // schreiben in returns
                
                arg0->getMember() /= arg1->getMember();
        },
        {});

        registerFunction("__divAssign__", {DOUBLE::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__divAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_ARG(DOUBLE, 0); GET_ARG(INT, 1);

                // schreiben in returns
                arg0->getMember() /= arg1->getMember();
        },
        {});

        //
        registerFunction("__divAssign__", {INT::typeIndex, DOUBLE::typeIndex},
            [__functionLabel__ = "__divAssign__", __numArgs__ = 2](FREG_ARGS){

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
                
                arg0->getMember() /= arg1->getMember();
        },
        {});

        //
        registerFunction("__divAssign__", {DOUBLE::typeIndex, DOUBLE::typeIndex},
            [__functionLabel__ = "__divAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_ARG(DOUBLE, 0); GET_ARG(DOUBLE, 1);

                // schreiben in returns
                arg0->getMember() /= arg1->getMember();
        },
        {});

        registerFunction("__expAssign__", {DOUBLE::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__expAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_ARG(DOUBLE, 0); GET_ARG(INT, 1);

                // schreiben in returns
                arg0->getMember() = pow(arg0->getMember(), arg1->getMember());
        },
        {});

        //
        registerFunction("__expAssign__", {INT::typeIndex, DOUBLE::typeIndex},
            [__functionLabel__ = "__expAssign__", __numArgs__ = 2](FREG_ARGS){

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
                
                arg0->getMember() = pow(arg0->getMember(), arg1->getMember());
        },
        {});

        //
        registerFunction("__expAssign__", {DOUBLE::typeIndex, DOUBLE::typeIndex},
            [__functionLabel__ = "__expAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_ARG(DOUBLE, 0); GET_ARG(DOUBLE, 1);

                // schreiben in returns
                arg0->getMember() = pow(arg0->getMember(), arg1->getMember());
        },
        {});

        // Operatoren
        registerFunction("__equal__", {DOUBLE::typeIndex, DOUBLE::typeIndex},
            [__functionLabel__ = "__equal__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                
                returns.emplace_back();
                returns[returns.size() - 1].constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                BOOL* ret0 = static_cast<BOOL*>(returns[returns.size()-1].getVariableRef().getData());

                GET_ARG(DOUBLE, 0); GET_ARG(DOUBLE, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() == arg1->getMember();
        },
        {BOOL::typeIndex});

        // Operatoren
        registerFunction("__equal__", {INT::typeIndex, DOUBLE::typeIndex},
            [__functionLabel__ = "__equal__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                
                returns.emplace_back();
                returns[returns.size() - 1].constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                BOOL* ret0 = static_cast<BOOL*>(returns[returns.size()-1].getVariableRef().getData());

                GET_ARG(INT, 0); GET_ARG(DOUBLE, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() == arg1->getMember();
        },
        {BOOL::typeIndex});

        // Operatoren
        registerFunction("__equal__", {DOUBLE::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__equal__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                
                returns.emplace_back();
                returns[returns.size() - 1].constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                BOOL* ret0 = static_cast<BOOL*>(returns[returns.size()-1].getVariableRef().getData());

                GET_ARG(DOUBLE, 0); GET_ARG(INT, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() == arg1->getMember();
        },
        {BOOL::typeIndex});

        // Operatoren
        registerFunction("__notEqual__", {DOUBLE::typeIndex, DOUBLE::typeIndex},
            [__functionLabel__ = "__notEqual__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                
                returns.emplace_back();
                returns[returns.size() - 1].constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                BOOL* ret0 = static_cast<BOOL*>(returns[returns.size()-1].getVariableRef().getData());

                GET_ARG(DOUBLE, 0); GET_ARG(DOUBLE, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() != arg1->getMember();
        },
        {BOOL::typeIndex});

        // Operatoren
        registerFunction("__notEqual__", {INT::typeIndex, DOUBLE::typeIndex},
            [__functionLabel__ = "__notEqual__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                
                returns.emplace_back();
                returns[returns.size() - 1].constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                BOOL* ret0 = static_cast<BOOL*>(returns[returns.size()-1].getVariableRef().getData());

                GET_ARG(INT, 0); GET_ARG(DOUBLE, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() != arg1->getMember();
        },
        {BOOL::typeIndex});

        // Operatoren
        registerFunction("__notEqual__", {DOUBLE::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__notEqual__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                
                returns.emplace_back();
                returns[returns.size() - 1].constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                BOOL* ret0 = static_cast<BOOL*>(returns[returns.size()-1].getVariableRef().getData());

                GET_ARG(DOUBLE, 0); GET_ARG(INT, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() != arg1->getMember();
        },
        {BOOL::typeIndex});

        // Operatoren
        registerFunction("__bigger__", {DOUBLE::typeIndex, DOUBLE::typeIndex},
            [__functionLabel__ = "__bigger__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                
                returns.emplace_back();
                returns[returns.size() - 1].constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                BOOL* ret0 = static_cast<BOOL*>(returns[returns.size()-1].getVariableRef().getData());

                GET_ARG(DOUBLE, 0); GET_ARG(DOUBLE, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() > arg1->getMember();
        },
        {BOOL::typeIndex});

        // Operatoren
        registerFunction("__bigger__", {INT::typeIndex, DOUBLE::typeIndex},
            [__functionLabel__ = "__bigger__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                
                returns.emplace_back();
                returns[returns.size() - 1].constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                BOOL* ret0 = static_cast<BOOL*>(returns[returns.size()-1].getVariableRef().getData());

                GET_ARG(INT, 0); GET_ARG(DOUBLE, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() > arg1->getMember();
        },
        {BOOL::typeIndex});

        // Operatoren
        registerFunction("__bigger__", {DOUBLE::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__bigger__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                
                returns.emplace_back();
                returns[returns.size() - 1].constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                BOOL* ret0 = static_cast<BOOL*>(returns[returns.size()-1].getVariableRef().getData());

                GET_ARG(DOUBLE, 0); GET_ARG(INT, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() > arg1->getMember();
        },
        {BOOL::typeIndex});

        // Operatoren
        registerFunction("__biggerEqual__", {DOUBLE::typeIndex, DOUBLE::typeIndex},
            [__functionLabel__ = "__biggerEqual__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                
                returns.emplace_back();
                returns[returns.size() - 1].constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                BOOL* ret0 = static_cast<BOOL*>(returns[returns.size()-1].getVariableRef().getData());

                GET_ARG(DOUBLE, 0); GET_ARG(DOUBLE, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() >= arg1->getMember();
        },
        {BOOL::typeIndex});

        // Operatoren
        registerFunction("__biggerEqual__", {INT::typeIndex, DOUBLE::typeIndex},
            [__functionLabel__ = "__biggerEqual__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                
                returns.emplace_back();
                returns[returns.size() - 1].constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                BOOL* ret0 = static_cast<BOOL*>(returns[returns.size()-1].getVariableRef().getData());

                GET_ARG(INT, 0); GET_ARG(DOUBLE, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() >= arg1->getMember();
        },
        {BOOL::typeIndex});

        // Operatoren
        registerFunction("__biggerEqual__", {DOUBLE::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__biggerEqual__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                
                returns.emplace_back();
                returns[returns.size() - 1].constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                BOOL* ret0 = static_cast<BOOL*>(returns[returns.size()-1].getVariableRef().getData());

                GET_ARG(DOUBLE, 0); GET_ARG(INT, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() >= arg1->getMember();
        },
        {BOOL::typeIndex});

        // Operatoren
        registerFunction("__smaller__", {DOUBLE::typeIndex, DOUBLE::typeIndex},
            [__functionLabel__ = "__smaller__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                
                returns.emplace_back();
                returns[returns.size() - 1].constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                BOOL* ret0 = static_cast<BOOL*>(returns[returns.size()-1].getVariableRef().getData());

                GET_ARG(DOUBLE, 0); GET_ARG(DOUBLE, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() < arg1->getMember();
        },
        {BOOL::typeIndex});

        // Operatoren
        registerFunction("__smaller__", {INT::typeIndex, DOUBLE::typeIndex},
            [__functionLabel__ = "__smaller__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                
                returns.emplace_back();
                returns[returns.size() - 1].constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                BOOL* ret0 = static_cast<BOOL*>(returns[returns.size()-1].getVariableRef().getData());

                GET_ARG(INT, 0); GET_ARG(DOUBLE, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() < arg1->getMember();
        },
        {BOOL::typeIndex});

        // Operatoren
        registerFunction("__smaller__", {DOUBLE::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__smaller__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                
                returns.emplace_back();
                returns[returns.size() - 1].constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                BOOL* ret0 = static_cast<BOOL*>(returns[returns.size()-1].getVariableRef().getData());

                GET_ARG(DOUBLE, 0); GET_ARG(INT, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() < arg1->getMember();
        },
        {BOOL::typeIndex});

        // Operatoren
        registerFunction("__smallerEqual__", {DOUBLE::typeIndex, DOUBLE::typeIndex},
            [__functionLabel__ = "__smallerEqual__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                
                returns.emplace_back();
                returns[returns.size() - 1].constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                BOOL* ret0 = static_cast<BOOL*>(returns[returns.size()-1].getVariableRef().getData());

                GET_ARG(DOUBLE, 0); GET_ARG(DOUBLE, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() <= arg1->getMember();
        },
        {BOOL::typeIndex});

        // Operatoren
        registerFunction("__smallerEqual__", {INT::typeIndex, DOUBLE::typeIndex},
            [__functionLabel__ = "__smallerEqual__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                
                returns.emplace_back();
                returns[returns.size() - 1].constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                BOOL* ret0 = static_cast<BOOL*>(returns[returns.size()-1].getVariableRef().getData());

                GET_ARG(INT, 0); GET_ARG(DOUBLE, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() <= arg1->getMember();
        },
        {BOOL::typeIndex});

        // Operatoren
        registerFunction("__smallerEqual__", {DOUBLE::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__smallerEqual__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                
                returns.emplace_back();
                returns[returns.size() - 1].constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                BOOL* ret0 = static_cast<BOOL*>(returns[returns.size()-1].getVariableRef().getData());

                GET_ARG(DOUBLE, 0); GET_ARG(INT, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() <= arg1->getMember();
        },
        {BOOL::typeIndex});

        //
        registerFunction("__negate__", {DOUBLE::typeIndex},
            [__functionLabel__ = "__negate__", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_RETURN(DOUBLE, 0); GET_ARG(DOUBLE, 0);

                // schreiben in returns
                ret0->getMember() = -arg0->getMember();
        },
        {DOUBLE::typeIndex});

        //
        registerFunction("sleep", {DOUBLE::typeIndex},
            [__functionLabel__ = "sleep", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_ARG(DOUBLE, 0);

                // schreiben in returns
                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(arg0->getMember() * 1000)));
        },
        {});

        //
        registerFunction("sum", {IObject::ARGS_TYPE},
            [__functionLabel__ = "sum", __numArgs__ = 0](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                // ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_RETURN(DOUBLE, 0);
                double& res = ret0->getMember();

                for (size_t argIdx = 0; argIdx < inputs.size(); argIdx++) {

                    RETURNING_ASSERT(
                        inputs[argIdx]->getData()->getTypeIndex() == DOUBLE::typeIndex ||
                        inputs[argIdx]->getData()->getTypeIndex() == INT::typeIndex,
                        "sum benötigt INT oder DOUBLE als Inputs",
                    );

                    if(inputs[argIdx]->getData()->getTypeIndex() == DOUBLE::typeIndex) {
                        res += static_cast<DOUBLE*>(inputs[argIdx]->getVariableRef().getData())->getMember();
                    } else {
                        res += static_cast<double>(static_cast<INT*>(inputs[argIdx]->getVariableRef().getData())->getMember());
                    }
                }
        },
        {DOUBLE::typeIndex});

        return true;
    }
};