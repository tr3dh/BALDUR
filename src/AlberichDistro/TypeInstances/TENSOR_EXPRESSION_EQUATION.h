#pragma once

#include "TENSOR_EXPRESSION.h"

struct TensorExpressionEquation{

    TensorExpression lhs, rhs;

    TensorExpressionEquation() = default;
    TensorExpressionEquation(const TensorExpression& lhsIn, const TensorExpression& rhsIn){
        
        lhs = lhsIn;
        rhs = rhsIn;
    }

    void rearangeFor(const TensorExpression& expr);
    bool rearangeOnceFor(const TensorExpression& expr);

    friend std::ostream& operator<<(std::ostream& os, const TensorExpressionEquation& eq);
};

namespace types{

    class TENSOR_EXPRESSION_EQUATION : public INativeObject<TENSOR_EXPRESSION_EQUATION, TensorExpressionEquation>{

        public:

            static bool setUpClass();

            TENSOR_EXPRESSION_EQUATION() = default;
            TENSOR_EXPRESSION_EQUATION(TensorExpressionEquation* Ptr) : INativeObject(Ptr){}

            // virtual ist redundant, die print bleibt überscheibbar
            void print() const override;
    };
}