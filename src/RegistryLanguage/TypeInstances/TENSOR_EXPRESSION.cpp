#include "TENSOR_EXPRESSION.h"

NotationIndex TensorExpression::NotationIndexCounter = 0;

std::ostream& operator<<(std::ostream& os, const TensorExpression& expr){

    os << "TensorExpr >> ";

    if(expr.Relation == TkType::Argument){

        os << "Argument '" << expr.label << "' [";

        for(const auto& idx : expr.notatedIndices){

            os << idx << ", ";
        }

        os << "]";
    }
    else if(expr.Relation == TkType::Operator){

        os << "Operation " << magic_enum::enum_name(expr.Operator) << " | childs : " << endl;

        for(const auto& child : expr.children){

            os << "| " << child << endl; 
        }
    }
    else{

        os << "NodeType '" << magic_enum::enum_name(expr.Relation) << "'";
    }

    return os;
}

void moveSelfIntoFirstChild(TensorExpression& node)
{
    if (node.children.empty()){
        node.children.emplace_back();
    }

    TensorExpression tmp = std::move(node);

    node = TensorExpression();
    node.children.push_back(std::move(tmp));
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

        // Konstruktoren
        registerFunction("__matProductAssign__", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__matProductAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                TensorExpression& member0 = arg0->getMember();
                TensorExpression& member1 = arg1->getMember();

                if(member0.Relation != TkType::Operator || member0.Operator != IndexNotationOperator::Multiplication){

                    moveSelfIntoFirstChild(arg0->getMember());

                    //
                    arg0->getMember().Relation = TkType::Operator;
                    arg0->getMember().Operator = IndexNotationOperator::Multiplication;
                }
                
                //
                member0.children.emplace_back(member1);

                //
                TensorExpression& processedMember0 = member0.children[member0.children.size() - 2];
                TensorExpression& processedMember1 = member0.children[member0.children.size() - 1];

                //
                if(processedMember0.Relation == TkType::Argument && processedMember1.Relation == TkType::Argument){

                    processedMember1.notatedIndices[0] = processedMember0.notatedIndices[processedMember0.notatedIndices.size() - 1];
                }
        },
        {});

        return true;
    }
}