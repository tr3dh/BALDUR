#include "TENSOR_EXPRESSION.h"

std::ostream& operator<<(std::ostream& os, const TensorExpression& expr){

    os << "Tensor " << expr.label << " | Dimension " << expr.tensorOrder << endl;
    return os;
}

namespace types{

    int TENSOR_EXPRESSION::setUpClass(){

        // register in TypeRegister
        if(!init("TensorExpression", [](){ return new TENSOR_EXPRESSION(); })){ return false; }

        // Registrierung von Keyword zur rvalue Konstruktion von ints
        // ...

        // Konstruktoren
        registerFunction("TensorExpression", {STRING::typeIndex, INT::typeIndex},
            [__functionLabel__ = "TensorExpression", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_RETURN(TENSOR_EXPRESSION, 0);
                GET_ARG(STRING, 0); GET_ARG(INT, 1);

                // schreiben in returns
                ret0->getMember() = TensorExpression(arg0->getMember(), arg1->getMember());
        },
        {TENSOR_EXPRESSION::typeIndex});

        return true;
    }
}