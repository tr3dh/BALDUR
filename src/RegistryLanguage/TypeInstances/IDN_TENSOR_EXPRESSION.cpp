#include "TENSOR_EXPRESSION.h"

NotationIndex IndexNotatedTensorExpression::NotationIndexCounter = 0;

std::map<IndexNotationOperator, std::string> IndexNotationOperatorStrings = {

    {IndexNotationOperator::Addition, "+"},
    {IndexNotationOperator::Subtraction, "-"},
    {IndexNotationOperator::Multiplication, "*"},
};

void moveSelfIntoFirstChild(IndexNotatedTensorExpression& node)
{
    IndexNotatedTensorExpression tmp = std::move(node);

    node = IndexNotatedTensorExpression();
    node.children.emplace_back(std::move(tmp));
}

IndexNotatedTensorExpression::IndexNotatedTensorExpression() = default;
    
// Konstruktion einer Arg node
IndexNotatedTensorExpression::IndexNotatedTensorExpression(const std::string& labelIn, int tensorOrderIn) : label(labelIn), tensorOrder(tensorOrderIn){

    Relation = TkType::Argument;

    fillIndices();
}

IndexNotatedTensorExpression::IndexNotatedTensorExpression(float valueIn) : value(valueIn){

    Relation = TkType::Argument;
    tensorOrder = 0;
    isConstant = true;
}

bool IndexNotatedTensorExpression::isValid(){

    return label != NULLSTR && tensorOrder >= 0;
}

void IndexNotatedTensorExpression::fillIndices(){
    
    notatedIndices.clear();
    notatedIndices.reserve(tensorOrder);

    for(int i = 0; i < tensorOrder; i++){

        //
        notatedIndices.emplace_back(NotationIndexCounter++);
    }
}

void IndexNotatedTensorExpression::replaceIndex(NotationIndex oldIndex, NotationIndex newIndex){

    for(auto& idx : notatedIndices){

        if(idx == oldIndex){

            idx = newIndex;
        }
    }

    for(auto& child : children){

        child.replaceIndex(oldIndex, newIndex);
    }
}

void IndexNotatedTensorExpression::replaceIndices(const std::vector<NotationIndex>& oldIndices, const std::vector<NotationIndex>& newIndices){

    RETURNING_ASSERT(oldIndices.size() == newIndices.size(),
        "replaceIndices: oldIndices und newIndices müssen die gleiche Länge haben", );

    for(size_t i = 0; i < oldIndices.size(); i++){

        replaceIndex(oldIndices[i], newIndices[i]);
    }
}

std::vector<NotationIndex> IndexNotatedTensorExpression::getUniqueChildIndices() const {

    std::unordered_map<NotationIndex, int> indexCount;
    std::vector<NotationIndex> order;

    for (const auto& child : children) {
        for (auto idx : child.notatedIndices) {
            if (indexCount[idx]++ == 0) {
                order.push_back(idx);
            }
        }
    }

    std::vector<NotationIndex> uniqueIndices;
    uniqueIndices.reserve(order.size());

    for (auto idx : order) {
        if (indexCount[idx] == 1) {
            uniqueIndices.push_back(idx);
        }
    }

    return uniqueIndices;
}

//
const std::vector<NotationIndex>& IndexNotatedTensorExpression::getSortedIndices(){

    // Nur Wenn Transpose Container keinen reverse der notated Indices durchführt
    // if(Relation == TkType::Container && Operator == IndexNotationOperator::Transposition){

    //     cachedSortedIndices.assign(notatedIndices.rbegin(), notatedIndices.rend());
    //     return cachedSortedIndices;
    // }

    return notatedIndices;
}

//         if(member0.Relation != TkType::Operator || member0.Operator != IndexNotationOperator::Addition){

        //             moveSelfIntoFirstChild(arg0->getMember());

        //             //
        //             arg0->getMember().Relation = TkType::Operator;
        //             arg0->getMember().Operator = IndexNotationOperator::Addition;
        //         }
                
        //         //
        //         member0.children.emplace_back(member1);

        //         //
        //         IndexNotatedTensorExpression& processedMember0 =
        //             member0.children.size() == 2 ? member0.children[member0.children.size() - 2] : member0;
        //         IndexNotatedTensorExpression& processedMember1 = member0.children[member0.children.size() - 1];

        //         //
        //         if(processedMember0.Relation == TkType::Argument) processedMember0.fillIndices();
        //         if(processedMember1.Relation == TkType::Argument) processedMember1.fillIndices();

        //         //
        //         RETURNING_ASSERT(processedMember0.tensorOrder == processedMember1.tensorOrder,
        //             "Addition von Tensoren unterschiedlicher Stufe nicht möglich", );
                
        //         // A[i,j] + B[i,j] = C[i,j]
        //         processedMember1.replaceIndices(processedMember1.notatedIndices, processedMember0.notatedIndices);

        //         // Aufgrund von Assoziaitivität
        //         if(processedMember1.Relation == TkType::Operator &&
        //            processedMember1.Operator == IndexNotationOperator::Addition){

        //             IndexNotatedTensorExpression tmp = std::move(processedMember1);
        //             member0.children.pop_back();

        //             member0.children.insert(
        //                 member0.children.end(),
        //                 std::make_move_iterator(tmp.children.begin()),
        //                 std::make_move_iterator(tmp.children.end())
        //             );
        //         }

        //         // ab hier sind die processedMember Referenzen nicht mehr gültig

        //         // freie Indices bleiben erhalten
        //         member0.notatedIndices = member0.children.back().notatedIndices;
        //         member0.tensorOrder = member0.children.back().tensorOrder;
        // },
        // {});

void IndexNotatedTensorExpression::moveSelfIntoFirstChild(){

    IndexNotatedTensorExpression tmp = std::move(*this);

    *this = IndexNotatedTensorExpression();
    children.emplace_back(std::move(tmp));
}

// Operatoren
void IndexNotatedTensorExpression::addAssign(const IndexNotatedTensorExpression& other){

    //
    static IndexNotationOperator scalarOperation = IndexNotationOperator::Addition;
    static TensorExpressionOperator operation = TensorExpressionOperator::Addition;

    // ASSERTS
    RETURNING_ASSERT(tensorOrder == other.tensorOrder, "Addition von Tensoren unterschiedlicher Stufe versucht",);

    //
    bool copySelf = false;

    //
    if(Relation == TkType::Argument){ fillIndices(); }

    //
    if(Relation != TkType::Operator || Operator != scalarOperation){

        // mov
        if(this == &other){ copySelf = true; }
        moveSelfIntoFirstChild();

        // node erneut Aufsetzen
        Relation = TkType::Operator;
        Operator = scalarOperation;
        notatedIndices = children.begin()->notatedIndices;
        tensorOrder = children.begin()->tensorOrder;
    }

    //
    children.emplace_back(copySelf ? children.back() : other);
    if(children.back().Relation == TkType::Argument){ children.back().fillIndices(); }

    if(children.back().Relation == TkType::Argument){ children.back().fillIndices(); }

    //
    const std::vector<NotationIndex>& operand0Indices = children.size() > 2 ? this->getSortedIndices() : children.begin()->getSortedIndices();
    const std::vector<NotationIndex>& operand1Indices = children.back().getSortedIndices();

    // Eigentliche Logik

    // Addition >> alle Indices des erster Operanden werden in die zweiten geschrieben
    children.back().replaceIndices(operand1Indices, operand0Indices);

    // unwrap (nur für assoziative Operatoren)
    if(children.back().Relation == TkType::Operator && children.back().Operator == scalarOperation){

        // sichere Kopie
        std::vector<IndexNotatedTensorExpression> tempChildren = children.back().children;
        children.pop_back();
        children.insert(children.end(), std::make_move_iterator(tempChildren.begin()), std::make_move_iterator(tempChildren.end()));
    }

    // Tensor Order muss nicht angepasst werde
}

void IndexNotatedTensorExpression::subAssign(const IndexNotatedTensorExpression& other){

    //
    static IndexNotationOperator scalarOperation = IndexNotationOperator::Subtraction;
    static TensorExpressionOperator operation = TensorExpressionOperator::Subtraction;

    // ASSERTS
    RETURNING_ASSERT(tensorOrder == other.tensorOrder, "Addition von Tensoren unterschiedlicher Stufe versucht",);

    //
    bool copySelf = false;

    //
    if(Relation == TkType::Argument){ fillIndices(); }

    //
    if(Relation != TkType::Operator || Operator != scalarOperation){

        // mov
        if(this == &other){ copySelf = true; }
        moveSelfIntoFirstChild();

        // node erneut Aufsetzen
        Relation = TkType::Operator;
        Operator = scalarOperation;
        notatedIndices = children.begin()->notatedIndices;
        tensorOrder = children.begin()->tensorOrder;
    }

    //
    children.emplace_back(copySelf ? children.back() : other);
    if(children.back().Relation == TkType::Argument){ children.back().fillIndices(); }
    
    //
    const std::vector<NotationIndex>& operand0Indices = children.size() > 2 ? this->getSortedIndices() : children.begin()->getSortedIndices();
    const std::vector<NotationIndex>& operand1Indices = children.back().getSortedIndices();

    // Eigentliche Logik

    // Addition >> alle Indices des erster Operanden werden in die zweiten geschrieben
    children.back().replaceIndices(operand1Indices, operand0Indices);

    // Tensor Order muss nicht angepasst werde
}

void IndexNotatedTensorExpression::mulAssign(const IndexNotatedTensorExpression& other){

    //
    static IndexNotationOperator scalarOperation = IndexNotationOperator::Multiplication;
    static TensorExpressionOperator operation = TensorExpressionOperator::Multiplication;

    // ASSERTS
    RETURNING_ASSERT(tensorOrder == 0 || other.tensorOrder == 0, "Skalar Multiplikation braucht beteiligtes Skalar",);

    //
    bool copySelf = false;

    //
    if(Relation == TkType::Argument){ fillIndices(); }

    //
    if(Relation != TkType::Operator || Operator != scalarOperation){

        // mov
        if(this == &other){ copySelf = true; }
        moveSelfIntoFirstChild();

        // node erneut Aufsetzen
        Relation = TkType::Operator;
        Operator = scalarOperation;
        notatedIndices = children.begin()->notatedIndices;
        tensorOrder = children.begin()->tensorOrder;
    }

    //
    children.emplace_back(copySelf ? children.back() : other);
    if(children.back().Relation == TkType::Argument){ children.back().fillIndices(); }

    //
    const std::vector<NotationIndex>& operand0Indices = children.size() > 2 ? this->getSortedIndices() : children.begin()->getSortedIndices();
    const std::vector<NotationIndex>& operand1Indices = children.back().getSortedIndices();

    // Eigentliche Logik

    // Addition >> alle Indices des erster Operanden werden in die zweiten geschrieben

    // unwrap (nur für assoziative Operatoren)
    if(children.back().Relation == TkType::Operator && children.back().Operator == scalarOperation){

        // sichere Kopie
        std::vector<IndexNotatedTensorExpression> tempChildren = children.back().children;
        children.pop_back();
        children.insert(children.end(), std::make_move_iterator(tempChildren.begin()), std::make_move_iterator(tempChildren.end()));
    }

    // Tensor Order muss nicht angepasst werde
    notatedIndices = getUniqueChildIndices();
    tensorOrder = notatedIndices.size();
}

void IndexNotatedTensorExpression::dotProductAssign(const IndexNotatedTensorExpression& other){

    //
    static IndexNotationOperator scalarOperation = IndexNotationOperator::Multiplication;
    static TensorExpressionOperator operation = TensorExpressionOperator::DotProduct;

    // ASSERTS
    RETURNING_ASSERT(tensorOrder > 0 && other.tensorOrder > 0,
                    "Skalarproduktbildung mit skalaren Operanden funktioniert nicht",);

    //
    bool copySelf = false;

    //
    if(Relation == TkType::Argument){ fillIndices(); }

    //
    if(Relation != TkType::Operator || Operator != scalarOperation){

        // mov
        if(this == &other){ copySelf = true; }
        moveSelfIntoFirstChild();

        // node erneut Aufsetzen
        Relation = TkType::Operator;
        Operator = scalarOperation;
        notatedIndices = children.begin()->notatedIndices;
        tensorOrder = children.begin()->tensorOrder;
    }

    //
    children.emplace_back(copySelf ? children.back() : other);
    if(children.back().Relation == TkType::Argument){ children.back().fillIndices(); }

    //
    const std::vector<NotationIndex>& operand0Indices = children.size() > 2 ? this->getSortedIndices() : children.begin()->getSortedIndices();
    const std::vector<NotationIndex>& operand1Indices = children.back().getSortedIndices();

    // Eigentliche Logik

    // Addition >> alle Indices des erster Operanden werden in die zweiten geschrieben
    children.back().replaceIndex(operand1Indices[0], operand0Indices.back());

    // unwrap (nur für assoziative Operatoren)
    if(children.back().Relation == TkType::Operator && children.back().Operator == scalarOperation){

        // sichere Kopie
        std::vector<IndexNotatedTensorExpression> tempChildren = children.back().children;
        children.pop_back();
        children.insert(children.end(), std::make_move_iterator(tempChildren.begin()), std::make_move_iterator(tempChildren.end()));
    }

    // Tensor Order muss nicht angepasst werde
    notatedIndices = getUniqueChildIndices();
    tensorOrder = notatedIndices.size();
}

void IndexNotatedTensorExpression::crossProductAssign(const IndexNotatedTensorExpression& other){

    //
    static IndexNotationOperator scalarOperation = IndexNotationOperator::Multiplication;
    static TensorExpressionOperator operation = TensorExpressionOperator::CrossProduct;

    // ASSERTS
    RETURNING_ASSERT(tensorOrder == 1 && other.tensorOrder == 1,
                    "Kreuzproduktbildung mit nicht vektoriellen Operanden funktioniert nicht",);

    //
    bool copySelf = false;

    //
    if(Relation == TkType::Argument){ fillIndices(); }

    //
    if(Relation != TkType::Operator || Operator != scalarOperation){

        // mov
        if(this == &other){ copySelf = true; }
        moveSelfIntoFirstChild();

        // node erneut Aufsetzen
        Relation = TkType::Operator;
        Operator = scalarOperation;
        notatedIndices = children.begin()->notatedIndices;
        tensorOrder = children.begin()->tensorOrder;
    }

    //
    children.emplace_back(copySelf ? children.back() : other);
    if(children.back().Relation == TkType::Argument){ children.back().fillIndices(); }

    //
    const std::vector<NotationIndex>& operand0Indices = children.size() > 2 ? this->getSortedIndices() : children.begin()->getSortedIndices();
    const std::vector<NotationIndex>& operand1Indices = children.back().getSortedIndices();

    // Eigentliche Logik

    // Addition >> alle Indices des erster Operanden werden in die zweiten geschrieben
    
    // unwrap (nur für assoziative Operatoren)
    if(children.back().Relation == TkType::Operator && children.back().Operator == scalarOperation){

        // sichere Kopie
        std::vector<IndexNotatedTensorExpression> tempChildren = children.back().children;
        children.pop_back();
        children.insert(children.end(), std::make_move_iterator(tempChildren.begin()), std::make_move_iterator(tempChildren.end()));
    }

    //
    IndexNotatedTensorExpression civitaDelta("eps", 3);
    civitaDelta.notatedIndices = {IndexNotatedTensorExpression::NotationIndexCounter++, operand0Indices.back(), operand1Indices.back()};
    children.emplace_back(std::move(civitaDelta));

    // Tensor Order muss nicht angepasst werde
    notatedIndices = getUniqueChildIndices();
    tensorOrder = notatedIndices.size();
}

void IndexNotatedTensorExpression::dyadProductAssign(const IndexNotatedTensorExpression& other){

    //
    static IndexNotationOperator scalarOperation = IndexNotationOperator::Multiplication;
    static TensorExpressionOperator operation = TensorExpressionOperator::DyadicProduct;

    // ASSERTS
    RETURNING_ASSERT(tensorOrder > 0 && other.tensorOrder > 0,
                    "Dyadproduktbildung mit skalaren Operanden funktioniert nicht",);

    //
    bool copySelf = false;

    //
    if(Relation == TkType::Argument){ fillIndices(); }

    //
    if(Relation != TkType::Operator || Operator != scalarOperation){

        // mov
        if(this == &other){ copySelf = true; }
        moveSelfIntoFirstChild();

        // node erneut Aufsetzen
        Relation = TkType::Operator;
        Operator = scalarOperation;
        notatedIndices = children.begin()->notatedIndices;
        tensorOrder = children.begin()->tensorOrder;
    }

    //
    children.emplace_back(copySelf ? children.back() : other);
    if(children.back().Relation == TkType::Argument){ children.back().fillIndices(); }

    //
    const std::vector<NotationIndex>& operand0Indices = children.size() > 2 ? this->getSortedIndices() : children.begin()->getSortedIndices();
    const std::vector<NotationIndex>& operand1Indices = children.back().getSortedIndices();

    // Eigentliche Logik

    // Addition >> alle Indices des erster Operanden werden in die zweiten geschrieben

    // unwrap (nur für assoziative Operatoren)
    if(children.back().Relation == TkType::Operator && children.back().Operator == scalarOperation){

        // sichere Kopie
        std::vector<IndexNotatedTensorExpression> tempChildren = children.back().children;
        children.pop_back();
        children.insert(children.end(), std::make_move_iterator(tempChildren.begin()), std::make_move_iterator(tempChildren.end()));
    }

    // Tensor Order muss nicht angepasst werde
    notatedIndices = getUniqueChildIndices();
    tensorOrder = notatedIndices.size();
}

void IndexNotatedTensorExpression::mirroringDoubleContractionAssign(const IndexNotatedTensorExpression& other){

    //
    static IndexNotationOperator scalarOperation = IndexNotationOperator::Multiplication;
    static TensorExpressionOperator operation = TensorExpressionOperator::MirroringDoubleContraction;

    // ASSERTS
    RETURNING_ASSERT(tensorOrder > 1 && other.tensorOrder > 1,
                    "Für Doppelte Überschiebungen werden Tensoren mit jeweils Tensorstufe > 2 benötigt",);

    //
    bool copySelf = false;

    //
    if(Relation == TkType::Argument){ fillIndices(); }

    //
    if(Relation != TkType::Operator || Operator != scalarOperation){

        // mov
        if(this == &other){ copySelf = true; }
        moveSelfIntoFirstChild();

        // node erneut Aufsetzen
        Relation = TkType::Operator;
        Operator = scalarOperation;
        notatedIndices = children.begin()->notatedIndices;
        tensorOrder = children.begin()->tensorOrder;
    }

    //
    children.emplace_back(copySelf ? children.back() : other);
    if(children.back().Relation == TkType::Argument){ children.back().fillIndices(); }

    //
    const std::vector<NotationIndex>& operand0Indices = children.size() > 2 ? this->getSortedIndices() : children.begin()->getSortedIndices();
    const std::vector<NotationIndex>& operand1Indices = children.back().getSortedIndices();

    // Eigentliche Logik

    // Addition >> alle Indices des erster Operanden werden in die zweiten geschrieben
    children.back().replaceIndices({operand1Indices[0], operand1Indices[1]}, {operand0Indices.back(), operand0Indices[operand0Indices.size() - 2]});

    // unwrap (nur für assoziative Operatoren)
    if(children.back().Relation == TkType::Operator && children.back().Operator == scalarOperation){

        // sichere Kopie
        std::vector<IndexNotatedTensorExpression> tempChildren = children.back().children;
        children.pop_back();
        children.insert(children.end(), std::make_move_iterator(tempChildren.begin()), std::make_move_iterator(tempChildren.end()));
    }

    // Tensor Order muss nicht angepasst werde
    notatedIndices = getUniqueChildIndices();
    tensorOrder = notatedIndices.size();
}

void IndexNotatedTensorExpression::crossingDoubleContractionAssign(const IndexNotatedTensorExpression& other){

    //
    static IndexNotationOperator scalarOperation = IndexNotationOperator::Multiplication;
    static TensorExpressionOperator operation = TensorExpressionOperator::CrossingDoubleContraction;

    // ASSERTS
    RETURNING_ASSERT(tensorOrder > 1 && other.tensorOrder > 1,
                    "Für Doppelte Überschiebungen werden Tensoren mit jeweils Tensorstufe > 2 benötigt",);

    //
    bool copySelf = false;

    //
    if(Relation == TkType::Argument){ fillIndices(); }

    //
    if(Relation != TkType::Operator || Operator != scalarOperation){

        // mov
        if(this == &other){ copySelf = true; }
        moveSelfIntoFirstChild();

        // node erneut Aufsetzen
        Relation = TkType::Operator;
        Operator = scalarOperation;
        notatedIndices = children.begin()->notatedIndices;
        tensorOrder = children.begin()->tensorOrder;
    }

    //
    children.emplace_back(copySelf ? children.back() : other);
    if(children.back().Relation == TkType::Argument){ children.back().fillIndices(); }

    //
    const std::vector<NotationIndex>& operand0Indices = children.size() > 2 ? this->getSortedIndices() : children.begin()->getSortedIndices();
    const std::vector<NotationIndex>& operand1Indices = children.back().getSortedIndices();

    // Eigentliche Logik

    // Addition >> alle Indices des erster Operanden werden in die zweiten geschrieben
    children.back().replaceIndices({operand1Indices[1], operand1Indices[0]}, {operand0Indices.back(), operand0Indices[operand0Indices.size() - 2]});

    // unwrap (nur für assoziative Operatoren)
    if(children.back().Relation == TkType::Operator && children.back().Operator == scalarOperation){

        // sichere Kopie
        std::vector<IndexNotatedTensorExpression> tempChildren = children.back().children;
        children.pop_back();
        children.insert(children.end(), std::make_move_iterator(tempChildren.begin()), std::make_move_iterator(tempChildren.end()));
    }

    // Tensor Order muss nicht angepasst werde
    notatedIndices = getUniqueChildIndices();
    tensorOrder = notatedIndices.size();
}

void IndexNotatedTensorExpression::transposeAssign(){

    //
    if(Relation == TkType::Argument){ fillIndices(); }

    //
    moveSelfIntoFirstChild();

    // node erneut Aufsetzen
    Relation = TkType::Container;
    Operator = IndexNotationOperator::Transposition;

    notatedIndices = children.begin()->notatedIndices;
    std::reverse(notatedIndices.begin(), notatedIndices.end());

    tensorOrder = children.begin()->tensorOrder;
}

void IndexNotatedTensorExpression::inverseAssign(){

    //
    if(Relation == TkType::Argument){ fillIndices(); }

    //
    moveSelfIntoFirstChild();

    // neue Indizes zuweisen ?? >> kein Zusammenhang zwischen indices bzw. Einträgen von source und inverse

    // node erneut Aufsetzen
    Relation = TkType::Container;
    Operator = IndexNotationOperator::Inversion;

    // Option 1
    // notatedIndices = children.begin()->notatedIndices;

    // Option 2
    notatedIndices.reserve(children.begin()->notatedIndices.size());
    for(size_t i = 0; i < children.begin()->notatedIndices.size(); i++){
        notatedIndices.emplace_back(IndexNotatedTensorExpression::NotationIndexCounter++);
    }

    tensorOrder = children.begin()->tensorOrder;
}

void IndexNotatedTensorExpression::traceAssign(){

    //
    RETURNING_ASSERT(tensorOrder > 1, "Tensor hat keine ausreichende Stufe um die Spur zu bestimmen",);

    //
    if(Relation == TkType::Argument){ fillIndices(); }

    // Container benötigt ??

    //
    const std::vector<NotationIndex>& indices = getSortedIndices();

    //
    for(size_t i = 1; i < indices.size(); i++){
        replaceIndex(indices[i], indices[0]);
    }

    //
    moveSelfIntoFirstChild();

    // node erneut Aufsetzen
    Relation = TkType::Container;
    Operator = IndexNotationOperator::Trace;
    notatedIndices = getUniqueChildIndices();
    tensorOrder = notatedIndices.size();
}

void IndexNotatedTensorExpression::traceAssign(int contractIndices){

    //
    RETURNING_ASSERT(tensorOrder > 1, "Tensor hat keine ausreichende Stufe um die Spur zu bestimmen",);
    RETURNING_ASSERT(tensorOrder >= contractIndices, "Tensor kann nicht so viele Indices kontrahieren",);

    //
    if(Relation == TkType::Argument){ fillIndices(); }

    //
    const std::vector<NotationIndex>& indices = getSortedIndices();

    //
    size_t i = 1, contractedIndices = 0;
    while(contractedIndices < contractIndices && i < indices.size()){

        if(indices[i] != indices[0]){ replaceIndex(indices[i], indices[0]); contractedIndices++; }
        i++;
    }

    //
    moveSelfIntoFirstChild();

    // node erneut Aufsetzen
    Relation = TkType::Container;
    Operator = IndexNotationOperator::Trace;
    notatedIndices = getUniqueChildIndices();
    tensorOrder = notatedIndices.size();
}

void IndexNotatedTensorExpression::determinantAssign(){

    //
    RETURNING_ASSERT(tensorOrder > 1, "Tensor hat keine ausreichende Stufe um die Spur zu bestimmen",);

    //
    if(Relation == TkType::Argument){ fillIndices(); }

    // Container benötigt ??

    //
    moveSelfIntoFirstChild();

    // node erneut Aufsetzen
    Relation = TkType::Container;
    Operator = IndexNotationOperator::Determinant;
    notatedIndices = {};
    tensorOrder = 0;
}

std::string IndexNotatedTensorExpression::toString(size_t depth) const {

    //
    std::string result = "";

    if(depth == 0){

        result += "Res[";

        for(size_t i = 0; i < notatedIndices.size(); i++){

            result += std::to_string(notatedIndices[i]);
            result += i < notatedIndices.size() - 1 ? "," : "";
        }

        result += "] = ";
    }

    if(isConstant){
        
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(6) << value;
        std::string str = oss.str();

        // Entferne trailing zeros
        str.erase(str.find_last_not_of('0') + 1, std::string::npos);

        // Entferne trailing '.' wenn vorhanden
        if(str.back() == '.') str.pop_back();

        result += str;
    }
    else if(Relation == TkType::Argument){

        result += label + "[";

        for(size_t i = 0; i < notatedIndices.size(); i++){

            result += std::to_string(notatedIndices[i]);
            result += i < notatedIndices.size() - 1 ? "," : "";
        }

        result += "]";
    }
    else if(Relation == TkType::Container){

        result += "Container " + std::string(magic_enum::enum_name(Operator)) + " [";

        for(size_t i = 0; i < notatedIndices.size(); i++){

            result += std::to_string(notatedIndices[i]);
            result += i < notatedIndices.size() - 1 ? "," : "";
        }

        result += "]";

        result += "{ ";

        for(size_t i = 0; i < children.size(); i++){

            const IndexNotatedTensorExpression& child = children[i];

            result += child.toString(depth + 1);
        }

        result += " }";
    }
    else if(Relation == TkType::Operator && Operator == IndexNotationOperator::Diff){

        result += "diff(";

        for(size_t i = 0; i < children.size(); i++){

            const IndexNotatedTensorExpression& child = children[i];

            result += i > 0 ? ", " : "";
            result += child.toString(depth + 1);
        }

        result += ")";
    }
    else if(Relation == TkType::Operator){
        
        RETURNING_ASSERT(IndexNotationOperatorStrings.contains(Operator), "Unbekannter IndexNotationOperator", "");

        result += depth > 0 ? "(" : "";

        for(size_t i = 0; i < children.size(); i++){

            const IndexNotatedTensorExpression& child = children[i];

            result += (i > 0) ? " " + IndexNotationOperatorStrings[Operator] + " " : "";
            result += child.toString(depth + 1);
        }

        result += depth > 0 ? ")" : "";
    }

    return result;
}

std::ostream& operator<<(std::ostream& os, const IndexNotatedTensorExpression& expr){

    os << "TensorExpr >> ";

    if(expr.Relation == TkType::Argument){

        os << "Argument '" << expr.label << "' [";

        for(const auto& idx : expr.notatedIndices){

            os << idx << ", ";
        }

        os << "]";
    }
    else if(expr.Relation == TkType::Operator){

        os << "Operation : " << magic_enum::enum_name(expr.Operator) << " | indices [";

        for(const auto& idx : expr.notatedIndices){

            os << idx << ", ";
        }

        os << "] | " << expr.children.size() << " childs : " << endl;

        for(const auto& child : expr.children){

            os << "| " << child << endl; 
        }
    }
    else if(expr.Relation == TkType::Container){

        os << "Container | indices [";

        for(const auto& idx : expr.notatedIndices){

            os << idx << ", ";
        }

        os << "] | " << expr.children.size() << " childs : " << endl;

        for(const auto& child : expr.children){

            os << "| " << child << endl; 
        }
    }
    else{

        os << "NodeType '" << magic_enum::enum_name(expr.Relation) << "'";
    }

    return os;
}

std::map<TensorExpressionOperator, void(IndexNotatedTensorExpression::*)(const IndexNotatedTensorExpression&)> operatorFunctions = {

    {TensorExpressionOperator::Addition, &IndexNotatedTensorExpression::addAssign},
    {TensorExpressionOperator::Subtraction, &IndexNotatedTensorExpression::subAssign},
    {TensorExpressionOperator::Multiplication, &IndexNotatedTensorExpression::mulAssign},
    {TensorExpressionOperator::DotProduct, &IndexNotatedTensorExpression::dotProductAssign},
    {TensorExpressionOperator::CrossProduct, &IndexNotatedTensorExpression::crossProductAssign},
    {TensorExpressionOperator::DyadicProduct, &IndexNotatedTensorExpression::dyadProductAssign},
    {TensorExpressionOperator::MirroringDoubleContraction, &IndexNotatedTensorExpression::mirroringDoubleContractionAssign},
    {TensorExpressionOperator::CrossingDoubleContraction, &IndexNotatedTensorExpression::crossingDoubleContractionAssign},
    {TensorExpressionOperator::Diff, &IndexNotatedTensorExpression::diffAssign},
};

bool IndexNotatedTensorExpression::equals(const IndexNotatedTensorExpression& other){

    if(label != other.label){ return false; }

    if(notatedIndices.size() != other.notatedIndices.size()){ return false; }

    for(size_t idx = 0; idx < notatedIndices.size(); idx++){

        if(notatedIndices[idx] != other.notatedIndices[idx]){ return false; }
    }

    return true;
}

void IndexNotatedTensorExpression::diffAssign(const IndexNotatedTensorExpression& other){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::Diff;

    bool copySelf = false;

    // mov
    if(this == &other){ copySelf = true; }
    moveSelfIntoFirstChild();

    // node erneut Aufsetzen
    Relation = TkType::Operator;
    Operator = IndexNotationOperator::Diff;

    //
    children.emplace_back(copySelf ? children.back() : other);

    //
    if(children.begin()->Relation == TkType::Argument){ children.begin()->fillIndices(); }
    if(children.back().Relation == TkType::Argument){ children.back().fillIndices(); }

    //
    tensorOrder = children.begin()->tensorOrder + children.back().tensorOrder;

    //
    notatedIndices.clear();
    notatedIndices = getUniqueChildIndices();
}

IndexNotatedTensorExpression convertToIndexNotation(const TensorExpression& expr, size_t depth = 0){

    IndexNotatedTensorExpression res;

    switch(expr.Relation){
        
        case (TkType::Argument):{

            if(expr.isConstant){

                res = IndexNotatedTensorExpression(expr.value);
            }
            else{

                res = IndexNotatedTensorExpression(expr.label, expr.tensorOrder);
            }

            break;
        }
        case (TkType::Operator):{

            res = convertToIndexNotation(expr.children[0]);

            if(operatorFunctions.contains(expr.Operator)){

                RETURNING_ASSERT(expr.children.size() > 1, "...", res);

                for(size_t idx = 1; idx < expr.children.size(); idx++){

                    (res.*operatorFunctions[expr.Operator])(convertToIndexNotation(expr.children[idx], depth + 1));
                }
            }
            else if(expr.Operator == TensorExpressionOperator::Inversion){

                res.inverseAssign(); 
            }
            else if(expr.Operator == TensorExpressionOperator::Transposition){

                res.transposeAssign();
            }
            else if(expr.Operator == TensorExpressionOperator::Trace && expr.tensorOrder == 0){

                res.traceAssign();
            }
            else if(expr.Operator == TensorExpressionOperator::Trace){
                
                res.traceAssign(expr.children.begin()->tensorOrder - expr.tensorOrder - 1);
            }
            else if(expr.Operator == TensorExpressionOperator::Determinant){
                
                res.determinantAssign();
            }
            else if(expr.Operator == TensorExpressionOperator::Section){
                
            }
            else{

                RETURNING_ASSERT(TRIGGER_ASSERT,
                    "Für Operator " + std::string(magic_enum::enum_name(expr.Operator)) + " ist keine verknüpfende Funktion hinterlegt", res);
            }

            break;
        }
        default:{

            break;
        }
    }

    return res;
}

namespace types{

    int INDEX_NOTATED_TENSOR_EXPRESSION::setUpClass(){

        // register in TypeRegister
        if(!init("tIdn", [](){ return new INDEX_NOTATED_TENSOR_EXPRESSION(); })){ return false; }

        // Registrierung von Keyword zur rvalue Konstruktion von ints
        // ...

        // Konstruktoren
        registerFunction("IndexNotatedTensorExpression", {STRING::typeIndex, INT::typeIndex},
            [__functionLabel__ = "IndexNotatedTensorExpression", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_RETURN(INDEX_NOTATED_TENSOR_EXPRESSION, 0);
                GET_ARG(STRING, 0); GET_ARG(INT, 1);

                // schreiben in returns
                ret0->getMember() = IndexNotatedTensorExpression(arg0->getMember(), arg1->getMember());
        },
        {INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex});

        // Konstruktoren
        registerFunction("tIDN", {STRING::typeIndex, INT::typeIndex},
            [__functionLabel__ = "tIDN", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_RETURN(INDEX_NOTATED_TENSOR_EXPRESSION, 0);
                GET_ARG(STRING, 0); GET_ARG(INT, 1);

                // schreiben in returns
                ret0->getMember() = IndexNotatedTensorExpression(arg0->getMember(), arg1->getMember());
        },
        {INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex});

        //
        registerFunction("toIDN", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "toIDN", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_RETURN(INDEX_NOTATED_TENSOR_EXPRESSION, 0);
                GET_ARG(TENSOR_EXPRESSION, 0);

                //
                ret0->getMember() = convertToIndexNotation(arg0->getMember());
        },
        {INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex});

        // Member
        registerMemberFunction(INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex, "toString", {},
            [__functionLabel__ = "toString", __numArgs__ = 0](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_RETURN(STRING, 0);
                GET_MEMBER(INDEX_NOTATED_TENSOR_EXPRESSION);

                //
                ret0->getMember() = mb->getMember().toString();
        },
        {STRING::typeIndex});

        // Operator Überladung
        registerFunction("__addAssign__", {INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex, INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__addAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(INDEX_NOTATED_TENSOR_EXPRESSION, 0); GET_ARG(INDEX_NOTATED_TENSOR_EXPRESSION, 1);

                IndexNotatedTensorExpression& member0 = arg0->getMember();
                IndexNotatedTensorExpression& member1 = arg1->getMember();

                member0.addAssign(member1);
        },
        {});

        registerFunction("__subAssign__", {INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex, INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__subAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(INDEX_NOTATED_TENSOR_EXPRESSION, 0); GET_ARG(INDEX_NOTATED_TENSOR_EXPRESSION, 1);

                IndexNotatedTensorExpression& member0 = arg0->getMember();
                IndexNotatedTensorExpression& member1 = arg1->getMember();

                member0.subAssign(member1);
        },
        {});

        registerFunction("__mulAssign__", {INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex, INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__mulAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(INDEX_NOTATED_TENSOR_EXPRESSION, 0); GET_ARG(INDEX_NOTATED_TENSOR_EXPRESSION, 1);

                IndexNotatedTensorExpression& member0 = arg0->getMember();
                IndexNotatedTensorExpression& member1 = arg1->getMember();

                member0.mulAssign(member1);
        },
        {});

        registerFunction("__dotProductAssign__", {INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex, INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__dotProductAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(INDEX_NOTATED_TENSOR_EXPRESSION, 0); GET_ARG(INDEX_NOTATED_TENSOR_EXPRESSION, 1);

                IndexNotatedTensorExpression& member0 = arg0->getMember();
                IndexNotatedTensorExpression& member1 = arg1->getMember();

                member0.dotProductAssign(member1);
        },
        {});

        registerFunction("__crossProductAssign__", {INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex, INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__crossProductAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(INDEX_NOTATED_TENSOR_EXPRESSION, 0); GET_ARG(INDEX_NOTATED_TENSOR_EXPRESSION, 1);

                IndexNotatedTensorExpression& member0 = arg0->getMember();
                IndexNotatedTensorExpression& member1 = arg1->getMember();

                member0.crossProductAssign(member1);
        },
        {});

        registerFunction("__dyadProductAssign__", {INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex, INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__dyadProductAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(INDEX_NOTATED_TENSOR_EXPRESSION, 0); GET_ARG(INDEX_NOTATED_TENSOR_EXPRESSION, 1);

                IndexNotatedTensorExpression& member0 = arg0->getMember();
                IndexNotatedTensorExpression& member1 = arg1->getMember();

                member0.dyadProductAssign(member1);
        },
        {});

        registerFunction("__mirroringDoubleContractionAssign__", {INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex, INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__mirroringDoubleContractionAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(INDEX_NOTATED_TENSOR_EXPRESSION, 0); GET_ARG(INDEX_NOTATED_TENSOR_EXPRESSION, 1);

                IndexNotatedTensorExpression& member0 = arg0->getMember();
                IndexNotatedTensorExpression& member1 = arg1->getMember();

                member0.mirroringDoubleContractionAssign(member1);
        },
        {});

        registerFunction("__crossingDoubleContractionAssign__", {INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex, INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__crossingDoubleContractionAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(INDEX_NOTATED_TENSOR_EXPRESSION, 0); GET_ARG(INDEX_NOTATED_TENSOR_EXPRESSION, 1);

                IndexNotatedTensorExpression& member0 = arg0->getMember();
                IndexNotatedTensorExpression& member1 = arg1->getMember();

                member0.crossingDoubleContractionAssign(member1);
        },
        {});

        //
        registerFunction("__inverseAssign__", {INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__inverseAssign__", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                if(inputs[0]->isLValue()){ returns[0].cloneIntoRValue(inputs[0]->getVariableRef()); }
                else{ returns[0].moveIntoRValue(inputs[0]->getVariableRef()); }

                GET_RETURN(INDEX_NOTATED_TENSOR_EXPRESSION, 0);
                ret0->getMember().inverseAssign();
        },
        {INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex});

        //
        registerFunction("__transposeAssign__", {INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__transposeAssign__", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                if(inputs[0]->isLValue()){ returns[0].cloneIntoRValue(inputs[0]->getVariableRef()); }
                else{ returns[0].moveIntoRValue(inputs[0]->getVariableRef()); }

                GET_RETURN(INDEX_NOTATED_TENSOR_EXPRESSION, 0);
                ret0->getMember().transposeAssign();
        },
        {INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex});

        //
        registerFunction("__traceAssign__", {INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__traceAssign__", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                if(inputs[0]->isLValue()){ returns[0].cloneIntoRValue(inputs[0]->getVariableRef()); }
                else{ returns[0].moveIntoRValue(inputs[0]->getVariableRef()); }

                GET_RETURN(INDEX_NOTATED_TENSOR_EXPRESSION, 0);
                ret0->getMember().traceAssign();
        },
        {INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex});

        registerFunction("__traceAssign__", {INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__traceAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_ARG(INDEX_NOTATED_TENSOR_EXPRESSION, 0); GET_ARG(INT, 1);
                arg0->getMember().traceAssign(arg1->getMember());
        },
        {});

        //
        registerFunction("__inverseInplaceAssign__", {INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__inverseInplaceAssign__", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(INDEX_NOTATED_TENSOR_EXPRESSION, 0);

                IndexNotatedTensorExpression& member0 = arg0->getMember();
                member0.inverseAssign();
        },
        {});

        //
        registerFunction("__transposeInplaceAssign__", {INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__transposeInplaceAssign__", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(INDEX_NOTATED_TENSOR_EXPRESSION, 0);

                IndexNotatedTensorExpression& member0 = arg0->getMember();
                member0.transposeAssign();
        },
        {});

        //
        registerFunction("__traceInplaceAssign__", {INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__traceInplaceAssign__", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(INDEX_NOTATED_TENSOR_EXPRESSION, 0);

                IndexNotatedTensorExpression& member0 = arg0->getMember();
                member0.traceAssign();
        },
        {});

        //
        registerFunction("diff", {INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex, INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "diff", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                //
                returns[0].getVariableRef().clone(inputs[0]->getVariableRef());

                // Returns
                GET_RETURN(INDEX_NOTATED_TENSOR_EXPRESSION, 0);
                GET_ARG(INDEX_NOTATED_TENSOR_EXPRESSION, 0); GET_ARG(INDEX_NOTATED_TENSOR_EXPRESSION, 1);

                ret0->getMember().diffAssign(arg1->getMember());
        },
        {INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex});

        //
        registerFunction("__diffAssign__", {INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex, INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__diffAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(INDEX_NOTATED_TENSOR_EXPRESSION, 0); GET_ARG(INDEX_NOTATED_TENSOR_EXPRESSION, 1);

                IndexNotatedTensorExpression& member0 = arg0->getMember();
                IndexNotatedTensorExpression& member1 = arg1->getMember();

                member0.diffAssign(member1);
        },
        {});

        return true;
    }
}