#include "TENSOR_EXPRESSION.h"

std::map<TensorExpressionOperator, std::string> TensorExpressionOperatorStrings = {

    {TensorExpressionOperator::None, "INV_OPS"},

    {TensorExpressionOperator::Addition, "+"},
    {TensorExpressionOperator::Subtraction, "-"},
    {TensorExpressionOperator::Multiplication, "*"},

    {TensorExpressionOperator::DotProduct, "."},
    {TensorExpressionOperator::CrossProduct, "x"},
    {TensorExpressionOperator::DyadicProduct, "(x)"},
    {TensorExpressionOperator::CrossingDoubleContraction, ".."},
    {TensorExpressionOperator::MirroringDoubleContraction, ":"},
    
    {TensorExpressionOperator::Inversion, "^t"},
    {TensorExpressionOperator::Transposition, "^-1"},
};

void moveSelfIntoFirstChild(TensorExpression& node){

    TensorExpression tmp = std::move(node);

    node = TensorExpression();
    node.children.emplace_back(std::move(tmp));
}

// Default Konstruktion
TensorExpression::TensorExpression() = default;

// Konstruktion einer Arg node
TensorExpression::TensorExpression(const std::string& labelIn, int tensorOrderIn) : label(labelIn), tensorOrder(tensorOrderIn){

    Relation = TkType::Argument;
}

void TensorExpression::moveSelfIntoFirstChild(){

    TensorExpression tmp = std::move(*this);

    *this = TensorExpression();
    children.emplace_back(std::move(tmp));
}

// Operatoren
void TensorExpression::addAssign(const TensorExpression& other){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::Addition;

    // ASSERTS
    RETURNING_ASSERT(tensorOrder == other.tensorOrder, "Addition von Tensoren unterschiedlicher Stufe versucht",);

    //
    bool copySelf = false;

    //
    if(Relation != TkType::Operator || Operator != operation){

        // mov
        if(this == &other){ copySelf = true; }
        moveSelfIntoFirstChild();

        // node erneut Aufsetzen
        Relation = TkType::Operator;
        Operator = operation;
        tensorOrder = children.begin()->tensorOrder;
    }

    //
    children.emplace_back(copySelf ? children.back() : other);

    // unwrap (nur für assoziative Operatoren)
    if(children.back().Relation == TkType::Operator && children.back().Operator == operation){

        // sichere Kopie
        std::vector<TensorExpression> tempChildren = children.back().children;
        children.pop_back();
        children.insert(children.end(), std::make_move_iterator(tempChildren.begin()), std::make_move_iterator(tempChildren.end()));
    }

    // Anpassen TensorOrder
    // hier nicht nötig aufgrund von Addition
    // ...
}

//
void TensorExpression::subAssign(const TensorExpression& other){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::Subtraction;

    // ASSERTS
    RETURNING_ASSERT(tensorOrder == other.tensorOrder, "Addition von Tensoren unterschiedlicher Stufe versucht",);

    //
    bool copySelf = false;

    //
    if(Relation != TkType::Operator || Operator != operation){

        // mov
        if(this == &other){ copySelf = true; }
        moveSelfIntoFirstChild();

        // node erneut Aufsetzen
        Relation = TkType::Operator;
        Operator = operation;
        tensorOrder = children.begin()->tensorOrder;
    }

    //
    children.emplace_back(copySelf ? children.back() : other);

    // Anpassen TensorOrder
    // hier nicht nötig aufgrund von Addition
    // ...
}

void TensorExpression::mulAssign(const TensorExpression& other){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::Multiplication;

    // ASSERTS
    RETURNING_ASSERT(tensorOrder == 0 || other.tensorOrder == 0, "Skalar Multiplikation ohne Skalar versucht",);

    //
    bool copySelf = false;

    //
    if(Relation != TkType::Operator || Operator != operation){

        // mov
        if(this == &other){ copySelf = true; }
        moveSelfIntoFirstChild();

        // node erneut Aufsetzen
        Relation = TkType::Operator;
        Operator = operation;
        tensorOrder = children.begin()->tensorOrder;
    }

    //
    children.emplace_back(copySelf ? children.back() : other);

    // unwrap (nur falls ein Operand ein Skalar ist)
    if(children.back().Relation == TkType::Operator && children.back().Operator == operation
        && (children.back().tensorOrder == 0 || tensorOrder == 0)){

        // sichere Kopie
        std::vector<TensorExpression> tempChildren = children.back().children;
        children.pop_back();
        children.insert(children.end(), std::make_move_iterator(tempChildren.begin()), std::make_move_iterator(tempChildren.end()));
    }

    // Anpassen TensorOrder
    // hier nicht nötig aufgrund von Addition
    tensorOrder = other.tensorOrder > tensorOrder ? other.tensorOrder : tensorOrder;
}

void TensorExpression::dotProductAssign(const TensorExpression& other){

}

void TensorExpression::crossProductAssign(const TensorExpression& other){}

void TensorExpression::dyadProductAssign(const TensorExpression& other){}

void TensorExpression::mirroringDoubleContractionAssign(const TensorExpression& other){}

void TensorExpression::crossingDoubleContractionAssign(const TensorExpression& other){}

void TensorExpression::transposeAssign(){}

void TensorExpression::inverseAssign(){}

//
std::string TensorExpression::toString(size_t depth) const{

    //
    std::string res;

    // Argument node
    if(Relation == TkType::Argument && children.size() == 0){

        res += label + "[" + std::to_string(tensorOrder) + "]";
    }
    // Container
    else if(Relation == TkType::Operator && children.size() == 1){

        res += "(" + children.begin()->toString(depth+1) + ") " + TensorExpressionOperatorStrings[Operator];
        res += "[" + std::to_string(tensorOrder) + "]";
    }
    // durch Operator verknüpfte Child nodes
    else if(Relation == TkType::Operator && Operator != TensorExpressionOperator::None && children.size() > 1){

        //
        res += "(";
        // res += depth > 0 ? "(" : "";
        for(size_t childIdx = 0; childIdx < children.size(); childIdx++){

            // print der Verknüfpung über Operator 
            if(childIdx > 0){ res += " " + TensorExpressionOperatorStrings[Operator] + " "; }

            // print der node
            res += children[childIdx].toString(depth+1);
        }
        // res += depth > 0 ? ")" : "";
        res += ")";
        res += "[" + std::to_string(tensorOrder) + "]";
    }
    else{

        RETURNING_ASSERT(TRIGGER_ASSERT, "Invalides TensorExpressionformat kann nicht ausgegeben werden", "");
    }

    return res;
}

std::ostream& operator<<(std::ostream& os, const TensorExpression& expr){

    if(expr.Relation == TkType::Argument){
        os << "Node : " << expr.label;
    }
    else{

        os << "Operation : " << TensorExpressionOperatorStrings[expr.Operator] << " {" << endl;
        for(const auto& child : expr.children){
            os << "|" << child << endl;
        }
        os << "}";
    }

    return os;
}

namespace types{

    int TENSOR_EXPRESSION::setUpClass(){

        // register in TypeRegister
        if(!init("tExpr", [](){ return new TENSOR_EXPRESSION(); })){ return false; }

        // Registrierung von Keyword zur rvalue Konstruktion von ints
        // ...

        // Konstruktoren
        registerFunction("tExpr", {STRING::typeIndex, INT::typeIndex},
            [__functionLabel__ = "tExpr", __numArgs__ = 2](FREG_ARGS){

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