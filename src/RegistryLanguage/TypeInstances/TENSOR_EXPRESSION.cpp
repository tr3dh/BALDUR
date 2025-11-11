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
    
    {TensorExpressionOperator::Inversion, "^-1"},
    {TensorExpressionOperator::Transposition, "^t"},
    {TensorExpressionOperator::Trace, "^//"},
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
    
    //
    static TensorExpressionOperator operation = TensorExpressionOperator::DotProduct;

    // ASSERTS
    RETURNING_ASSERT(tensorOrder > 0 && other.tensorOrder > 0, "Tensoren mit Stufe kleiner 1 and Skalarprodukt beteiligt",);

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
    tensorOrder = tensorOrder + (copySelf ? children.back() : other).tensorOrder - 2;
}

void TensorExpression::crossProductAssign(const TensorExpression& other){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::CrossProduct;

    // ASSERTS
    RETURNING_ASSERT(tensorOrder == 1 && other.tensorOrder == 1, "Tensoren für Vektorprodukt müssen Vektoren sein",);

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
}

void TensorExpression::dyadProductAssign(const TensorExpression& other){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::DyadicProduct;

    // ASSERTS
    RETURNING_ASSERT(tensorOrder > 1 && other.tensorOrder > 1, "Tensoren mit Stufe kleiner 2 ...",);

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
    tensorOrder = tensorOrder + (copySelf ? children.back() : other).tensorOrder;
}

void TensorExpression::mirroringDoubleContractionAssign(const TensorExpression& other){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::MirroringDoubleContraction;

    // ASSERTS
    RETURNING_ASSERT(tensorOrder > 0 && other.tensorOrder > 0, "Tensoren mit Stufe kleiner 1 and ... beteiligt",);

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
    tensorOrder = tensorOrder + (copySelf ? children.back() : other).tensorOrder - 4;
}

void TensorExpression::crossingDoubleContractionAssign(const TensorExpression& other){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::CrossingDoubleContraction;

    // ASSERTS
    RETURNING_ASSERT(tensorOrder > 0 && other.tensorOrder > 0, "Tensoren mit Stufe kleiner 1 and ... beteiligt",);

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
    tensorOrder = tensorOrder + (copySelf ? children.back() : other).tensorOrder - 4;
}

void TensorExpression::transposeAssign(){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::Transposition;

    //
    moveSelfIntoFirstChild();

    // node erneut Aufsetzen
    Relation = TkType::Operator;
    Operator = operation;
    tensorOrder = children.begin()->tensorOrder;
}

void TensorExpression::inverseAssign(){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::Inversion;

    //
    moveSelfIntoFirstChild();

    // node erneut Aufsetzen
    Relation = TkType::Operator;
    Operator = operation;
    tensorOrder = children.begin()->tensorOrder;
}

void TensorExpression::traceAssign(){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::Trace;

    //
    moveSelfIntoFirstChild();

    // node erneut Aufsetzen
    Relation = TkType::Operator;
    Operator = operation;
    tensorOrder = 0;
}

void TensorExpression::traceAssign(int contractIndices){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::Trace;

    //
    RETURNING_ASSERT(tensorOrder >= contractIndices, "Tensor Dimension nicht groß genug für Trace mit angegebener Kontraktion",);

    //
    moveSelfIntoFirstChild();

    // node erneut Aufsetzen
    Relation = TkType::Operator;
    Operator = operation;
    tensorOrder = children.back().tensorOrder - (contractIndices + 1);
}

bool TensorExpression::operator==(const TensorExpression& other){

    bool equal = true;

    equal &= Relation == other.Relation;
    equal &= Operator == other.Operator;
    equal &= label == other.label;
    equal &= tensorOrder == other.tensorOrder;

    equal &= children.size() == other.children.size();

    if(!equal){

        return equal;
    }

    for(size_t childIdx = 0; childIdx < children.size(); childIdx++){
        
        equal &= children[childIdx] == other.children[childIdx]; 
    }

    return equal;
}

void TensorExpression::diffAssign(const TensorExpression& other){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::Diff;

    //
    bool copySelf = false;

    //
    if(*this == other){

        *this = TensorExpression("Identity", this->tensorOrder);
    }
    else if(Relation == TkType::Argument && other.Relation == TkType::Argument){

        // mov
        if(this == &other){ copySelf = true; }
        moveSelfIntoFirstChild();

        // node erneut Aufsetzen
        Relation = TkType::Operator;
        Operator = operation;

        //
        children.emplace_back(copySelf ? children.back() : other);

        //
        tensorOrder = children.begin()->tensorOrder + children.back().tensorOrder;
    }
}

//
std::string TensorExpression::toString(size_t depth) const{

    //
    std::string res;

    res += depth == 0 ? "TensorExpression[" + std::to_string(tensorOrder) + "] = " : "";

    // Argument node
    if(Relation == TkType::Argument && children.size() == 0){

        res += label + "[" + std::to_string(tensorOrder) + "]";
    }
    // Container
    else if(Relation == TkType::Operator && children.size() == 1){

        res += "(" + children.begin()->toString(depth+1) + ")" + TensorExpressionOperatorStrings[Operator] + " ";
        res += "[" + std::to_string(tensorOrder) + "]";
    }
    // durch Operator verknüpfte Child nodes
    else if(Relation == TkType::Operator && Operator != TensorExpressionOperator::None && children.size() > 1 &&
            TensorExpressionOperatorStrings.contains(Operator)){

        //
        // res += "(";
        res += depth > 0 ? "(" : "";
        for(size_t childIdx = 0; childIdx < children.size(); childIdx++){

            // print der Verknüfpung über Operator 
            if(childIdx > 0){ res += " " + TensorExpressionOperatorStrings[Operator] + " "; }

            // print der node
            res += children[childIdx].toString(depth+1);
        }
        res += depth > 0 ? ")" : " ";
        // res += ")";
        res += depth > 0 ? "[" + std::to_string(tensorOrder) + "]" : "";
    }
    // durch Operator verknüpfte Child nodes
    else if(Relation == TkType::Operator && Operator == TensorExpressionOperator::Diff && children.size() == 2){

        res += "diff(" + children[0].toString(depth+1) + " / " + children[1].toString(depth+1) + ")";        
    }
    else{

        res += "Invalid Expr";
    }

    return res;
}

// Für Tree
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

    void TENSOR_EXPRESSION::print() const {

        // Implementation
        LOG << member->toString();
    }

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

        // Operator Überladung
        registerFunction("__addAssign__", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__addAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                TensorExpression& member0 = arg0->getMember();
                TensorExpression& member1 = arg1->getMember();

                member0.addAssign(member1);
        },
        {});

        registerFunction("__subAssign__", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__subAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                TensorExpression& member0 = arg0->getMember();
                TensorExpression& member1 = arg1->getMember();

                member0.subAssign(member1);
        },
        {});

        registerFunction("__mulAssign__", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__mulAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                TensorExpression& member0 = arg0->getMember();
                TensorExpression& member1 = arg1->getMember();

                member0.mulAssign(member1);
        },
        {});

        registerFunction("__dotProductAssign__", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__dotProductAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                TensorExpression& member0 = arg0->getMember();
                TensorExpression& member1 = arg1->getMember();

                member0.dotProductAssign(member1);
        },
        {});

        registerFunction("__crossProductAssign__", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__crossProductAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                TensorExpression& member0 = arg0->getMember();
                TensorExpression& member1 = arg1->getMember();

                member0.crossProductAssign(member1);
        },
        {});

        registerFunction("__dyadProductAssign__", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__dyadProductAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                TensorExpression& member0 = arg0->getMember();
                TensorExpression& member1 = arg1->getMember();

                member0.dyadProductAssign(member1);
        },
        {});

        registerFunction("__mirroringDoubleContractionAssign__", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__mirroringDoubleContractionAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                TensorExpression& member0 = arg0->getMember();
                TensorExpression& member1 = arg1->getMember();

                member0.mirroringDoubleContractionAssign(member1);
        },
        {});

        registerFunction("__crossingDoubleContractionAssign__", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__crossingDoubleContractionAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                TensorExpression& member0 = arg0->getMember();
                TensorExpression& member1 = arg1->getMember();

                member0.crossingDoubleContractionAssign(member1);
        },
        {});

        //
        registerFunction("__inverseAssign__", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__inverseAssign__", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                if(inputs[0]->isLValue()){ returns[0].cloneIntoRValue(inputs[0]->getVariableRef()); }
                else{ returns[0].moveIntoRValue(inputs[0]->getVariableRef()); }

                GET_RETURN(TENSOR_EXPRESSION, 0);
                ret0->getMember().inverseAssign();
        },
        {TENSOR_EXPRESSION::typeIndex});

        //
        registerFunction("__transposeAssign__", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__transposeAssign__", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                if(inputs[0]->isLValue()){ returns[0].cloneIntoRValue(inputs[0]->getVariableRef()); }
                else{ returns[0].moveIntoRValue(inputs[0]->getVariableRef()); }

                GET_RETURN(TENSOR_EXPRESSION, 0);
                ret0->getMember().transposeAssign();
        },
        {TENSOR_EXPRESSION::typeIndex});

        //
        registerFunction("__traceAssign__", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__traceAssign__", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                if(inputs[0]->isLValue()){ returns[0].cloneIntoRValue(inputs[0]->getVariableRef()); }
                else{ returns[0].moveIntoRValue(inputs[0]->getVariableRef()); }

                GET_RETURN(TENSOR_EXPRESSION, 0);
                ret0->getMember().traceAssign();
        },
        {TENSOR_EXPRESSION::typeIndex});

        registerFunction("__traceAssign__", {TENSOR_EXPRESSION::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__traceAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(INT, 1);
                arg0->getMember().traceAssign(arg1->getMember());
        },
        {});

        //
        registerFunction("__inverseInplaceAssign__", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__inverseInplaceAssign__", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0);

                TensorExpression& member0 = arg0->getMember();
                member0.inverseAssign();
        },
        {});

        //
        registerFunction("__transposeInplaceAssign__", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__transposeInplaceAssign__", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0);

                TensorExpression& member0 = arg0->getMember();
                member0.transposeAssign();
        },
        {});

        //
        registerFunction("__traceInplaceAssign__", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__traceInplaceAssign__", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0);

                TensorExpression& member0 = arg0->getMember();
                member0.traceAssign();
        },
        {});

        //
        registerFunction("diff", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "diff", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_RETURN(TENSOR_EXPRESSION, 0);
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                arg0->getMember().diffAssign(arg1->getMember());
                ret0->getMember() = arg0->getMember();
        },
        {TENSOR_EXPRESSION::typeIndex});

        //
        registerFunction("__diffAssign__", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__diffAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                TensorExpression& member0 = arg0->getMember();
                TensorExpression& member1 = arg1->getMember();

                member0.diffAssign(member1);
        },
        {});

        return true;
    }
}