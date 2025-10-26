#pragma once

#include "../../SemanticAnalysis/AST.h"

#include "InstanceIncludes.h"
#include "BOOL.h"
#include "INT.h"
#include "DOUBLE.h"
#include "STRING.h"

typedef size_t NotationIndex;

enum class IndexNotationOperator{

    Multiplication,
};

struct TensorExpression{

    //
    static NotationIndex NotationIndexCounter;

    // Inhalt
    std::string label = NULLSTR;
    int tensorOrder = -1;
    std::vector<NotationIndex> notatedIndices;

    //
    TkType Relation;
    IndexNotationOperator Operator;
    std::vector<TensorExpression> children;

    TensorExpression() = default;
    
    // Konstruktion einer Arg node
    TensorExpression(const std::string& labelIn, int tensorOrderIn) : label(labelIn), tensorOrder(tensorOrderIn){

        Relation = TkType::Argument;

        notatedIndices.reserve(tensorOrder);

        for(int i = 0; i < tensorOrder; i++){

            //
            notatedIndices.emplace_back(NotationIndexCounter++);
        }
    }

    bool isValid(){
        return label != NULLSTR && tensorOrder >= 0;
    }

    friend std::ostream& operator<<(std::ostream& os, const TensorExpression& expr);
};

void moveSelfIntoFirstChild(TensorExpression& node);

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