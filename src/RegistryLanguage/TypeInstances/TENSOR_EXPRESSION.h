#pragma once

#include "InstanceIncludes.h"
#include "BOOL.h"
#include "INT.h"
#include "DOUBLE.h"
#include "STRING.h"

struct TensorExpression{

    //
    bool IsOperation = false;

    // Inhalt
    std::string label = NULLSTR;
    int tensorOrder = -1;

    TensorExpression() = default;
    
    TensorExpression(const std::string& labelIn, int tensorOrderIn) : label(labelIn), tensorOrder(tensorOrderIn){

    }

    bool isValid(){
        return label != NULLSTR && tensorOrder >= 0;
    }

    friend std::ostream& operator<<(std::ostream& os, const TensorExpression& expr);
};

namespace types{

    class TENSOR_EXPRESSION : public INativeObject<TENSOR_EXPRESSION, TensorExpression>{

    public:

        static int setUpClass();

        TENSOR_EXPRESSION() = default;
        TENSOR_EXPRESSION(TensorExpression* Ptr) : INativeObject(Ptr){}

        // virtual ist redundant, die print bleibt überscheibbar
        void print() const override{

            LOG << getMember();
        }
    };
};