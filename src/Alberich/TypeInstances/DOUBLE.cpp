#include "DOUBLE.h"

namespace types{

    double nthRoot(double value, double n) {
        
        if (value == 0) return 0;
        if (value < 0 && static_cast<int>(n) % 2 == 0) {
            
            // Gerade Wurzel aus negativer Zahl nicht definiert
            return std::numeric_limits<double>::quiet_NaN();
        }
        
        // Für negative Werte bei ungerader Wurzel
        double sign = (value < 0 && static_cast<int>(n) % 2 == 1) ? -1.0 : 1.0;
        return sign * std::exp(std::log(std::abs(value)) / n);
    }

    bool DOUBLE::setUpClass(){

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
        registerFunction("sqrt", {DOUBLE::typeIndex},
            [__functionLabel__ = "sqrt", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_RETURN(DOUBLE, 0);
                GET_ARG(DOUBLE, 0);

                // schreiben in returns
                ret0->getMember() = std::sqrt(arg0->getMember());
        },
        {DOUBLE::typeIndex});

        //
        registerFunction("sqrt", {INT::typeIndex},
            [__functionLabel__ = "sqrt", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_RETURN(DOUBLE, 0);
                GET_ARG(INT, 0);

                // schreiben in returns
                ret0->getMember() = std::sqrt(static_cast<double>(arg0->getMember()));
        },
        {DOUBLE::typeIndex});

        // nthRoot(int, int) -> double
        registerFunction("sqrt", {INT::typeIndex, INT::typeIndex},
            [__functionLabel__ = "sqrt", __numArgs__ = 2](FREG_ARGS){
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_RETURN(DOUBLE, 0);
                GET_ARG(INT, 0); GET_ARG(INT, 1);

                ret0->getMember() = nthRoot(static_cast<double>(arg0->getMember()), 
                                            static_cast<double>(arg1->getMember()));
            },
            {DOUBLE::typeIndex});

        // nthRoot(int, double) -> double
        registerFunction("sqrt", {INT::typeIndex, DOUBLE::typeIndex},
            [__functionLabel__ = "sqrt", __numArgs__ = 2](FREG_ARGS){
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_RETURN(DOUBLE, 0);
                GET_ARG(INT, 0); GET_ARG(DOUBLE, 1);

                ret0->getMember() = nthRoot(static_cast<double>(arg0->getMember()), 
                                            arg1->getMember());
            },
            {DOUBLE::typeIndex});

        // nthRoot(double, int) -> double
        registerFunction("sqrt", {DOUBLE::typeIndex, INT::typeIndex},
            [__functionLabel__ = "sqrt", __numArgs__ = 2](FREG_ARGS){
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_RETURN(DOUBLE, 0);
                GET_ARG(DOUBLE, 0); GET_ARG(INT, 1);

                ret0->getMember() = nthRoot(arg0->getMember(), 
                                            static_cast<double>(arg1->getMember()));
            },
            {DOUBLE::typeIndex});

        // nthRoot(double, double) -> double
        registerFunction("sqrt", {DOUBLE::typeIndex, DOUBLE::typeIndex},
            [__functionLabel__ = "sqrt", __numArgs__ = 2](FREG_ARGS){
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_RETURN(DOUBLE, 0);
                GET_ARG(DOUBLE, 0); GET_ARG(DOUBLE, 1);

                ret0->getMember() = nthRoot(arg0->getMember(), arg1->getMember());
            },
            {DOUBLE::typeIndex});

        // sin(int) -> double
        registerFunction("sin", {INT::typeIndex},
            [__functionLabel__ = "sin", __numArgs__ = 1](FREG_ARGS){
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_RETURN(DOUBLE, 0);
                GET_ARG(INT, 0);

                ret0->getMember() = std::sin(static_cast<double>(arg0->getMember()));
            },
            {DOUBLE::typeIndex});

        // sin(double) -> double
        registerFunction("sin", {DOUBLE::typeIndex},
            [__functionLabel__ = "sin", __numArgs__ = 1](FREG_ARGS){
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_RETURN(DOUBLE, 0);
                GET_ARG(DOUBLE, 0);

                ret0->getMember() = std::sin(arg0->getMember());
            },
            {DOUBLE::typeIndex});

        // cos(int) -> double
        registerFunction("cos", {INT::typeIndex},
            [__functionLabel__ = "cos", __numArgs__ = 1](FREG_ARGS){
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_RETURN(DOUBLE, 0);
                GET_ARG(INT, 0);

                ret0->getMember() = std::cos(static_cast<double>(arg0->getMember()));
            },
            {DOUBLE::typeIndex});

        // cos(double) -> double
        registerFunction("cos", {DOUBLE::typeIndex},
            [__functionLabel__ = "cos", __numArgs__ = 1](FREG_ARGS){
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_RETURN(DOUBLE, 0);
                GET_ARG(DOUBLE, 0);

                ret0->getMember() = std::cos(arg0->getMember());
            },
            {DOUBLE::typeIndex});

        // tan(int) -> double
        registerFunction("tan", {INT::typeIndex},
            [__functionLabel__ = "tan", __numArgs__ = 1](FREG_ARGS){
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_RETURN(DOUBLE, 0);
                GET_ARG(INT, 0);

                ret0->getMember() = std::tan(static_cast<double>(arg0->getMember()));
            },
            {DOUBLE::typeIndex});

        // tan(double) -> double
        registerFunction("tan", {DOUBLE::typeIndex},
            [__functionLabel__ = "tan", __numArgs__ = 1](FREG_ARGS){
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_RETURN(DOUBLE, 0);
                GET_ARG(DOUBLE, 0);

                ret0->getMember() = std::tan(arg0->getMember());
            },
            {DOUBLE::typeIndex});

        // cotan(int) -> double
        registerFunction("cotan", {INT::typeIndex},
            [__functionLabel__ = "cotan", __numArgs__ = 1](FREG_ARGS){
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_RETURN(DOUBLE, 0);
                GET_ARG(INT, 0);

                ret0->getMember() = 1.0 / std::tan(static_cast<double>(arg0->getMember()));
            },
            {DOUBLE::typeIndex});

        // cotan(double) -> double
        registerFunction("cotan", {DOUBLE::typeIndex},
            [__functionLabel__ = "cotan", __numArgs__ = 1](FREG_ARGS){
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_RETURN(DOUBLE, 0);
                GET_ARG(DOUBLE, 0);

                ret0->getMember() = 1.0 / std::tan(arg0->getMember());
            },
            {DOUBLE::typeIndex});

        // arcsin(int) -> double
        registerFunction("arcsin", {INT::typeIndex},
            [__functionLabel__ = "arcsin", __numArgs__ = 1](FREG_ARGS){
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_RETURN(DOUBLE, 0);
                GET_ARG(INT, 0);

                ret0->getMember() = std::asin(static_cast<double>(arg0->getMember()));
            },
            {DOUBLE::typeIndex});

        // arcsin(double) -> double
        registerFunction("arcsin", {DOUBLE::typeIndex},
            [__functionLabel__ = "arcsin", __numArgs__ = 1](FREG_ARGS){
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_RETURN(DOUBLE, 0);
                GET_ARG(DOUBLE, 0);

                ret0->getMember() = std::asin(arg0->getMember());
            },
            {DOUBLE::typeIndex});

        // arccos(int) -> double
        registerFunction("arccos", {INT::typeIndex},
            [__functionLabel__ = "arccos", __numArgs__ = 1](FREG_ARGS){
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_RETURN(DOUBLE, 0);
                GET_ARG(INT, 0);

                ret0->getMember() = std::acos(static_cast<double>(arg0->getMember()));
            },
            {DOUBLE::typeIndex});

        // arccos(double) -> double
        registerFunction("arccos", {DOUBLE::typeIndex},
            [__functionLabel__ = "arccos", __numArgs__ = 1](FREG_ARGS){
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_RETURN(DOUBLE, 0);
                GET_ARG(DOUBLE, 0);

                ret0->getMember() = std::acos(arg0->getMember());
            },
            {DOUBLE::typeIndex});

        // arctan(int) -> double
        registerFunction("arctan", {INT::typeIndex},
            [__functionLabel__ = "arctan", __numArgs__ = 1](FREG_ARGS){
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_RETURN(DOUBLE, 0);
                GET_ARG(INT, 0);

                ret0->getMember() = std::atan(static_cast<double>(arg0->getMember()));
            },
            {DOUBLE::typeIndex});

        // arctan(double) -> double
        registerFunction("arctan", {DOUBLE::typeIndex},
            [__functionLabel__ = "arctan", __numArgs__ = 1](FREG_ARGS){
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_RETURN(DOUBLE, 0);
                GET_ARG(DOUBLE, 0);

                ret0->getMember() = std::atan(arg0->getMember());
            },
            {DOUBLE::typeIndex});

        // arccotan(int) -> double
        registerFunction("arccotan", {INT::typeIndex},
            [__functionLabel__ = "arccotan", __numArgs__ = 1](FREG_ARGS){
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_RETURN(DOUBLE, 0);
                GET_ARG(INT, 0);

                ret0->getMember() = std::atan(1.0 / static_cast<double>(arg0->getMember()));
            },
            {DOUBLE::typeIndex});

        // arccotan(double) -> double
        registerFunction("arccotan", {DOUBLE::typeIndex},
            [__functionLabel__ = "arccotan", __numArgs__ = 1](FREG_ARGS){
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_RETURN(DOUBLE, 0);
                GET_ARG(DOUBLE, 0);

                ret0->getMember() = std::atan(1.0 / arg0->getMember());
            },
            {DOUBLE::typeIndex});

    // atan2(int, int) -> double
    registerFunction("atan2", {INT::typeIndex, INT::typeIndex},
        [__functionLabel__ = "atan2", __numArgs__ = 2](FREG_ARGS){
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            GET_RETURN(DOUBLE, 0);
            GET_ARG(INT, 0); GET_ARG(INT, 1);

            ret0->getMember() = std::atan2(static_cast<double>(arg0->getMember()), 
                                            static_cast<double>(arg1->getMember()));
        },
        {DOUBLE::typeIndex});

    // atan2(int, double) -> double
    registerFunction("atan2", {INT::typeIndex, DOUBLE::typeIndex},
        [__functionLabel__ = "atan2", __numArgs__ = 2](FREG_ARGS){
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            GET_RETURN(DOUBLE, 0);
            GET_ARG(INT, 0); GET_ARG(DOUBLE, 1);

            ret0->getMember() = std::atan2(static_cast<double>(arg0->getMember()), 
                                            arg1->getMember());
        },
        {DOUBLE::typeIndex});

    // atan2(double, int) -> double
    registerFunction("atan2", {DOUBLE::typeIndex, INT::typeIndex},
        [__functionLabel__ = "atan2", __numArgs__ = 2](FREG_ARGS){
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            GET_RETURN(DOUBLE, 0);
            GET_ARG(DOUBLE, 0); GET_ARG(INT, 1);

            ret0->getMember() = std::atan2(arg0->getMember(), 
                                            static_cast<double>(arg1->getMember()));
        },
        {DOUBLE::typeIndex});

    // atan2(double, double) -> double
    registerFunction("atan2", {DOUBLE::typeIndex, DOUBLE::typeIndex},
        [__functionLabel__ = "atan2", __numArgs__ = 2](FREG_ARGS){
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            GET_RETURN(DOUBLE, 0);
            GET_ARG(DOUBLE, 0); GET_ARG(DOUBLE, 1);

            ret0->getMember() = std::atan2(arg0->getMember(), arg1->getMember());
        },
        {DOUBLE::typeIndex});

    // log(int) -> double
    registerFunction("log", {INT::typeIndex},
        [__functionLabel__ = "log", __numArgs__ = 1](FREG_ARGS){
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            GET_RETURN(DOUBLE, 0);
            GET_ARG(INT, 0);

            ret0->getMember() = std::log(static_cast<double>(arg0->getMember()));
        },
        {DOUBLE::typeIndex});

    // log(double) -> double
    registerFunction("log", {DOUBLE::typeIndex},
        [__functionLabel__ = "log", __numArgs__ = 1](FREG_ARGS){
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            GET_RETURN(DOUBLE, 0);
            GET_ARG(DOUBLE, 0);

            ret0->getMember() = std::log(arg0->getMember());
        },
        {DOUBLE::typeIndex});

    // log(int, int) -> double (basis, value)
    registerFunction("log", {INT::typeIndex, INT::typeIndex},
        [__functionLabel__ = "log", __numArgs__ = 2](FREG_ARGS){
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            GET_RETURN(DOUBLE, 0);
            GET_ARG(INT, 0); GET_ARG(INT, 1);

            ret0->getMember() = std::log(static_cast<double>(arg1->getMember())) / 
                                std::log(static_cast<double>(arg0->getMember()));
        },
        {DOUBLE::typeIndex});

    // log(int, double) -> double (basis, value)
    registerFunction("log", {INT::typeIndex, DOUBLE::typeIndex},
        [__functionLabel__ = "log", __numArgs__ = 2](FREG_ARGS){
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            GET_RETURN(DOUBLE, 0);
            GET_ARG(INT, 0); GET_ARG(DOUBLE, 1);

            ret0->getMember() = std::log(arg1->getMember()) / 
                                std::log(static_cast<double>(arg0->getMember()));
        },
        {DOUBLE::typeIndex});

    // log(double, int) -> double (basis, value)
    registerFunction("log", {DOUBLE::typeIndex, INT::typeIndex},
        [__functionLabel__ = "log", __numArgs__ = 2](FREG_ARGS){
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            GET_RETURN(DOUBLE, 0);
            GET_ARG(DOUBLE, 0); GET_ARG(INT, 1);

            ret0->getMember() = std::log(static_cast<double>(arg1->getMember())) / 
                                std::log(arg0->getMember());
        },
        {DOUBLE::typeIndex});

    // log(double, double) -> double (basis, value)
    registerFunction("log", {DOUBLE::typeIndex, DOUBLE::typeIndex},
        [__functionLabel__ = "log", __numArgs__ = 2](FREG_ARGS){
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            GET_RETURN(DOUBLE, 0);
            GET_ARG(DOUBLE, 0); GET_ARG(DOUBLE, 1);

            ret0->getMember() = std::log(arg1->getMember()) / std::log(arg0->getMember());
        },
        {DOUBLE::typeIndex});

    registerFunction("now", {},
        [__functionLabel__ = "now", __numArgs__ = 0](FREG_ARGS){

            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            GET_RETURN(DOUBLE, 0);

            ret0->getMember() = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
        },
        {DOUBLE::typeIndex});

        return true;
    }
};