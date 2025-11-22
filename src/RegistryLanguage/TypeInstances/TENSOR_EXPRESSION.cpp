#include "TENSOR_EXPRESSION.h"

std::map<TensorExpressionOperator, std::string> TensorExpressionOperatorStrings = {

    {TensorExpressionOperator::None, "INV_OPS"},

    {TensorExpressionOperator::Addition, "+"},
    {TensorExpressionOperator::Subtraction, "-"},
    {TensorExpressionOperator::Multiplication, "*"},

    {TensorExpressionOperator::DotProduct, "."},
    {TensorExpressionOperator::CrossProduct, "x"},
    {TensorExpressionOperator::DyadicProduct, "(x)"},
    {TensorExpressionOperator::CrossingDoubleContraction, ":"},
    {TensorExpressionOperator::MirroringDoubleContraction, ".."},
    
    {TensorExpressionOperator::Inversion, "^-1"},
    {TensorExpressionOperator::Transposition, "^t"},
    {TensorExpressionOperator::Trace, "^//"},
};

std::map<TensorExpressionOperator, void (TensorExpression::*)(const TensorExpression&)> operatorMemberFunctions = {

    {TensorExpressionOperator::DotProduct, &TensorExpression::dotProductAssign},
    {TensorExpressionOperator::CrossProduct, &TensorExpression::crossProductAssign},
    {TensorExpressionOperator::DyadicProduct, &TensorExpression::dyadProductAssign},
    {TensorExpressionOperator::CrossingDoubleContraction, &TensorExpression::crossingDoubleContractionAssign},
    {TensorExpressionOperator::MirroringDoubleContraction, &TensorExpression::mirroringDoubleContractionAssign},
};

std::map<std::pair<TensorExpression, TensorExpression>, TensorExpression> tensorExpressionDiffs = {};
std::map<std::pair<TensorExpression, TensorExpression>, TensorExpression> tensorExpressionDiffTemplates = {};

void moveSelfIntoFirstChild(TensorExpression& node){

    TensorExpression tmp = std::move(node);

    node = TensorExpression();
    node.children.emplace_back(std::move(tmp));
}

bool operator<(const TensorExpression& lhs, const TensorExpression& rhs)
{
    if (lhs.Relation != rhs.Relation)
        return lhs.Relation < rhs.Relation;

    if (lhs.Operator != rhs.Operator)
        return lhs.Operator < rhs.Operator;

    if (lhs.label != rhs.label)
        return lhs.label < rhs.label;

    if (lhs.tensorOrder != rhs.tensorOrder)
        return lhs.tensorOrder < rhs.tensorOrder;

    //
    const auto& a = lhs.children;
    const auto& b = rhs.children;

    if (a.size() != b.size())
        return a.size() < b.size();

    for (std::size_t i = 0; i < a.size(); ++i) {
        if (a[i] == b[i]) continue;
        return a[i] < b[i];
    }

    //
    return false;
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

std::vector<std::string> TensorExpression::getRawLabels(){

    std::vector<std::string> labels, res;

    if(Relation == TkType::Argument){
        
        res.emplace_back(label);
    }
    else{
        for(auto& child : children){

            labels = child.getRawLabels();
            res.insert(res.end(), labels.begin(), labels.end());
        }
    }

    return res;
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

bool TensorExpression::operator==(const TensorExpression& other) const {

    bool equal = true;

    equal &= Relation == other.Relation;
    equal &= Operator == other.Operator;
    equal &= label == other.label;
    equal &= tensorOrder == other.tensorOrder;
    equal &= children.size() == other.children.size();

    if(!equal){ return equal; }

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
    if(tensorExpressionDiffs.contains(std::make_pair(*this, other))){

        *this = tensorExpressionDiffs[std::make_pair(*this, other)];
    }
    else if(*this == other){

        *this = TensorExpression("Identity", this->tensorOrder);
    }
    else if((Relation == TkType::Argument && other.Relation == TkType::Argument) ||
            Relation == TkType::Operator && Operator == TensorExpressionOperator::Diff){

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
    else if(Relation == TkType::Operator){
     
        if(Operator == TensorExpressionOperator::Addition){

            std::vector tmpChilds = std::move(children);

            tmpChilds[0].diffAssign(other);
            *this = std::move(tmpChilds[0]);

            for(size_t i = 1; i < tmpChilds.size(); i++){

                tmpChilds[i].diffAssign(other);
                addAssign(std::move(tmpChilds[i]));
            }
        }
        else if(Operator == TensorExpressionOperator::Subtraction){

            std::vector tmpChilds = std::move(children);

            tmpChilds[0].diffAssign(other);
            *this = std::move(tmpChilds[0]);

            for(size_t i = 1; i < tmpChilds.size(); i++){

                tmpChilds[i].diffAssign(other);
                subAssign(std::move(tmpChilds[i]));
            }
        }
        // aus diff(dotProduct(a, b, c, ...), x) wird
        // >> sum(dotProduct(diff(a,x), b, c), dotProduct(a, diff(b, x), c), ...)
        //  || Operator == TensorExpressionOperator::DyadicProduct ||
        //         Operator == TensorExpressionOperator::CrossProduct || Operator == TensorExpressionOperator::MirroringDoubleContraction ||
        //         Operator == TensorExpressionOperator::CrossingDoubleContraction
        else if(operatorMemberFunctions.contains(Operator)){

            TensorExpression self = *this;
            std::vector<TensorExpression> tmpChilds = self.children;

            TensorExpression result;
            bool firstTerm = true;

            for (size_t i = 0; i < tmpChilds.size(); i++) {

                // Start mit dem ersten Kind
                TensorExpression term = tmpChilds[0];

                if (i == 0) {
                    term.diffAssign(other);
                }

                // Die restlichen Kinder anhängen
                for (size_t j = 1; j < tmpChilds.size(); j++) {

                    if (j == i) {
                        TensorExpression tmp = tmpChilds[j];
                        tmp.diffAssign(other);
                        (term.*operatorMemberFunctions[Operator])(tmp);
                    } else {
                        (term.*operatorMemberFunctions[Operator])(tmpChilds[j]);
                    }
                }

                if (firstTerm) {
                    result = std::move(term);
                    firstTerm = false;
                } else {
                    result.addAssign(term);
                }
            }

            *this = std::move(result);
        }
    }
}

//
void TensorExpression::convertToTemplate(){

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
        res += depth > 0 ? "[" + std::to_string(tensorOrder) + "]" : "";     
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

        // Operatoren
        registerFunction("__equal__", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__equal__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
               
                //
                returns.emplace_back();
                returns[returns.size() - 1].constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                BOOL* ret0 = static_cast<BOOL*>(returns[returns.size()-1].getVariableRef().getData());

                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() == arg1->getMember();
        },
        {BOOL::typeIndex});

        //
        registerFunction("setDiff", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "setDiff", __numArgs__ = 3](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1); GET_ARG(TENSOR_EXPRESSION, 2);

                TensorExpression& member0 = arg0->getMember(), member1 = arg1->getMember(), member2 = arg2->getMember();

                RETURNING_ASSERT(tensorExpressionDiffs.try_emplace(std::make_pair(member0, member1), member2).second,
                                 "Differential für gegebenes Tensorpaar bereits gesetzt",);
        },
        {});

        //
        registerFunction("setDiffTemplate", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "setDiffTemplate", __numArgs__ = 3](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1); GET_ARG(TENSOR_EXPRESSION, 2);

                TensorExpression& member0 = arg0->getMember(), member1 = arg1->getMember(), member2 = arg2->getMember();
                member0.convertToTemplate();
                member1.convertToTemplate();
                member2.convertToTemplate();

                RETURNING_ASSERT(tensorExpressionDiffTemplates.try_emplace(std::make_pair(member0, member1), member2).second,
                                 "Differential für gegebenes Tensortemplatepaar bereits gesetzt",);
        },
        {});

        return true;
    }
}