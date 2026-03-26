#include "TENSOR_EXPRESSION_EQUATION.h"

// Für Tree
std::ostream& operator<<(std::ostream& os, const TensorExpressionEquation& eq){

    //
    os << eq.lhs.toString(1) << " = " << eq.rhs.toString(1);
    return os;
}

void TensorExpressionEquation::rearangeFor(const TensorExpression& expr){

    // Ausgangsformat :
    // . lhs soll Ausdruck enthalten der isoliert wird
    // >> Operationen werden sol lange rekursiv aufgeschlüsselt und invertiert auf rhs beauftragt bis
    //    expr alleine in lhs steht
    // . lhs darf damit nur ein Vorkommen von expr beinhalten rhs keins
    // . Für andere Umformungen sind komplexere Algorithmen notwendig die momentan (noch ??) nicht implementiert sind
    
    //
    bool lhsContainsExpr = lhs.contains(expr), rhsContainsExpr = rhs.contains(expr);
    RETURNING_ASSERT(lhsContainsExpr != rhsContainsExpr, "Übergebene Gleichung entspricht nicht dem Zielformat",);

    // swap wenn expr keine dependencie der linken seite
    if(!lhsContainsExpr){ std::swap(lhs, rhs); }

    // Anzahl der Vorkommen in linker Seit muss genau 1 sein 
    RETURNING_ASSERT(lhs.countOccurences(expr), "Linke Seite der umzustellenden Gleichung enthält mehr als ein Vorkommen der zu isolierenden Größe",);

    // zur unkomplizierteren Umformung wird die gepackte Form gewählt
    if(!lhs.isWrapped()){ lhs.wrap(); }
    if(!rhs.isWrapped()){ rhs.wrap(); }

    //
    bool storedFlag = unwrapOperands;
    unwrapOperands = false;

    //
    while(rearangeOnceFor(expr)){}

    // reset der Flag auf den von Nutzer gewünschten Standard
    unwrapOperands = storedFlag;
}

bool TensorExpressionEquation::rearangeOnceFor(const TensorExpression& expr){

    // erwartet gewrapten Ausdruck >> maximal zwei childs pro Node
    // rhs ist praktisch egal es geht nur darum herauszufinden in welcher child node 
    // expr vorkommt, die andere child node mit der inversen Operation zu beaufschlagen und dann lhs durch
    // die childnode zu ersetzen in der expr vorkommt

    // folglich können nur 2, 1 oder 0 childs auftauchen
    // 0 childs -> externe node die entweder expr ist oder nicht
    // >> in jedem Fall kann die Iteration beendet werden
    // 1 child  -> Container der einen Ausdruck enthält in dem expr vorkommt oder es selber ist
    // >> Unterscheidung danach ob node expr ist
    // 2 childs -> Operation von der eine child node expr enthält
    // >> Unterscheidung danach welche node expr beinhaltet

    // Abbruch der Iteration
    if(lhs == expr){ return false; }
    else if(!lhs.children.size()){ return false; }

    //
    RETURNING_ASSERT(lhs.Relation == TkType::Operator, "", false);

    //
    if(lhs.children.size() == 1){

        //
        TensorExpressionOperator Ops = lhs.Operator;
        lhs = std::move(lhs.children.front());

        switch(Ops){

            case(TensorExpressionOperator::Inversion):
            {
                //
                rhs.inverseAssign();
                break;
            }
            case(TensorExpressionOperator::Transposition):{
                
                //
                rhs.transposeAssign();
                break;
            }
            default:{

                //
                RETURNING_ASSERT(TRIGGER_ASSERT, "Node kann nicht eindeutig nach entsprechender Größe umgeformt werden", false);

                //
                break;
            }
        }

        return true;
    }

    //
    else if(lhs.children.size() == 2){

        //
        TensorExpressionOperator Ops = lhs.Operator;

        //
        bool firstChildContainsExpr = lhs.children[0].contains(expr);
        bool secondChildContainsExpr = lhs.children[1].contains(expr);

        RETURNING_ASSERT(firstChildContainsExpr != secondChildContainsExpr, "...", false);

        int exprChildIdx = firstChildContainsExpr ? 0 : 1;
        int notExprChildIdx = exprChildIdx == 0 ? 1 : 0;
    
        switch(Ops){

            case(TensorExpressionOperator::Addition):
            {
                //
                rhs.subAssign(lhs.children[notExprChildIdx]);
                lhs = std::move(lhs.children[exprChildIdx]);

                break;
            }
            case(TensorExpressionOperator::Subtraction):
            {
                //
                if(firstChildContainsExpr){

                    // exprChildIdx = 0 | notExprChildIdx = 1 
                    rhs.addAssign(lhs.children[notExprChildIdx]);
                    lhs = std::move(lhs.children[exprChildIdx]);
                }
                else{

                    //
                    std::vector<TensorExpression> operands = std::move(lhs.children);

                    // exprChildIdx = 1 | notExprChildIdx = 0 
                    lhs = TensorExpression(-1);
                    lhs.mulAssign(operands[exprChildIdx]);
                    lhs.addAssign(operands[notExprChildIdx]);

                    //
                    return rearangeOnceFor(expr);
                }

                break;
            }
            case(TensorExpressionOperator::Multiplication):
            {
                //
                bool scalarContainsExpr = lhs.children[exprChildIdx].tensorOrder == 0;
                RETURNING_ASSERT(!scalarContainsExpr, "...", false);

                TensorExpression tmp = std::move(lhs.children[notExprChildIdx]);
                tmp.inverseAssign();
                rhs.mulAssign(tmp);

                lhs = std::move(lhs.children[exprChildIdx]);

                break;
            }
            case(TensorExpressionOperator::DotProduct):
            case(TensorExpressionOperator::MirroringDoubleContraction):
            case(TensorExpressionOperator::CrossingDoubleContraction):{
                
                TensorExpression tmp = std::move(lhs.children[notExprChildIdx]);
                tmp.inverseAssign();
                if(secondChildContainsExpr){ tmp.transposeAssign(); } 

                (rhs.*operatorMemberFunctions[lhs.Operator])(tmp);
                lhs = std::move(lhs.children[exprChildIdx]);
                
                break;
            }
            default:{

                //
                RETURNING_ASSERT(TRIGGER_ASSERT, "Node kann nicht eindeutig nach entsprechender Größe umgeformt werden", false);

                //
                break;
            }
        }

        return true;
    }

    //
    RETURNING_ASSERT(TRIGGER_ASSERT, "Unverarbeitbares Format erhalten", false);

    //
    return false;
}

namespace types{

    void TENSOR_EXPRESSION_EQUATION::print() const {

        // Implementation
        LOG << *member;
    }

    bool TENSOR_EXPRESSION_EQUATION::setUpClass(){

        // register in TypeRegister
        if(!init("tExprEq", [](){ return new TENSOR_EXPRESSION_EQUATION(); })){ return false; }

        // Registrierung von Keyword zur rvalue Konstruktion von eqs
        // ...

        // Konstruktoren
        registerFunction("tExprEq", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "tExprEq", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_RETURN(TENSOR_EXPRESSION_EQUATION, 0);
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                // schreiben in returns
                ret0->getMember() = TensorExpressionEquation(arg0->getMember(), arg1->getMember());
        },
        {TENSOR_EXPRESSION_EQUATION::typeIndex});

        // Konstruktoren
        registerMemberFunction(TENSOR_EXPRESSION_EQUATION::typeIndex, "rearangeFor", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "rearangeFor", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_MEMBER(TENSOR_EXPRESSION_EQUATION);
                GET_ARG(TENSOR_EXPRESSION, 0);

                // schreiben in returns
                mb->getMember().rearangeFor(arg0->getMember());
        },
        {});

        return true;
    }
}