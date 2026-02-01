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

        // Konstruktoren
        registerFunction("string", {INT::typeIndex},
            [__functionLabel__ = "string", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_RETURN(STRING, 0);
                GET_ARG(INT, 0);

                // schreiben in returns
                ret0->getMember() = std::to_string(arg0->getMember());
        },
        {STRING::typeIndex});

        // Konstruktoren
        registerFunction("string", {DOUBLE::typeIndex},
            [__functionLabel__ = "string", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_RETURN(STRING, 0);
                GET_ARG(DOUBLE, 0);

                // schreiben in returns
                ret0->getMember() = std::to_string(arg0->getMember());
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
        registerFunction("__addAssign__", {STRING::typeIndex, STRING::typeIndex},
            [__functionLabel__ = "__addAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_ARG(STRING, 0); GET_ARG(STRING, 1);

                // schreiben in returns
                arg0->getMember() += arg1->getMember();
        },
        {});

        //
        registerFunction("__mulAssign__", {INT::typeIndex, STRING::typeIndex},
            [__functionLabel__ = "__mulAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                //
                int multiplier = static_cast<INT*>(inputs[0]->getData())->getMember();

                //
                RECAST_INPUT_INPLACE(STRING, INT, 0);

                // Returns | Inputs
                GET_ARG(STRING, 0); GET_ARG(STRING, 1);

                // schreiben in returns
                arg0->getMember() = std::string();

                //
                arg0->getMember().reserve(arg1->getMember().size() * multiplier);
                for (int i = 0; i < multiplier; i++){

                    arg0->getMember().append(arg1->getMember());
                }
        },
        {});

        //
        registerFunction("__mulAssign__", {STRING::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__mulAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_ARG(STRING, 0); GET_ARG(INT, 1);

                //
                std::string sequence = arg0->getMember();
                int multiplier = arg1->getMember();

                // schreiben in returns
                arg0->getMember() = std::string();

                //
                arg0->getMember().reserve(sequence.size() * multiplier);
                for (int i = 0; i < multiplier; i++){

                    arg0->getMember().append(sequence);
                }
        },
        {});

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

        // Operatoren
        registerMemberFunction(STRING::typeIndex, "writeToFile", {STRING::typeIndex},
            [__functionLabel__ = "writeToFile", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;
            
                GET_MEMBER(STRING);
                GET_ARG(STRING, 0);

                //
                std::ofstream outputFile;
                outputFile.open(arg0->getMember(), std::ios::trunc);

                //
                outputFile << mb->getMember();
                outputFile.close();
        },
        {});

        return true;
    }
};