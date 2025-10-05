#include "BOOL.h"

namespace types{

    bool BOOL::setUpClass(){

        // register in TypeRegister
        if(!init("bool", [](){ return new BOOL(); })){ return false; }

        // Registrierung von Keyword zur rvalue Konstruktion von bools
        registerKeyword("true", new BOOL(true));
        registerKeyword("false", new BOOL(false));

        registerFunction("equals", {BOOL::typeIndex, BOOL::typeIndex},
            [__functionLabel__ = "equals", __numArgs__ = 2](std::vector<std::shared_ptr<EvalResult>>& returns, const std::vector<std::shared_ptr<EvalResult>>& inputs, const std::vector<TypeIndex>& functionReturnTypes, std::shared_ptr<EvalResult> member){

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