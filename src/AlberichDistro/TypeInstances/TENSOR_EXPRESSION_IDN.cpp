#include "TENSOR_EXPRESSION.h"
#include "PermutationGenerator.h"

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

IndexNotatedTensorExpression::IndexNotatedTensorExpression(const std::string& labelIn, int tensorOrderIn, const std::vector<int>& dimensionsIn)
    : IndexNotatedTensorExpression(labelIn, tensorOrderIn){

    RETURNING_ASSERT(tensorOrder == dimensionsIn.size(),
        "Konstruktionsangaben : {" + labelIn + ", " + std::to_string(tensorOrderIn) + ", " + printPlainVector(dimensionsIn) + "} sind Inkonsistent",);

    dimensions = dimensionsIn;
}

//
IndexNotatedTensorExpression IndexNotatedTensorExpression::asExternalNode(const std::string& label) const{

    auto tmp = IndexNotatedTensorExpression(label, tensorOrder, dimensions);
    tmp.notatedIndices = notatedIndices;

    return tmp;
}

bool IndexNotatedTensorExpression::containsDimensions() const {

    return dimensions.size() == tensorOrder;
}

bool IndexNotatedTensorExpression::containsIndices() const{

    //
    if(tensorOrder > 0){
        return true;
    }

    //
    else{

        for(const auto& child : children){

            if(child.containsIndices()){
                return true;
            }
        }
    }

    return false;
}

size_t IndexNotatedTensorExpression::getNumOfNodes() const{

    size_t numOfNodes = children.size();

    for(const auto& child : children){

        numOfNodes += child.getNumOfNodes();
    }

    return numOfNodes;
}

size_t IndexNotatedTensorExpression::getNumOfExternalNodes() const{

    if(children.empty()){ return 1; }

    size_t numOfExternalNodes = 0;

    for(const auto& child : children){

        numOfExternalNodes += child.getNumOfExternalNodes();
    }

    return numOfExternalNodes;
}

bool IndexNotatedTensorExpression::areEqualExternals(const IndexNotatedTensorExpression& lhs, const IndexNotatedTensorExpression& rhs){

    //
    if(lhs.Relation != TkType::Argument || lhs.Relation != TkType::Argument) return false;

    if(lhs.isConstant != rhs.isConstant) return false;
    if(lhs.value != rhs.value) return false;

    if (rhs.tensorOrder != lhs.tensorOrder) return false;
    if(lhs.dimensions != rhs.dimensions) return false;

    // Label nur relevant bei konkreten Ausdrücken
    if (rhs.label != lhs.label) return false;

    return true;
}

std::vector<const IndexNotatedTensorExpression*> IndexNotatedTensorExpression::getUniqueExternalNodes() const{

    //
    std::vector<const IndexNotatedTensorExpression*> uniqueNodes;

    // rekursive DFS Funktion
    std::function<void(const IndexNotatedTensorExpression&)> dfs = [&](const IndexNotatedTensorExpression& node)
    {
        if(node.Relation == TkType::Argument){

            // Konstanten außenvor lassen
            if(node.isConstant){ return; }

            // Prüfen ob die Node schon in uniqueNodes enthalten ist
            for (const auto* u : uniqueNodes)
            {   
                if (areEqualExternals(*u, node)){ return; }
            }

            // Node ist einzigartig
            uniqueNodes.push_back(&node);
        }

        // weiter durch childs iterieren
        for (const auto& child : node.children)
        {
            dfs(child);
        }
    };

    dfs(*this);

    return uniqueNodes;
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

std::vector<int> IndexNotatedTensorExpression::getUniqueChildDimensions() const {

    std::unordered_map<NotationIndex, int> indexCount;
    std::unordered_map<NotationIndex, int> indexToDimension;
    std::vector<NotationIndex> order;

    // Sammle Indices und ihre Dimensionen
    for (const auto& child : children) {
        for (size_t i = 0; i < child.notatedIndices.size(); i++) {
            NotationIndex idx = child.notatedIndices[i];
            int dim = child.dimensions[i];

            if (indexCount[idx]++ == 0) {
                order.push_back(idx);
                indexToDimension[idx] = dim;
            }
        }
    }

    // Filtere unique Indices und gib ihre Dimensionen zurück
    std::vector<int> uniqueDimensions;
    uniqueDimensions.reserve(order.size());

    for (auto idx : order) {
        if (indexCount[idx] == 1) {
            uniqueDimensions.push_back(indexToDimension[idx]);
        }
    }

    return uniqueDimensions;
}

std::vector<NotationIndex> IndexNotatedTensorExpression::getNotUniqueChildIndices() const {

    std::unordered_map<NotationIndex, int> indexCount;
    std::vector<NotationIndex> order;

    for (const auto& child : children) {
        for (auto idx : child.notatedIndices) {
            if (indexCount[idx]++ == 0) {
                order.push_back(idx);
            }
        }
    }

    std::vector<NotationIndex> notUniqueIndices;
    notUniqueIndices.reserve(order.size());

    for (auto idx : order) {
        if (indexCount[idx] > 1) {
            notUniqueIndices.push_back(idx);
        }
    }

    return notUniqueIndices;
}

std::vector<NotationIndex> IndexNotatedTensorExpression::getNotUniqueChildDimensions() const {

    std::unordered_map<NotationIndex, int> indexCount;
    std::unordered_map<NotationIndex, int> indexToDimension;
    std::vector<NotationIndex> order;

    // Sammle Indices und ihre Dimensionen
    for (const auto& child : children) {
        for (size_t i = 0; i < child.notatedIndices.size(); i++) {
            NotationIndex idx = child.notatedIndices[i];
            int dim = child.dimensions[i];

            if (indexCount[idx]++ == 0) {
                order.push_back(idx);
                indexToDimension[idx] = dim;
            }
        }
    }

    // Filtere nicht-unique Indices und gib ihre Dimensionen zurück
    std::vector<int> notUniqueDimensions;
    notUniqueDimensions.reserve(order.size());

    for (auto idx : order) {
        if (indexCount[idx] > 1) {
            notUniqueDimensions.push_back(indexToDimension[idx]);
        }
    }

    return notUniqueDimensions;
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
    RETURNING_ASSERT(tensorOrder == other.tensorOrder, "Addition von Tensoren unterschiedlicher Stufe versucht " + toString() + " | " + other.toString(),);

    //
    bool copySelf = false;

    //
    if(Relation == TkType::Argument){ fillIndices(); }

    //
    if(!unwrapOperands || Relation != TkType::Operator || Operator != scalarOperation){

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

    // Dimensionen anpassen

    // unwrap (nur für assoziative Operatoren)
    if(unwrapOperands && children.back().Relation == TkType::Operator && children.back().Operator == scalarOperation){

        // sichere Kopie
        std::vector<IndexNotatedTensorExpression> tempChildren = children.back().children;
        children.pop_back();
        children.insert(children.end(), std::make_move_iterator(tempChildren.begin()), std::make_move_iterator(tempChildren.end()));
    }

    // Tensor Order muss nicht angepasst werde

    // reval falls durchgereichte Index Replaces zu dopplungen in notatedIndices führen
    // reEvaluateIndices();
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
    if(!unwrapOperands || Relation != TkType::Operator || Operator != scalarOperation){

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

    // reval falls durchgereichte Index Replaces zu dopplungen in notatedIndices führen
    // reEvaluateIndices();
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
    if(!unwrapOperands || Relation != TkType::Operator || Operator != scalarOperation){

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
    if(unwrapOperands && children.back().Relation == TkType::Operator && children.back().Operator == scalarOperation){

        // sichere Kopie
        std::vector<IndexNotatedTensorExpression> tempChildren = children.back().children;
        children.pop_back();
        children.insert(children.end(), std::make_move_iterator(tempChildren.begin()), std::make_move_iterator(tempChildren.end()));
    }

    // Tensor Order muss nicht angepasst werde
    notatedIndices = getUniqueChildIndices();
    tensorOrder = notatedIndices.size();

    // reval falls durchgereichte Index Replaces zu dopplungen in notatedIndices führen
    // reEvaluateIndices();
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
    if(!unwrapOperands || Relation != TkType::Operator || Operator != scalarOperation){

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
    if(unwrapOperands && children.back().Relation == TkType::Operator && children.back().Operator == scalarOperation){

        // sichere Kopie
        std::vector<IndexNotatedTensorExpression> tempChildren = children.back().children;
        children.pop_back();
        children.insert(children.end(), std::make_move_iterator(tempChildren.begin()), std::make_move_iterator(tempChildren.end()));
    }

    // Tensor Order muss nicht angepasst werde
    notatedIndices = getUniqueChildIndices();
    tensorOrder = notatedIndices.size();

    // reval falls durchgereichte Index Replaces zu dopplungen in notatedIndices führen
    // reEvaluateIndices();
}

void IndexNotatedTensorExpression::contractingDotProductAssign(const IndexNotatedTensorExpression& other){

    //
    static IndexNotationOperator scalarOperation = IndexNotationOperator::Multiplication;
    static TensorExpressionOperator operation = TensorExpressionOperator::ContractingDotProduct;

    // ASSERTS
    RETURNING_ASSERT(tensorOrder > 0 && other.tensorOrder > 0,
                    "Skalarproduktbildung mit skalaren Operanden funktioniert nicht",);

    //
    bool copySelf = false;

    //
    if(Relation == TkType::Argument){ fillIndices(); }

    //
    if(!unwrapOperands || Relation != TkType::Operator || Operator != scalarOperation){

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
    int contractIndices = std::min(operand0Indices.size(), operand1Indices.size());

    // >> n erste Indices von other werden mit n letzten Indices von A überschreiben

    // Kontrahiere die letzten contractIndices von operand0
    // mit den ersten contractIndices von operand1
    for(int i = 0; i < contractIndices; i++) {

        static const NotationIndex* op0_idx;
        static const NotationIndex* op1_idx;

        op0_idx = &operand0Indices[operand0Indices.size() - contractIndices + i];
        op1_idx = &operand1Indices[i];

        children.back().replaceIndex(*op1_idx, *op0_idx);
    }

    // unwrap (nur für assoziative Operatoren)
    if(unwrapOperands && children.back().Relation == TkType::Operator && children.back().Operator == scalarOperation){

        // sichere Kopie
        std::vector<IndexNotatedTensorExpression> tempChildren = children.back().children;
        children.pop_back();
        children.insert(children.end(), std::make_move_iterator(tempChildren.begin()), std::make_move_iterator(tempChildren.end()));
    }

    // Tensor Order muss nicht angepasst werde
    notatedIndices = getUniqueChildIndices();
    tensorOrder = notatedIndices.size();

    // reval falls durchgereichte Index Replaces zu dopplungen in notatedIndices führen
    // reEvaluateIndices();
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
    if(!unwrapOperands || Relation != TkType::Operator || Operator != scalarOperation){

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
    if(unwrapOperands && children.back().Relation == TkType::Operator && children.back().Operator == scalarOperation){

        // sichere Kopie
        std::vector<IndexNotatedTensorExpression> tempChildren = children.back().children;
        children.pop_back();
        children.insert(children.end(), std::make_move_iterator(tempChildren.begin()), std::make_move_iterator(tempChildren.end()));
    }

    //
    int vectorLen = children.front().dimensions.front();
    IndexNotatedTensorExpression civitaDelta("eps", 3);
    civitaDelta.dimensions = {vectorLen, vectorLen, vectorLen};

    civitaDelta.notatedIndices = {IndexNotatedTensorExpression::NotationIndexCounter++, operand0Indices.back(), operand1Indices.back()};
    children.emplace_back(std::move(civitaDelta));

    // Tensor Order muss nicht angepasst werde
    notatedIndices = getUniqueChildIndices();
    tensorOrder = notatedIndices.size();

    // reval falls durchgereichte Index Replaces zu dopplungen in notatedIndices führen
    // reEvaluateIndices();
}

void IndexNotatedTensorExpression::dyadProductAssign(const IndexNotatedTensorExpression& other){

    //
    static IndexNotationOperator scalarOperation = IndexNotationOperator::Multiplication;
    static TensorExpressionOperator operation = TensorExpressionOperator::DyadicProduct;

    // ASSERTS
    // RETURNING_ASSERT(tensorOrder > 0 && other.tensorOrder > 0,
    //                 "Dyadproduktbildung mit skalaren Operanden funktioniert nicht",);

    //
    bool copySelf = false;

    //
    if(Relation == TkType::Argument){ fillIndices(); }

    //
    if(!unwrapOperands || Relation != TkType::Operator || Operator != scalarOperation){

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
    if(unwrapOperands && children.back().Relation == TkType::Operator && children.back().Operator == scalarOperation){

        // sichere Kopie
        std::vector<IndexNotatedTensorExpression> tempChildren = children.back().children;
        children.pop_back();
        children.insert(children.end(), std::make_move_iterator(tempChildren.begin()), std::make_move_iterator(tempChildren.end()));
    }

    // Tensor Order muss nicht angepasst werde
    notatedIndices = getUniqueChildIndices();
    tensorOrder = notatedIndices.size();

    // reval falls durchgereichte Index Replaces zu dopplungen in notatedIndices führen
    // reEvaluateIndices();
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
    if(!unwrapOperands || Relation != TkType::Operator || Operator != scalarOperation){

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
    if(unwrapOperands && children.back().Relation == TkType::Operator && children.back().Operator == scalarOperation){

        // sichere Kopie
        std::vector<IndexNotatedTensorExpression> tempChildren = children.back().children;
        children.pop_back();
        children.insert(children.end(), std::make_move_iterator(tempChildren.begin()), std::make_move_iterator(tempChildren.end()));
    }

    // Tensor Order muss nicht angepasst werde
    notatedIndices = getUniqueChildIndices();
    tensorOrder = notatedIndices.size();

    // reval falls durchgereichte Index Replaces zu dopplungen in notatedIndices führen
    // reEvaluateIndices();
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
    if(!unwrapOperands || Relation != TkType::Operator || Operator != scalarOperation){

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
    if(unwrapOperands && children.back().Relation == TkType::Operator && children.back().Operator == scalarOperation){

        // sichere Kopie
        std::vector<IndexNotatedTensorExpression> tempChildren = children.back().children;
        children.pop_back();
        children.insert(children.end(), std::make_move_iterator(tempChildren.begin()), std::make_move_iterator(tempChildren.end()));
    }

    // Tensor Order muss nicht angepasst werde
    notatedIndices = getUniqueChildIndices();
    tensorOrder = notatedIndices.size();

    // reval falls durchgereichte Index Replaces zu dopplungen in notatedIndices führen
    // reEvaluateIndices();
}

//
void IndexNotatedTensorExpression::contractAssign(const IndexNotatedTensorExpression& other, int order, bool reversed){

    //
    static IndexNotationOperator scalarOperation = IndexNotationOperator::Multiplication;
    static TensorExpressionOperator operation = TensorExpressionOperator::Contract;

    // ASSERTS
    RETURNING_ASSERT(tensorOrder > 1 && other.tensorOrder > 1, "Für Kontraktion werden Tensoren mit jeweils Tensorstufe > 1 benötigt",);
    RETURNING_ASSERT(order > 1, "Nicht aufgelöste beliebige Kontraktion kann nciht in Indexnotation umgewandelt werden",);

    //
    bool copySelf = false;

    //
    if(Relation == TkType::Argument){ fillIndices(); }

    // Wenn gepackt wird, der erste Operand kein Operator, keine scalareOperation
    if(!unwrapOperands || Relation != TkType::Operator || Operator != scalarOperation){

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

    if(reversed){

        // Contract Reversed (..., i, j) -> (j, i, ...)
        children.back().replaceIndices(
            std::vector<NotationIndex>(operand1Indices.rend() - order, operand1Indices.rend()),     // die ersten n Indizes (reversed) von Operand 1 werden ersetzt
            std::vector<NotationIndex>(operand0Indices.end() - order, operand0Indices.end()));          // mit den letzten n Indizes (forward) von Operand 0
    }
    else{

        // Contract Forward (..., i, j) -> (i, j, ...)
        children.back().replaceIndices(
            std::vector<NotationIndex>(operand1Indices.begin(), operand1Indices.begin() + order),       // die ersten n Indizes (forward) von Operand 1 werden ersetzt
            std::vector<NotationIndex>(operand0Indices.end() - order, operand0Indices.end()));          // mit den letzten n Indizes (forward) von Operand 0
    }

    // unwrap (nur für assoziative Operatoren)
    if(unwrapOperands && children.back().Relation == TkType::Operator && children.back().Operator == scalarOperation){

        // sichere Kopie
        std::vector<IndexNotatedTensorExpression> tempChildren = children.back().children;
        children.pop_back();
        children.insert(children.end(), std::make_move_iterator(tempChildren.begin()), std::make_move_iterator(tempChildren.end()));
    }

    // Tensor Order muss nicht angepasst werde
    notatedIndices = getUniqueChildIndices();
    tensorOrder = notatedIndices.size();

    // reval falls durchgereichte Index Replaces zu dopplungen in notatedIndices führen
    // reEvaluateIndices();
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

    // reval falls durchgereichte Index Replaces zu dopplungen in notatedIndices führen
    // reEvaluateIndices();
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

    // reval falls durchgereichte Index Replaces zu dopplungen in notatedIndices führen
    // reEvaluateIndices();
}

void IndexNotatedTensorExpression::traceAssign(){

    //
    RETURNING_ASSERT(tensorOrder > 1, "Tensor hat keine ausreichende Stufe um die Spur zu bestimmen",);

    //
    if(containsDimensions()){
        RETURNING_ASSERT(std::all_of(dimensions.begin(), dimensions.end(), [&](int x) { return x == dimensions.front(); }), "...",);
    }

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

    // reval falls durchgereichte Index Replaces zu dopplungen in notatedIndices führen
    // reEvaluateIndices();
}

void IndexNotatedTensorExpression::traceAssign(int contractIndices){

    //
    RETURNING_ASSERT(tensorOrder > 1, "Tensor hat keine ausreichende Stufe um die Spur zu bestimmen",);
    RETURNING_ASSERT(tensorOrder >= contractIndices, "Tensor kann nicht so viele Indices kontrahieren",);

    //
    if(Relation == TkType::Argument){ fillIndices(); }

    //
    if(containsDimensions()){
        RETURNING_ASSERT(std::all_of(dimensions.end() - contractIndices - 1, dimensions.end(), [&](int x) { return x == dimensions.back(); }), "...",);
    }

    //
    const std::vector<NotationIndex>& indices = getSortedIndices();

    //
    size_t i = indices.size() - 1, contractedIndices = 0;
    while(contractedIndices < contractIndices && i > 0){

        if(indices[i] != indices.back()){ replaceIndex(indices[i], indices.back()); contractedIndices++; }
        i--;
    }

    //
    moveSelfIntoFirstChild();

    // node erneut Aufsetzen
    Relation = TkType::Container;
    Operator = IndexNotationOperator::Trace;
    notatedIndices = getUniqueChildIndices();
    tensorOrder = notatedIndices.size();

    // reval falls durchgereichte Index Replaces zu dopplungen in notatedIndices führen
    // reEvaluateIndices();
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

    // reval falls durchgereichte Index Replaces zu dopplungen in notatedIndices führen
    // reEvaluateIndices();
}

void IndexNotatedTensorExpression::frobeniusAssign(){

    //
    // RETURNING_ASSERT(tensorOrder > 1, "Tensor hat keine ausreichende Stufe um die Spur zu bestimmen",);

    //
    if(Relation == TkType::Argument){ fillIndices(); }

    // Container benötigt ??

    //
    moveSelfIntoFirstChild();

    // node erneut Aufsetzen
    Relation = TkType::Container;
    Operator = IndexNotationOperator::Frobenius;
    notatedIndices = {};
    tensorOrder = 0;

    // reval falls durchgereichte Index Replaces zu dopplungen in notatedIndices führen
    // reEvaluateIndices();
}

void IndexNotatedTensorExpression::macaulayAssign(){

    //
    RETURNING_ASSERT(tensorOrder < 1, "Tensor hat keine ausreichende Stufe um die Spur zu bestimmen",);

    //
    moveSelfIntoFirstChild();

    // node erneut Aufsetzen
    Relation = TkType::Container;
    Operator = IndexNotationOperator::Macaulay;
    notatedIndices = {};
    tensorOrder = 0;

    // reval falls durchgereichte Index Replaces zu dopplungen in notatedIndices führen
    // reEvaluateIndices();
}

void IndexNotatedTensorExpression::signumAssign(){

    //
    RETURNING_ASSERT(tensorOrder < 1, "Tensor hat keine ausreichende Stufe um die Spur zu bestimmen",);

    //
    moveSelfIntoFirstChild();

    // node erneut Aufsetzen
    Relation = TkType::Container;
    Operator = IndexNotationOperator::Signum;
    notatedIndices = {};
    tensorOrder = 0;

    // reval falls durchgereichte Index Replaces zu dopplungen in notatedIndices führen
    // reEvaluateIndices();
}

void IndexNotatedTensorExpression::sqrtAssign(){

    //
    RETURNING_ASSERT(tensorOrder < 1, "Tensor hat keine ausreichende Stufe um die Spur zu bestimmen",);

    //
    moveSelfIntoFirstChild();

    // node erneut Aufsetzen
    Relation = TkType::Container;
    Operator = IndexNotationOperator::Sqrt;
    notatedIndices = {};
    tensorOrder = 0;

    // reval falls durchgereichte Index Replaces zu dopplungen in notatedIndices führen
    // reEvaluateIndices();
}

void IndexNotatedTensorExpression::sinAssign(){

    //
    RETURNING_ASSERT(tensorOrder < 1, "Tensor hat keine ausreichende Stufe um die Spur zu bestimmen",);

    //
    moveSelfIntoFirstChild();

    // node erneut Aufsetzen
    Relation = TkType::Container;
    Operator = IndexNotationOperator::Sin;
    notatedIndices = {};
    tensorOrder = 0;

    // reval falls durchgereichte Index Replaces zu dopplungen in notatedIndices führen
    // reEvaluateIndices();
}

void IndexNotatedTensorExpression::cosAssign(){

    //
    RETURNING_ASSERT(tensorOrder < 1, "Tensor hat keine ausreichende Stufe um die Spur zu bestimmen",);

    //
    moveSelfIntoFirstChild();

    // node erneut Aufsetzen
    Relation = TkType::Container;
    Operator = IndexNotationOperator::Cos;
    notatedIndices = {};
    tensorOrder = 0;

    // reval falls durchgereichte Index Replaces zu dopplungen in notatedIndices führen
    // reEvaluateIndices();
}

void IndexNotatedTensorExpression::tanAssign(){

    //
    RETURNING_ASSERT(tensorOrder < 1, "Tensor hat keine ausreichende Stufe um die Spur zu bestimmen",);

    //
    moveSelfIntoFirstChild();

    // node erneut Aufsetzen
    Relation = TkType::Container;
    Operator = IndexNotationOperator::Tan;
    notatedIndices = {};
    tensorOrder = 0;

    // reval falls durchgereichte Index Replaces zu dopplungen in notatedIndices führen
    // reEvaluateIndices();
}

void IndexNotatedTensorExpression::cotanAssign(){

    //
    RETURNING_ASSERT(tensorOrder < 1, "Tensor hat keine ausreichende Stufe um die Spur zu bestimmen",);

    //
    moveSelfIntoFirstChild();

    // node erneut Aufsetzen
    Relation = TkType::Container;
    Operator = IndexNotationOperator::Cotan;
    notatedIndices = {};
    tensorOrder = 0;

    // reval falls durchgereichte Index Replaces zu dopplungen in notatedIndices führen
    // reEvaluateIndices();
}

void IndexNotatedTensorExpression::evaluateIndexDimensions(std::map<int, int>& indexDimensions) const{

    for(size_t i = 0; i < notatedIndices.size(); i++){

        const auto& idx = notatedIndices[i];

        if(indexDimensions.contains(idx)){

            // >> nach extern vergebener Index
            // ...

            continue;
        }

        const auto& dim = dimensions[i];

        indexDimensions.try_emplace(idx, dim);
    }

    for(size_t i = 0; i < children.size(); i++){

        children[i].evaluateIndexDimensions(indexDimensions);
    }
}

bool isFunctionalNode(const IndexNotatedTensorExpression& node){

    if(node.label == "Identity" || node.label == "zeros" || node.label == "ones" || node.label == "eps"){

        return true;
    }

    return false;
}

std::string getArgLabel(const IndexNotatedTensorExpression& node){

    if(isFunctionalNode(node)){

        return "tExpr_" + node.label + "_ord" + std::to_string(node.tensorOrder) + "_dm" + printPlainVector(node.dimensions, false, "");
    }
    else{

        return node.label;
    }
}

//
bool usingTullio = false;
bool generateDebugCall = true;

std::string IndexNotatedTensorExpression::generateTensorSequenceJuliaString(const std::vector<NotationIndex>& indexPermutation, size_t depth) const{

    //
    // RETURNING_ASSERT(indexPermutation.size() == notatedIndices.size(), "...", "");

    // Zuordnung : notierter Index <> eingesetzter Wert für notierten Index
    static std::map<int, int> indexAssignment;
    if(depth == 0 && !indexPermutation.empty()){ indexAssignment.clear(); emplaceVectorsIntoMap(indexAssignment, notatedIndices, indexPermutation); }

    static const IndexNotatedTensorExpression* prevPtr = nullptr;
    static std::map<int, int> indexDimensions;

    if(depth == 0 && this != prevPtr){

        indexDimensions.clear();
        evaluateIndexDimensions(indexDimensions);

        prevPtr = this;
    }

    //
    std::string res;

    // if(depth == 0 && usingTullio){ res += "("; }

    //
    if(isConstant){ res += string::strippedString(value); }
    else if(Relation == TkType::Argument){

        res += getArgLabel(*this) + "[";

        for(size_t i = 0; i < notatedIndices.size(); i++){

            const auto& idx = notatedIndices[i];

            if(i > 0){ res += ", "; }

            if(indexAssignment.contains(idx)){

                res += std::to_string(indexAssignment[idx] + 1);
            }
            else{

                res += "idx" + std::to_string(idx);
            }
        }

        res += "]";
    }
    else if(Relation == TkType::Operator && (Operator == IndexNotationOperator::Addition || Operator == IndexNotationOperator::Subtraction)){

        RETURNING_ASSERT(IndexNotationOperatorStrings.contains(Operator), "Unbekannter IndexNotationOperator " + std::string(magic_enum::enum_name(Operator)), "");

        //
        res += "(";

        //
        for(size_t i = 0; i < children.size(); i++){

            const IndexNotatedTensorExpression& child = children[i];

            res += child.generateTensorSequenceJuliaString({}, depth + 1);
            res += i < children.size() - 1 ? (" " + IndexNotationOperatorStrings[Operator] + " ") : "";
        }

        //
        res += ")";
    }
    else if(Relation == TkType::Operator){

        if(!IndexNotationOperatorStrings.contains(Operator)){

            RETURNING_ASSERT(TRIGGER_ASSERT, "Unbekannter IndexNotationOperator '" + std::string(magic_enum::enum_name(Operator)) + "', Node : " + toString(), "");
        }

        // unique Indices <> external Indices
        const auto& notUniqueIndices = getNotUniqueChildIndices();

        // Wichtig : nicht Unique index kann nach extern weitergereicht werden
        // zb bei der Addition

        // über elemente der notUnique Indices summieren

        for(size_t i = 0; i < notUniqueIndices.size(); i++){

            const auto& idx = notUniqueIndices[i];

            res += usingTullio ? "(" : ("sum(idx" + std::to_string(idx) + " -> ");
        }

        //
        for(size_t i = 0; i < children.size(); i++){

            const IndexNotatedTensorExpression& child = children[i];

            res += child.generateTensorSequenceJuliaString({}, depth + 1);
            res += i < children.size() - 1 ? (" " + IndexNotationOperatorStrings[Operator] + " ") : "";
        }

        for(size_t i = 0; i < notUniqueIndices.size(); i++){

            const auto& idx = notUniqueIndices[i];

            res += usingTullio ? ")" : (", 1:" + std::to_string(indexDimensions[idx]) + ")");
        }
    }
    else if(Relation == TkType::Container){

        RETURNING_ASSERT(children.size() == 1, "...","");

        switch(Operator){

            // Node Substituieren
            case IndexNotationOperator::Determinant:
            case IndexNotationOperator::Frobenius:
            case IndexNotationOperator::Inversion:{

                break;
            }
            // einfacher printout des inhalts
            default:{

                res += children.front().generateTensorSequenceJuliaString({}, depth + 1);
                break;
            }
        }
    }

    // //
    // if(depth == 0 && usingTullio){

    //     res += ") (";

    //     //
    //     auto externIndices = getNotUniqueChildIndices();

    //     for(auto it = indexDimensions.begin(); it != indexDimensions.end(); ++it) {

    //         auto [idx, rng] = *it;
    //         res += "idx" + std::to_string(idx) + " ∈ 1:" + std::to_string(rng);
    //         if(std::next(it) != indexDimensions.end()) {
    //             res += ", ";
    //         }
    //     }

    //     res += ")";
    // }

    //
    return res;
}

int g_recommedSubstitutionFromComplexity = 25;
int g_forceSubstitutionFromComplexity = 30;

std::string IndexNotatedTensorExpression::wrapTensorSequenceTullioString(const std::string& resLabel) const{

    IndexNotatedTensorExpression copy = *this;
    return copy.generateTensorSequenceTullioString(0, false, false, resLabel);
}

// std::string IndexNotatedTensorExpression::wrapTensorSequenceIDNString() const{

// }

// std::string IndexNotatedTensorExpression::generateTensorSequenceIDNString(size_t depth = 0, bool forceSubstitution, bool useTensorNotation){

// }

bool IndexNotatedTensorExpression::containsOnlyScalars() const{

    if(tensorOrder > 0){

        return false;
    }

    for(const auto& child : children){

        if(!child.containsOnlyScalars()){

            return false;
        }
    }

    return true;
}

//
int IndexNotatedTensorExpression::dependencieIdx = 0;

// Funktion sollte unter keinen umständen auf Object angewendet werden mit dem weiter gearbeitet werden soll
// dafür gibts die wrapper funktion
std::string IndexNotatedTensorExpression::generateTensorSequenceTullioString(size_t depth, bool forceSubstitution, bool useTensorNotation, const std::string& resLabel){

    // Werte so setzen dass sie Rekursive Funktion direkt beim ersten Durchlauf abbrechen
    // static int dependencieIdx = 0;
    static std::string dependencieDecls = "__INVALIDDECLS__", dependencieAssignment = "__INVALIDDECLS__";
    static bool terminate = true;

    // >> Setup der Werte für jeden einzelnen Aufruf der Funktionen für einen frischen Ausdruck
    if(depth == 0){

        // dependencieIdx = 0;
        dependencieDecls = ""; dependencieAssignment = "";
        terminate = false;
    }

    //
    std::string res = "";

    //
    if(isConstant){ res += string::strippedString(value); }
    else if(Relation == TkType::Argument){

        if(tensorOrder < 1){ res += getArgLabel(*this); }
        else{ res += getArgLabel(*this) + (useTensorNotation ? "" : ("[" + fprintPlainVector(notatedIndices, [](const NotationIndex& elem){ return "idx" + std::to_string(elem); }, false) + "]")); }
    }
    else if(Relation == TkType::Operator){

        //
        RETURNING_ASSERT(IndexNotationOperatorStrings.contains(Operator), "Unbekannter IndexNotationOperator " + std::string(magic_enum::enum_name(Operator)) + ", Node : " + toString(), "");

        //
        res += fprintPlainVector(children, [&](IndexNotatedTensorExpression& child){ return child.generateTensorSequenceTullioString(depth + 1);},
                                    true, " " + IndexNotationOperatorStrings[Operator] + " ");
    }
    else if(Relation == TkType::Container){

        RETURNING_ASSERT(children.size() == 1, "...","");

        switch(Operator){
        
            // Node Substituieren
            case IndexNotationOperator::Macaulay:
            case IndexNotationOperator::Signum:
            case IndexNotationOperator::Determinant:
            case IndexNotationOperator::Frobenius:
            case IndexNotationOperator::Sqrt:
            case IndexNotationOperator::Sin:
            case IndexNotationOperator::Cos:
            case IndexNotationOperator::Tan:
            case IndexNotationOperator::Cotan:
            case IndexNotationOperator::Inversion:{

                //
                std::string jlFuncLabel;

                if(Operator == IndexNotationOperator::Macaulay){ jlFuncLabel = "macaulay"; }
                else if(Operator == IndexNotationOperator::Signum){ jlFuncLabel = "signum"; }
                else if(Operator == IndexNotationOperator::Determinant){ jlFuncLabel = "det"; }
                else if(Operator == IndexNotationOperator::Frobenius){ jlFuncLabel = "frobenius"; }
                else if(Operator == IndexNotationOperator::Sqrt){ jlFuncLabel = "sqrt"; }
                else if(Operator == IndexNotationOperator::Sin){ jlFuncLabel = "sin"; }
                else if(Operator == IndexNotationOperator::Cos){ jlFuncLabel = "cos"; }
                else if(Operator == IndexNotationOperator::Tan){ jlFuncLabel = "tan"; }
                else if(Operator == IndexNotationOperator::Cotan){ jlFuncLabel = "cot"; }
                else{ jlFuncLabel = "inv"; }

                //
                children.front().generateTensorSequenceTullioString(depth + 1, true);

                //
                bool returnScalar = tensorOrder == 0;
                bool onlyScalars = containsOnlyScalars();
                bool useTOps = (returnScalar && !onlyScalars) || !returnScalar;

                if(Operator == IndexNotationOperator::Inversion){

                    res += (returnScalar ? "(1/" : "(inv(") + children.front().generateTensorSequenceTullioString(depth + 1, false, true) + (returnScalar ? ")" : "))");
                }
                else{

                    res += jlFuncLabel + "(" + children.front().generateTensorSequenceTullioString(depth + 1, false, true) + ")";
                }

                std::string extNodeLabel = "tmpRes_" + std::to_string(dependencieIdx++);
                int complexity = getNumOfNodes();

                //
                dependencieDecls += "\t" + extNodeLabel + " = Base.zeros(Float64, " + printPlainVector(dimensions, false) + ")\n";

                //
                // dependencieAssignment += "\n\tprintln(\"[Evaluating '" + extNodeLabel + "', Komplexität " + std::to_string(complexity) + fprintPlainVector(children, [](IndexNotatedTensorExpression& child){ return std::to_string(child.getNumOfNodes()); }) + "]\")";
                dependencieAssignment += "\n\t" + extNodeLabel + " = " + res + "\n";

                *this = asExternalNode(extNodeLabel);
                res = generateTensorSequenceTullioString(depth + 1);

                break;
            }
            default:{

                res += children.front().generateTensorSequenceTullioString(depth + 1);
                break;
            }
        }
    }

    //
    if(Relation == TkType::Operator && getNumOfNodes() > g_forceSubstitutionFromComplexity){

        int numOfNodes = getNumOfNodes();

        for(auto& child : children){

            child.generateTensorSequenceTullioString(depth + 1, true);
            if(terminate){ return ""; }
        }

        // Ausdruck konnte trotz Optimierung nicht in benätigts Format gepackt werden
        // >> unwrapped Ausdruck mit zu langen Operandenketten
        if(numOfNodes == getNumOfNodes()){

            terminate = true;
            RETURNING_ASSERT(TRIGGER_ASSERT, "Ausdruck ist für Konvertierung zu breit aufgestellt, übergebe gepackte Version an jl Skript Generierung","");
        }

        res = generateTensorSequenceTullioString(depth + 1);
    }

    if(terminate){ return ""; }

    //
    bool returnScalar = tensorOrder == 0;
    bool onlyScalars = containsOnlyScalars();
    bool useTOps = (returnScalar && !onlyScalars) || !returnScalar;

    //
    if(returnScalar && Relation == TkType::Operator){ forceSubstitution = true; }

    //
    if(depth == 0){

        res = /* "\n\tprintln(\"[Ausdruck mit " + std::to_string(dependencieIdx) + " temporären Dependencies substituiert]\")\n" + */ \
            /* dependencieDecls + "\n" + */ dependencieAssignment + "\n" + \
            /* "\tres = Base.zeros" + printPlainVector(dimensions) + */ \
            /* "\tprintln(\"[Evaluating final Result, Komplexität " + std::to_string(getNumOfNodes()) + fprintPlainVector(children, [](IndexNotatedTensorExpression& child){ return std::to_string(child.getNumOfNodes()); }) + "]\")" + */ \
            (useTOps ? "\n\t@tensor opt=true " : "\n\t") + asExternalNode(resLabel).generateTensorSequenceTullioString(1) + ((returnScalar && useTOps) ? "[]" : "") + (useTOps ? " := " : " = ") + res + "\n\n";
    }
    else if((Relation == TkType::Operator && getNumOfNodes() > g_recommedSubstitutionFromComplexity) || forceSubstitution){

        RETURNING_ASSERT(getNumOfNodes() <= g_forceSubstitutionFromComplexity, "...", "");

        std::string extNodeLabel = "tmpRes_" + std::to_string(dependencieIdx++);
        int complexity = getNumOfNodes();

        //
        dependencieDecls += "\t" + extNodeLabel + " = Base.zeros(Float64, " + printPlainVector(dimensions, false) + ")\n";

        //
        // dependencieAssignment += "\n\tprintln(\"[Evaluating '" + extNodeLabel + "', Komplexität " + std::to_string(complexity) + fprintPlainVector(children, [](IndexNotatedTensorExpression& child){ return std::to_string(child.getNumOfNodes()); }) + "]\")";
        dependencieAssignment += (useTOps ? "\n\t@tensor opt=true " : "\n\t") + asExternalNode(extNodeLabel).generateTensorSequenceTullioString(depth + 1) + ((returnScalar && useTOps) ? "[]" : "") + (useTOps ? " := " : " = ") + res;

        //
        if(returnScalar && useTOps){

            //
            std::string prevNodeLabel = "tmpRes_" + std::to_string(dependencieIdx++);
            std::swap(extNodeLabel, prevNodeLabel);
 
            dependencieAssignment += "\n\t" + extNodeLabel + " = " + prevNodeLabel + "[]";
        }

        //
        dependencieAssignment += "\n";

        //
        *this = asExternalNode(extNodeLabel);
        res = generateTensorSequenceTullioString(depth + 1);
    }

    //
    return res;
}

//
std::string IndexNotatedTensorExpression::toJuliaString(const std::string& instanceLabel, const std::vector<IndexNotatedTensorExpression>& depsKeys, const std::vector<IndexNotatedTensorExpression>& depsValues) const {

    //
    RETURNING_ASSERT(containsDimensions(), "Ohne Dimensionsberücksichtigung kann Julia Skript nicht erstellt werden","");

    // Unique External Nodes
    auto uniqueExternals = getUniqueExternalNodes();

    // Format :
    // * depsKeys ist ein Vektor mit den konditionierten externen Nodes für die ein Ausdruck substituiert wird
    // * depsVals ist die entsprechende Substitution
    //
    // Bspl.
    // DepsKeys[0] = tIdn("devSigma", ...) | DepsVals[0] = toIDN(S . (E0 . (eps0 - epsvp0))  

    //
    RETURNING_ASSERT(depsKeys.size() == depsValues.size(), "Ungleiche Listengrößen für Deps angegeben", "");

    // //
    // LOG << fprintPlainVector(uniqueExternals, [](const IndexNotatedTensorExpression* elem){ return elem -> toString(1); }) << endln;

    // Übertrag der Abhängigkeiten
    for(size_t i = 0; i < depsKeys.size(); i++){

        std::vector<const IndexNotatedTensorExpression*> uniqueDepExternals = depsValues[i].getUniqueExternalNodes();

        // //
        // LOG << fprintPlainVector(uniqueDepExternals, [](const IndexNotatedTensorExpression* elem){ return elem -> toString(1); }) << endln;

        for(auto uniqueNode : uniqueDepExternals){

            bool alreadyIn = std::find_if(uniqueExternals.begin(), uniqueExternals.end(),
                [uniqueNode](const IndexNotatedTensorExpression* existing) {
                    return areEqualExternals(*existing, *uniqueNode);
                }) != uniqueExternals.end();

            if (!alreadyIn) {
                uniqueExternals.push_back(uniqueNode);
            }
        }
    }

    // //
    // LOG << fprintPlainVector(uniqueExternals, [](const IndexNotatedTensorExpression* elem){ return elem -> toString(1); }) << endln;

    // Löschen der Substituierten aus den Abhängigkeiten
    for(size_t i = 0; i < depsKeys.size(); i++){

        // Sicherstellen das depsKey externe Node ist
        RETURNING_ASSERT(areEqualExternals(depsKeys[i].asExternalNode(depsKeys[i].label), depsKeys[i]), "Übergebene Substitution ist keine externe Node", "");

        //
        uniqueExternals.erase(
            std::remove_if(uniqueExternals.begin(), uniqueExternals.end(),
                [&](const IndexNotatedTensorExpression* existing) {
                    return areEqualExternals(*existing, depsKeys[i]);
                }),
            uniqueExternals.end()
        );
    }

    // Return string
    std::string res;

    //
    res += "# Julia Skript\n#\n";
    res += "# unique external nodes :\n";

    //
    for(const auto& node : uniqueExternals){

        res += "# | arg '" + getArgLabel(*node) + "', order [" + std::to_string(node->tensorOrder) + "], dimensions {";
        res += printPlainVector(node->dimensions, false);
        res += "}\n";
    }

    //
    for(const auto& node : depsKeys){

        res += "# | deps '" + getArgLabel(node) + "', order [" + std::to_string(node.tensorOrder) + "], dimensions {";
        res += printPlainVector(node.dimensions, false);
        res += "}\n";
    }

    res += "\n";
    res += "using LinearAlgebra\nusing TensorOperations\nusing Dates\n\n";

    // Helper functions to create precomputed tensors
    res += "function create_zeros(dims::Integer...)\n";
    res += "    if length(dims) == 0\n";
    res += "        return 0\n";
    res += "    end\n";
    res += "    return zeros(Float64, dims...)\n";
    res += "end\n\n";

    res += "function create_ones(dims::Integer...)\n";
    res += "    if length(dims) == 0\n";
    res += "        return 1\n";
    res += "    end\n";
    res += "    return ones(Float64, dims...)\n";
    res += "end\n\n";

    res += "function create_Identity(dims::Integer...)\n";
    res += "    if length(dims) == 0\n";
    res += "        return 1\n";
    res += "    end\n";
    res += "    n = dims[1]\n";
    res += "    @assert all(d -> d == n, dims) \"All dimensions must be equal for Identity\"\n";
    res += "    @assert length(dims) % 2 == 0 \"Number of dimensions must be even\"\n";
    res += "    tensor = zeros(Float64, dims...)\n";
    res += "    half = length(dims) ÷ 2\n";
    res += "    for idxs in Iterators.product(ntuple(x -> 1:n, half)...)\n";
    res += "        full_indices = (idxs..., idxs...)\n";
    res += "        tensor[full_indices...] = 1.0\n";
    res += "    end\n";
    res += "    return tensor\n";
    res += "end\n\n";

    res += "function create_eps(dims::Integer...)\n";
    res += "    # Alle Dimensionen müssen gleich sein\n";
    res += "    n = dims[1]\n";
    res += "    @assert all(d -> d == n, dims) \"All dimensions must be equal for Levi-Civita\"\n";
    res += "    @assert n == 3 \"Levi-Civita only implemented for dimension 3\"\n";
    res += "    @assert length(dims) == 3 \"Levi-Civita must be 3D tensor\"\n";
    res += "    \n";
    res += "    eps_tensor = Base.zeros(Float64, dims...)\n";
    res += "    \n";
    res += "    for i in 1:n\n";
    res += "        for j in 1:n\n";
    res += "            for k in 1:n\n";
    res += "                indices = [i, j, k]\n";
    res += "                if length(unique(indices)) != 3\n";
    res += "                    continue\n";
    res += "                end\n";
    res += "                sign = 1\n";
    res += "                for x in 1:2\n";
    res += "                    for y in x+1:3\n";
    res += "                        if indices[x] > indices[y]\n";
    res += "                            sign *= -1\n";
    res += "                        end\n";
    res += "                    end\n";
    res += "                end\n";
    res += "                eps_tensor[i, j, k] = sign\n";
    res += "            end\n";
    res += "        end\n";
    res += "    end\n";
    res += "    \n";
    res += "    return eps_tensor\n";
    res += "end\n\n";

    res += "function macaulay(x)\n";
    res += "    val = x isa AbstractArray ? x[] : x\n";
    res += "    return val > 0 ? val : 0.0\n";
    res += "end\n\n";

    res += "function signum(x)\n";
    res += "    val = x isa AbstractArray ? x[] : x\n";
    res += "    return sign(val)\n";
    res += "end\n\n";

    res += "function frobenius(A)\n";
    res += "    result = 0.0\n";
    res += "    for i in axes(A, 1)\n";
    res += "        for j in axes(A, 2)\n";
    res += "            result += A[i,j]^2\n";
    res += "        end\n";
    res += "    end\n";
    res += "    return sqrt(result)\n";
    res += "end\n\n";

    res += "const Scalar0 = Array{Float64,0}\n\n";

    res += "import Base: +, -, *, /, sqrt\n";
    res += "import Base: sin, cos, tan, cot\n\n";

    res += "+(a::Scalar0, b::Scalar0) = a[] + b[]\n";
    res += "+(a::Scalar0, b::Real)    = a[] + b\n";
    res += "+(a::Real,    b::Scalar0) = a + b[]\n\n";

    res += "-(a::Scalar0, b::Scalar0) = a[] - b[]\n";
    res += "-(a::Scalar0, b::Real)    = a[] - b\n";
    res += "-(a::Real,    b::Scalar0) = a - b[]\n";
    res += "-(a::Scalar0)             = -a[]\n\n";

    res += "*(a::Scalar0, b::Scalar0) = a[] * b[]\n";
    res += "*(a::Scalar0, b::Real)    = a[] * b\n";
    res += "*(a::Real,    b::Scalar0) = a * b[]\n\n";

    res += "/(a::Scalar0, b::Scalar0) = a[] / b[]\n";
    res += "/(a::Scalar0, b::Real)    = a[] / b\n";
    res += "/(a::Real,    b::Scalar0) = a / b[]\n\n";

    res += "sqrt(a::Scalar0) = sqrt(a[])\n";
    res += "sin(a::Scalar0)  = sin(a[])\n";
    res += "cos(a::Scalar0)  = cos(a[])\n";
    res += "tan(a::Scalar0)  = tan(a[])\n";
    res += "cot(a::Scalar0)  = cot(a[])\n\n";

    res += "det(a::Scalar0)  = a[]\n";
    res += "inv(a::Scalar0)  = 1 / a[]\n\n";

    //
    res += "\n";
    res += "function " + instanceLabel + "(";

    //
    bool filledInFirstExternal = false;

    //
    for(const auto& node : uniqueExternals){

        if(isFunctionalNode(*node)){

            continue;
        }

        res += filledInFirstExternal ? ", " + node->label : node->label;
        filledInFirstExternal = true;
    }

    // Abhängigkeiten des Indexnotierten Ausdrucks >> unique External Nodes

    res += ")\n\n";

    //
    for(const auto& node : uniqueExternals){

        if(isFunctionalNode(*node)){

            res += "\t" + getArgLabel(*node) + " = create_" + node->label + printPlainVector(node->dimensions) + "\n";
        }
        else if(node->containsDimensions() && node->dimensions.size() > 1){

            res += "\t@assert size(" + node->label + ") == " + printPlainVector(node->dimensions) + "\n";
        }
        else if(node->containsDimensions() && node->dimensions.size() == 1){

            res += "\t@assert length(" + node->label + ") == " + std::to_string(node->dimensions.front()) + "\n";
        }
        else if(node->containsDimensions() && node->dimensions.size() == 0){

            res += "\t@assert ndims(" + node->label + ") == 0\n";
        }
    }

    res += "\n";

    // // Return Wert initialisieren
    // res += "\tres = Base.zeros" + printPlainVector(dimensions) + "\n\n";

    // //
    // if(!usingTullio){

    //     //
    //     for(const auto& idxs : generateTensorIndexPermutations(dimensions)){

    //         //
    //         res += "\tres[" + printIncreasedPlainVector(idxs, false) + "] = ";

    //         // Werte der nach extern weitergereichten Indices : idxs
    //         res += generateTensorSequenceJuliaString(idxs);

    //         //
    //         res += "\n";
    //     }
    // }
    // else{

    //     //
    //     res += "\t@tensor opt=true res[";

    //     res += fprintPlainVector(notatedIndices, [](const NotationIndex& elem){ return "idx" + std::to_string(elem); }, false);

    //     // fprintPlainVector(notatedIndices, [](const NotationIndex& elem){ return "idx" + std::to_string(elem); })

    //     res += "] = ";
    //     res += generateTensorSequenceJuliaString({});

    //     //
    //     res += "\n";
    // }

    // //
    // res += "\n\treturn res\n";

    //
    dependencieIdx = 0;

    for(size_t i = 0; i < depsKeys.size(); i++){

        // depsKeys[i] | depsValues[i]
        // LOG << depsValues[i].toString() << " | " << depsValues[i].toString() << endln;
        res += depsValues[i].wrapTensorSequenceTullioString(depsKeys[i].label);
    }

    std::string resLabel = "res";
    res += wrapTensorSequenceTullioString(resLabel);
    res += "\n\n\treturn " + resLabel;
    res += "\n\n";

    //
    res += "end\n\n";

    if(generateDebugCall){

        res += "start_time = time()\nres = " + instanceLabel + "(";

        //
        for(auto it = uniqueExternals.begin(); it != uniqueExternals.end(); ){

            if(isFunctionalNode(**it)){
                it = uniqueExternals.erase(it);
            } else {
                ++it;
            }
        }

        for(auto it = uniqueExternals.begin(); it != uniqueExternals.end(); ++it) {

            auto expr = *it;

            //
            res += "rand" + printPlainVector(expr->dimensions);
            
            //
            if(std::next(it) != uniqueExternals.end()) {
                res += ", ";
            }
        }

        res +=  ")\nelapsed = time() - start_time\nprintln(\"Laufzeit: \", elapsed, \" s\")\nprintln(\"Ergebnis: \", res)";
    }

    return res;
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

        result += "]";
    }

    if(depth == 0 && containsDimensions()){

        result += "(";

        for(const auto& dim : dimensions){

            result += std::to_string(dim) + ",";
        }

        result += ")";
    }

    result += depth == 0 ? " = " : "";

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

        result += label;
    }
    else if(Relation == TkType::Container){

        result += "Container " + std::string(magic_enum::enum_name(Operator));

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


    if(depth > 0){

        result += "[";

        for(size_t i = 0; i < notatedIndices.size(); i++){

            result += std::to_string(notatedIndices[i]);
            result += i < notatedIndices.size() - 1 ? "," : "";
        }

        result += "]";
    }

    if(depth > 0 && containsDimensions()){

        result += "(";
        for(const auto& dim : dimensions){
            result += std::to_string(dim) + ",";
        }
        result += ")";
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

        os << "] | " << expr.children.size() << " childs : " << endln;

        for(const auto& child : expr.children){

            os << "| " << child << endln;
        }
    }
    else if(expr.Relation == TkType::Container){

        os << "Container | indices [";

        for(const auto& idx : expr.notatedIndices){

            os << idx << ", ";
        }

        os << "] | " << expr.children.size() << " childs : " << endln;

        for(const auto& child : expr.children){

            os << "| " << child << endln;
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
    {TensorExpressionOperator::ContractingDotProduct, &IndexNotatedTensorExpression::contractingDotProductAssign},
    {TensorExpressionOperator::CrossProduct, &IndexNotatedTensorExpression::crossProductAssign},
    {TensorExpressionOperator::DyadicProduct, &IndexNotatedTensorExpression::dyadProductAssign},
    {TensorExpressionOperator::MirroringDoubleContraction, &IndexNotatedTensorExpression::mirroringDoubleContractionAssign},
    {TensorExpressionOperator::CrossingDoubleContraction, &IndexNotatedTensorExpression::crossingDoubleContractionAssign},
    {TensorExpressionOperator::Diff, &IndexNotatedTensorExpression::diffAssign},
};

// void IndexNotatedTensorExpression::reEvaluateIndices(){

//     // if(Relation == TkType::Argument){ return; }

//     // for(auto& child : children){

//     //     child.// reEvaluateIndices();
//     // }

//     // if(Operator == IndexNotationOperator::Addition){

//     //     notatedIndices = children.front().notatedIndices;
//     //     dimensions = children.front().dimensions;
//     // }
//     // else if(Operator == IndexNotationOperator::Transposition){

//     //     notatedIndices = getUniqueChildIndices();
//     //     dimensions = getUniqueChildDimensions();
//     //     std::reverse(notatedIndices.begin(), notatedIndices.end());
//     //     std::reverse(dimensions.begin(), dimensions.end());
//     // }
//     // else{

//     //     notatedIndices = getUniqueChildIndices();
//     //     dimensions = getUniqueChildDimensions();
//     // }

//     // tensorOrder = notatedIndices.size();
// }

bool IndexNotatedTensorExpression::equals(const IndexNotatedTensorExpression& other) const{

    if(label != other.label){ return false; }

    if(notatedIndices.size() != other.notatedIndices.size()){ return false; }

    for(size_t idx = 0; idx < notatedIndices.size(); idx++){

        if(notatedIndices[idx] != other.notatedIndices[idx]){ return false; }
    }

    return true;
}

bool IndexNotatedTensorExpression::equalsWoIdx(const IndexNotatedTensorExpression& other) const{

    if(label != other.label){ return false; }

    if(notatedIndices.size() != other.notatedIndices.size()){ return false; }

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
            // else if(expr.containsDimensions()){

            //     res = IndexNotatedTensorExpression(expr.label, expr.tensorOrder, expr.dimensions);
            // }
            else{

                res = IndexNotatedTensorExpression(expr.label, expr.tensorOrder);
            }

            break;
        }
        case (TkType::Operator):{

            res = convertToIndexNotation(expr.children[0]);

            if(expr.Operator == TensorExpressionOperator::Contract){

                RETURNING_ASSERT(expr.children.size() > 1, "...", res);

                for(size_t idx = 1; idx < expr.children.size(); idx++){

                    res.contractAssign(convertToIndexNotation(expr.children[idx], depth + 1), expr.contractNIndices, expr.contractReversed);
                }
            }
            else if(operatorFunctions.contains(expr.Operator)){

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
            else if(expr.Operator == TensorExpressionOperator::Frobenius){

                res.frobeniusAssign();
            }
            else if(expr.Operator == TensorExpressionOperator::Macaulay){

                res.macaulayAssign();
            }
            else if(expr.Operator == TensorExpressionOperator::Signum){

                res.signumAssign();
            }
            else if(expr.Operator == TensorExpressionOperator::Sqrt){

                res.sqrtAssign();
            }
            else if(expr.Operator == TensorExpressionOperator::Sin){

                res.sinAssign();
            }
            else if(expr.Operator == TensorExpressionOperator::Cos){

                res.cosAssign();
            }
            else if(expr.Operator == TensorExpressionOperator::Tan){

                res.tanAssign();
            }
            else if(expr.Operator == TensorExpressionOperator::Cotan){

                res.cotanAssign();
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

    if(expr.containsDimensions()){

        res.dimensions = expr.dimensions;
    }

    return res;
}

namespace types{

    bool INDEX_NOTATED_TENSOR_EXPRESSION::setUpClass(){

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
        registerFunction("tIdn", {STRING::typeIndex, INT::typeIndex},
            [__functionLabel__ = "tidn", __numArgs__ = 2](FREG_ARGS){

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

        //
        registerMemberFunction(INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex, "getNumOfNodes", {},
            [__functionLabel__ = "getNumOfNodes", __numArgs__ = 0](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // schreiben in returns
                GET_MEMBER(INDEX_NOTATED_TENSOR_EXPRESSION);
                GET_RETURN(INT, 0);

                //
                ret0->getMember() = static_cast<int>(mb->getMember().getNumOfNodes());
        },
        {INT::typeIndex});

        //
        registerMemberFunction(INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex, "getNumOfExternalNodes", {},
            [__functionLabel__ = "getNumOfNodes", __numArgs__ = 0](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // schreiben in returns
                GET_MEMBER(INDEX_NOTATED_TENSOR_EXPRESSION);
                GET_RETURN(INT, 0);

                //
                ret0->getMember() = static_cast<int>(mb->getMember().getNumOfExternalNodes());
        },
        {INT::typeIndex});

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

        registerFunction("__contractAssign__", {INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex, INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex, INT::typeIndex, BOOL::typeIndex},
            [__functionLabel__ = "__contractAssign__", __numArgs__ = 4](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(INDEX_NOTATED_TENSOR_EXPRESSION, 0); GET_ARG(INDEX_NOTATED_TENSOR_EXPRESSION, 1);
                GET_ARG(INT, 2); GET_ARG(BOOL, 3);

                IndexNotatedTensorExpression& member0 = arg0->getMember();
                IndexNotatedTensorExpression& member1 = arg1->getMember();

                member0.contractAssign(member1, arg2->getMember(), arg3->getMember());
        },
        {});

        registerFunction("contract", {INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex, INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex, INT::typeIndex, BOOL::typeIndex},
            [__functionLabel__ = "contract", __numArgs__ = 4](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(INDEX_NOTATED_TENSOR_EXPRESSION, 0); GET_ARG(INDEX_NOTATED_TENSOR_EXPRESSION, 1);
                GET_ARG(INT, 2); GET_ARG(BOOL, 3);

                IndexNotatedTensorExpression& member0 = arg0->getMember();
                IndexNotatedTensorExpression& member1 = arg1->getMember();

                returns[0].constructRValueByObject(inputs[0]->getData()->clone().release());

                GET_RETURN(INDEX_NOTATED_TENSOR_EXPRESSION, 0);
                ret0->getMember().contractAssign(member1, arg2->getMember(), arg3->getMember());
        },
        {INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex});

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

        //
        registerMemberFunction(INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex, "toJuliaString", {},
            [__functionLabel__ = "toJuliaString", __numArgs__ = 0](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_MEMBER(INDEX_NOTATED_TENSOR_EXPRESSION);
                GET_RETURN(STRING, 0);

                ret0->getMember() = mb->getMember().toJuliaString();
        },
        {STRING::typeIndex});

        //
        registerMemberFunction(INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex, "toJuliaString", {STRING::typeIndex},
            [__functionLabel__ = "toJuliaString", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_MEMBER(INDEX_NOTATED_TENSOR_EXPRESSION);
                GET_ARG(STRING, 0); GET_RETURN(STRING, 0);

                ret0->getMember() = mb->getMember().toJuliaString(arg0->getMember());
        },
        {STRING::typeIndex});

        //
        registerMemberFunction(INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex, "toJuliaString", {STRING::typeIndex, ARGS::typeIndex, ARGS::typeIndex},
            [__functionLabel__ = "toJuliaString", __numArgs__ = 3](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_MEMBER(INDEX_NOTATED_TENSOR_EXPRESSION);
                GET_ARG(STRING, 0); GET_ARG(ARGS, 1); GET_ARG(ARGS, 2); GET_RETURN(STRING, 0);

                //
                RETURNING_ASSERT(arg1->getMember().size() == arg2->getMember().size(), "Übergebene Listen DependencieKeys und -Vals haben ungleiche Länge",);

                // DependencieKeys

                // EvalResultVec zu IndexNotatedTensorExpressionVec parsen
                std::vector<IndexNotatedTensorExpression> depsKeys = {};
                depsKeys.reserve(arg1->getMember().size());

                for(size_t i = 0; i < arg1->getMember().size(); i++){

                    RETURNING_ASSERT(arg1->getMember().at(i).getVariableRef().getData()->getTypeIndex() == INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex, "Input ist kein Indexnotierter Ausdruck",);
                    depsKeys.emplace_back(static_cast<INDEX_NOTATED_TENSOR_EXPRESSION*>(arg1->getMember().at(i).getVariableRef().getData())->getMember());
                }

                // DependencieVals

                // EvalResultVec zu IndexNotatedTensorExpressionVec parsen
                std::vector<IndexNotatedTensorExpression> depsVals = {};
                depsVals.reserve(arg2->getMember().size());

                for(size_t i = 0; i < arg1->getMember().size(); i++){

                    RETURNING_ASSERT(arg2->getMember().at(i).getVariableRef().getData()->getTypeIndex() == INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex, "Input ist kein Indexnotierter Ausdruck",);
                    depsVals.emplace_back(static_cast<INDEX_NOTATED_TENSOR_EXPRESSION*>(arg2->getMember().at(i).getVariableRef().getData())->getMember());
                }

                ret0->getMember() = mb->getMember().toJuliaString(arg0->getMember(), depsKeys, depsVals);
        },
        {STRING::typeIndex});


        //
        registerMemberFunction(INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex, "toFortranString", {},
            [__functionLabel__ = "toFortranString", __numArgs__ = 0](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_MEMBER(INDEX_NOTATED_TENSOR_EXPRESSION);
                GET_RETURN(STRING, 0);

                ret0->getMember() = mb->getMember().toFortranString();
        },
        {STRING::typeIndex});

        //
        registerMemberFunction(INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex, "toFortranString", {STRING::typeIndex},
            [__functionLabel__ = "toFortranString", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_MEMBER(INDEX_NOTATED_TENSOR_EXPRESSION);
                GET_ARG(STRING, 0); GET_RETURN(STRING, 0);

                ret0->getMember() = mb->getMember().toFortranString(arg0->getMember());
        },
        {STRING::typeIndex});

        //
        registerMemberFunction(INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex, "toFortranString", {STRING::typeIndex, ARGS::typeIndex, ARGS::typeIndex},
            [__functionLabel__ = "toFortranString", __numArgs__ = 3](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_MEMBER(INDEX_NOTATED_TENSOR_EXPRESSION);
                GET_ARG(STRING, 0); GET_ARG(ARGS, 1); GET_ARG(ARGS, 2); GET_RETURN(STRING, 0);

                //
                RETURNING_ASSERT(arg1->getMember().size() == arg2->getMember().size(), "Übergebene Listen DependencieKeys und -Vals haben ungleiche Länge",);

                // DependencieKeys

                // EvalResultVec zu IndexNotatedTensorExpressionVec parsen
                std::vector<IndexNotatedTensorExpression> depsKeys = {};
                depsKeys.reserve(arg1->getMember().size());

                for(size_t i = 0; i < arg1->getMember().size(); i++){

                    RETURNING_ASSERT(arg1->getMember().at(i).getVariableRef().getData()->getTypeIndex() == INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex, "Input ist kein Indexnotierter Ausdruck",);
                    depsKeys.emplace_back(static_cast<INDEX_NOTATED_TENSOR_EXPRESSION*>(arg1->getMember().at(i).getVariableRef().getData())->getMember());
                }

                // DependencieVals

                // EvalResultVec zu IndexNotatedTensorExpressionVec parsen
                std::vector<IndexNotatedTensorExpression> depsVals = {};
                depsVals.reserve(arg2->getMember().size());

                for(size_t i = 0; i < arg1->getMember().size(); i++){

                    RETURNING_ASSERT(arg2->getMember().at(i).getVariableRef().getData()->getTypeIndex() == INDEX_NOTATED_TENSOR_EXPRESSION::typeIndex, "Input ist kein Indexnotierter Ausdruck",);
                    depsVals.emplace_back(static_cast<INDEX_NOTATED_TENSOR_EXPRESSION*>(arg2->getMember().at(i).getVariableRef().getData())->getMember());
                }

                ret0->getMember() = mb->getMember().toFortranString(arg0->getMember(), depsKeys, depsVals);
        },
        {STRING::typeIndex});

        //
        registerFunction("setGenerateExportDebugCall", {BOOL::typeIndex},
            [__functionLabel__ = "setGenerateExportDebugCall", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(BOOL, 0);

                generateDebugCall = arg0->getMember();
        },
        {});

        
        //
        registerFunction("setGenerateExportHelpers", {BOOL::typeIndex},
            [__functionLabel__ = "setGenerateExportDebugCall", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(BOOL, 0);

                generateExportHelpers = arg0->getMember();
        },
        {});

        return true;
    }
}

std::vector<NotationIndex> IndexNotatedTensorExpression::collectAllContainedIndices() const {

    std::vector<NotationIndex> result;

    // Hilfsfunktion, fügt einen Index nur hinzu, falls er noch nicht enthalten ist
    auto addUnique = [&result](const NotationIndex& idx) {

        bool alreadyContained = false;
        for (const auto& existing : result) {

            if (existing == idx) {
                alreadyContained = true;
                break;
            }
        }
        if (!alreadyContained) {
            result.push_back(idx);
        }
    };

    // Eigene Indizes hinzufügen
    for (const auto& idx : notatedIndices) {
        addUnique(idx);
    }

    // Rekursiv Indizes aller Kinder zufügen
    for (const auto& child : children) {
        std::vector<NotationIndex> childIndices = child.collectAllContainedIndices();
        for (const auto& idx : childIndices) {
            addUnique(idx);
        }
    }

    return result;
}

// Fortran Export

std::string IndexNotatedTensorExpression::wrapTensorSequenceFortranString(const std::string& resLabel) const{

    IndexNotatedTensorExpression copy = *this;
    return copy.generateTensorSequenceFortranString(0, false, false, resLabel);
}

//
std::string g_fortranDependencieDecls = "C     __DECLS__\n\n";
std::string g_fortranDependencieAssignments = "C     __Assignment__\n\n";

//
bool generateExportHelpers = true;

std::string fortranApplyDims(const IndexNotatedTensorExpression& member, const std::string& dimsVarName = "DIMS"){

    std::string res;

    for(size_t i = 0; i < member.dimensions.size(); i++){

        res += "      " + dimsVarName + "(" + std::to_string(i+1) + ") = " + std::to_string(member.dimensions[i]) + "\n";
    }

    res += "\n";

    res += "      RANK = " + std::to_string(member.tensorOrder) + "\n";
    res += "\n";

    return res;
};

// Funktion sollte unter keinen umständen auf Object angewendet werden mit dem weiter gearbeitet werden soll
// dafür gibts die wrapper funktion
std::string IndexNotatedTensorExpression::generateTensorSequenceFortranString(size_t depth, bool forceSubstitution, bool useTensorNotation, const std::string& resLabel){

    // Werte so setzen dass sie Rekursive Funktion direkt beim ersten Durchlauf abbrechen
    // static int dependencieIdx = 0;
    static std::string dependencieDecls = "C     __DECLS__", dependencieAssignment = "C     __Assignments__";
    static bool terminate = true;

    // >> Setup der Werte für jeden einzelnen Aufruf der Funktionen für einen frischen Ausdruck
    if(depth == 0){

        // dependencieIdx = 0;
        dependencieDecls = ""; dependencieAssignment = "";
        terminate = false;
    }

    //
    std::string res = "";
    std::string insertion = "";

    //
    size_t loopContent = 0;

    //
    std::vector<NotationIndex> uniqueIndices = getUniqueChildIndices();
    std::vector<int> uniqueDimensions = getUniqueChildDimensions();

    //
    std::vector<NotationIndex> notUniqueIndices = getNotUniqueChildIndices();
    std::vector<int> notUniqueDimensions = getNotUniqueChildDimensions();

    //
    std::vector<NotationIndex> sortedIndices = getSortedIndices();

    if(Relation == TkType::Operator && (Operator == IndexNotationOperator::Addition || Operator == IndexNotationOperator::Subtraction)){

        uniqueIndices = notUniqueIndices;
        uniqueDimensions = notUniqueDimensions;
        notUniqueIndices = {};
        notUniqueDimensions = {};
    }

    if(Relation == TkType::Container && Operator == IndexNotationOperator::Inversion){

        uniqueIndices = sortedIndices;
        uniqueDimensions = dimensions;
        notUniqueIndices = {};
        notUniqueDimensions = {};
    }

    if(resLabel != ""){

        //
        res += "\n\n";

        // //
        // res += "C     Finishing Evaluation of " + resLabel + "\n\n";

        // Aeussere Schleifen: freie (einzigartige) Indizes
        for (size_t i = 0; i < uniqueIndices.size(); i++) {

            insertion = "      DO IDX_" + std::to_string(uniqueIndices[i]) + " = 1, " +
                        std::to_string(uniqueDimensions[i]) + "\n\n";

            res.insert(loopContent, insertion);
            loopContent += insertion.size();

            res.insert(loopContent, "\n      END DO\n");
        }

        if(tensorOrder == 0){

            insertion = "      " + resLabel + " = 0\n\n";
        }
        else{

            insertion = "      " + resLabel + fprintPlainVector(sortedIndices, [](const NotationIndex& elem){ return "IDX_" + std::to_string(elem); }) + " = 0\n\n";
        }

        //
        res.insert(loopContent, insertion);
        loopContent += insertion.size();

        // Innere Schleifen: kontrahierte (nicht-einzigartige) Indizes
        for (size_t i = 0; i < notUniqueIndices.size(); i++) {

            insertion = "      DO IDX_" + std::to_string(notUniqueIndices[i]) + " = 1, " +
                        std::to_string(notUniqueDimensions[i]) + "\n\n";

            res.insert(loopContent, insertion);
            loopContent += insertion.size();

            res.insert(loopContent, "\n      END DO\n");
        }

        if(tensorOrder == 0){

            insertion = "      " + resLabel + " = " + resLabel + "\n     & + (";
        }
        else{

            insertion = "      " + resLabel + fprintPlainVector(sortedIndices, [](const NotationIndex& elem){ return "IDX_" + std::to_string(elem); }) + "\n     & = " + \
                                    resLabel + fprintPlainVector(sortedIndices, [](const NotationIndex& elem){ return "IDX_" + std::to_string(elem); }) + "\n     & + (";
        }

        //
        res.insert(loopContent, insertion);
        loopContent += insertion.size();

        //
        res.insert(loopContent, generateTensorSequenceFortranString(depth + 1, forceSubstitution, useTensorNotation, "") + ")");

        //
        if(depth == 0){

            res.insert(0, dependencieAssignment);
        }

        return res;
    }
    else if(resLabel != ""){

        // läuft bis in die Behandlung von Inversion, det, etc. 
    }

    //
    bool childContainingNoAdditionalInternalIndices = false;
    std::vector<NotationIndex> internalChildIndices;

    // Childs nach internen Indices absuchen
    for(auto& child : children){

        // Interne Indices im Child (Indices über die die Evaluierung für das child eigenständig summieren muss)
        internalChildIndices = child.getNotUniqueChildIndices();

        // Überprüft, ob es interne child indices im child gibt, die nicht in den indices des parents verfügbar sind
        // >> nicht exportierte Indices müssen bei der Evaluierung des child intern summiert werden
        childContainingNoAdditionalInternalIndices = std::includes(
            sortedIndices.begin(), sortedIndices.end(),
            internalChildIndices.begin(), internalChildIndices.end()
        );

        //
        if(!childContainingNoAdditionalInternalIndices){

            //
            child.generateTensorSequenceFortranString(depth + 1, true, useTensorNotation, "");
        }
    }

    if(forceSubstitution){

        // Label der substituierten Dependencie
        std::string tmpResLabel = "tmpRes" + std::to_string(dependencieIdx++);

        // Predecls für substituierte Dependencie
        if(tensorOrder == 0){ g_fortranDependencieDecls += "      REAL*8 " + tmpResLabel; }
        else{ g_fortranDependencieDecls += "      REAL*8 " + tmpResLabel + printPlainVector(dimensions); }

        g_fortranDependencieDecls += "\n\n";

        // Zuweisung / Berechnung der substituierten Dependencie
        dependencieAssignment += "C     Eval Dependencie " + tmpResLabel + "\n\n" + generateTensorSequenceFortranString(depth + 1, false, useTensorNotation, tmpResLabel);

        //
        *this = this -> asExternalNode(tmpResLabel);
    }

    if(isConstant){ res += string::strippedString(value); }
    else if(Relation == TkType::Argument && (tensorOrder == 0 || useTensorNotation)){

        res += getArgLabel(*this);
    }
    else if(Relation == TkType::Argument && tensorOrder > 0){

        res += getArgLabel(*this) + fprintPlainVector(notatedIndices, [](const NotationIndex& elem){ return "IDX_" + std::to_string(elem); });
    }
    else if(Relation == TkType::Operator && Operator == IndexNotationOperator::Multiplication){

        res += fprintPlainVector(children, [&](IndexNotatedTensorExpression& elem){
            return elem.generateTensorSequenceFortranString(depth + 1, forceSubstitution, useTensorNotation, ""); },
            true, " \n     & * ");
    }
    else if(Relation == TkType::Operator && Operator == IndexNotationOperator::Addition){

        res += fprintPlainVector(children, [&](IndexNotatedTensorExpression& elem){
            return elem.generateTensorSequenceFortranString(depth + 1, forceSubstitution, useTensorNotation, ""); },
            true, " \n     & + ");
    }
    else if(Relation == TkType::Operator && Operator == IndexNotationOperator::Subtraction){

        res += fprintPlainVector(children, [&](IndexNotatedTensorExpression& elem){
            return elem.generateTensorSequenceFortranString(depth + 1, forceSubstitution, useTensorNotation, ""); },
            true, " \n     & + ");
    }
    else if(Relation == TkType::Container){

        RETURNING_ASSERT(children.size() == 1, "...","");

        switch(Operator){
        
            // Node Substituieren
            case IndexNotationOperator::Macaulay:
            case IndexNotationOperator::Signum:
            case IndexNotationOperator::Sqrt:
            case IndexNotationOperator::Sin:
            case IndexNotationOperator::Cos:
            case IndexNotationOperator::Tan:
            case IndexNotationOperator::Cotan:{
                
                //
                RETURNING_ASSERT(tensorOrder == 0, "...","");

                //
                std::string FuncLabel;

                if(Operator == IndexNotationOperator::Macaulay){ FuncLabel = "MACAULAY"; }
                else if(Operator == IndexNotationOperator::Signum){ FuncLabel = "SIGNUM"; }
                else if(Operator == IndexNotationOperator::Sqrt){ FuncLabel = "SQRT"; }
                else if(Operator == IndexNotationOperator::Sin){ FuncLabel = "SIN"; }
                else if(Operator == IndexNotationOperator::Cos){ FuncLabel = "COS"; }
                else if(Operator == IndexNotationOperator::Tan){ FuncLabel = "TAN"; }
                else if(Operator == IndexNotationOperator::Cotan){ FuncLabel = "COTAN"; }

                //
                std::string callArgs = children.front().generateTensorSequenceFortranString(depth + 1, false, useTensorNotation, "");
                bool areCallArgsWrappedInParens = string::startsWith(callArgs, "(") && string::endsWith(callArgs, ")"); 

                //
                res += FuncLabel + (areCallArgsWrappedInParens ? "" : "(") + callArgs + (areCallArgsWrappedInParens ? "" : ")"); 

                break;
            }
            // Operationen in deren zugehörige Funktionen die Tensoren eingesetzt werden, nicht die Tensoreinträge
            case IndexNotationOperator::Determinant:
            case IndexNotationOperator::Frobenius:
            case IndexNotationOperator::Inversion:{
        
                // Inhalt (1 Node) wird substituiert
                children.front().generateTensorSequenceFortranString(depth + 1, false, false, "");

                //
                std::string tmpResLabel = "tmpRes" + std::to_string(dependencieIdx++);

                // Predecls für substituierte Dependencie
                if(tensorOrder == 0){ g_fortranDependencieDecls += "      REAL*8 " + tmpResLabel; }
                else{ g_fortranDependencieDecls += "      REAL*8 " + tmpResLabel + printPlainVector(dimensions); }

                g_fortranDependencieDecls += "\n\n";

                // Zuweisung / Berechnung der substituierten Dependencie
                dependencieAssignment += "C     Eval Dependencie " + tmpResLabel + "\n\n";

                //
                dependencieAssignment += fortranApplyDims(*this);

                if(Operator == IndexNotationOperator::Inversion && tensorOrder == 0){

                    dependencieAssignment += "      CALL INVERT_SCALAR(" + children.front().label + ", " + tmpResLabel + ")\n\n";
                }
                else if(Operator == IndexNotationOperator::Inversion){
                    
                    dependencieAssignment += "      CALL INVERT_TENSOR(" + children.front().label + ", " + tmpResLabel + ", DIMS, RANK)\n\n";
                }
                else if(Operator == IndexNotationOperator::Determinant){
                    
                    dependencieAssignment += "      CALL DET_TENSOR(" + children.front().label + ", " + tmpResLabel + ", DIMS, RANK)\n\n";
                }
                else if(Operator == IndexNotationOperator::Frobenius){
                    
                    dependencieAssignment += "      CALL FROBENIUS(" + children.front().label + ", " + tmpResLabel + ", RANK)\n\n";
                }

                //
                *this = this -> asExternalNode(tmpResLabel);

                return generateTensorSequenceFortranString(depth + 1, forceSubstitution, useTensorNotation, "");
                break;
            }

            default:{

                res += children.front().generateTensorSequenceFortranString(depth + 1, false, useTensorNotation, "");
                break;
            }
        }
    }

    return res;
}

// Erzeugtes Skript testen per 'gfortran -g -Wall -std=legacy -ffixed-form -o Examples/fortranScripts/tangente.exe Examples/fortranScripts/evalSigma.f'
std::string IndexNotatedTensorExpression::toFortranString(const std::string& instanceLabel, const std::vector<IndexNotatedTensorExpression>& depsKeys, const std::vector<IndexNotatedTensorExpression>& depsValues) const {

    //
    RETURNING_ASSERT(containsDimensions(), "Ohne Dimensionsberücksichtigung kann Fortran Skript nicht erstellt werden","");

    // Unique External Nodes
    auto uniqueExternals = getUniqueExternalNodes();

    // Format :
    // * depsKeys ist ein Vektor mit den konditionierten externen Nodes für die ein Ausdruck substituiert wird
    // * depsVals ist die entsprechende Substitution
    //
    // Bspl.
    // DepsKeys[0] = tIdn("devSigma", ...) | DepsVals[0] = toIDN(S . (E0 . (eps0 - epsvp0))  

    //
    RETURNING_ASSERT(depsKeys.size() == depsValues.size(), "Ungleiche Listengrößen für Deps angegeben", "");

    // //
    // LOG << fprintPlainVector(uniqueExternals, [](const IndexNotatedTensorExpression* elem){ return elem -> toString(1); }) << endln;

    // Übertrag der Abhängigkeiten
    for(size_t i = 0; i < depsKeys.size(); i++){

        std::vector<const IndexNotatedTensorExpression*> uniqueDepExternals = depsValues[i].getUniqueExternalNodes();

        // //
        // LOG << fprintPlainVector(uniqueDepExternals, [](const IndexNotatedTensorExpression* elem){ return elem -> toString(1); }) << endln;

        for(auto uniqueNode : uniqueDepExternals){

            bool alreadyIn = std::find_if(uniqueExternals.begin(), uniqueExternals.end(),
                [uniqueNode](const IndexNotatedTensorExpression* existing) {
                    return areEqualExternals(*existing, *uniqueNode);
                }) != uniqueExternals.end();

            if (!alreadyIn) {
                uniqueExternals.push_back(uniqueNode);
            }
        }
    }

    // //
    // LOG << fprintPlainVector(uniqueExternals, [](const IndexNotatedTensorExpression* elem){ return elem -> toString(1); }) << endln;

    // Löschen der Substituierten aus den Abhängigkeiten
    for(size_t i = 0; i < depsKeys.size(); i++){

        // Sicherstellen das depsKey externe Node ist
        RETURNING_ASSERT(areEqualExternals(depsKeys[i].asExternalNode(depsKeys[i].label), depsKeys[i]), "Übergebene Substitution ist keine externe Node", "");

        //
        uniqueExternals.erase(
            std::remove_if(uniqueExternals.begin(), uniqueExternals.end(),
                [&](const IndexNotatedTensorExpression* existing) {
                    return areEqualExternals(*existing, depsKeys[i]);
                }),
            uniqueExternals.end()
        );
    }

    // Unique Externals sind sortiert

    // Extrahieren der generierten Nodes (Identity/zeros/ones/eps)
    // aus den uniqueExternals -> die werden NICHT als Argument
    // uebergeben, sondern in der Subroutine selbst erzeugt
    std::vector<const IndexNotatedTensorExpression*> generatedDeps;

    for(auto it = uniqueExternals.begin(); it != uniqueExternals.end(); ){

        if(isFunctionalNode(**it)){
            generatedDeps.push_back(*it);
            it = uniqueExternals.erase(it);
        } else {
            ++it;
        }
    }

    // Aussortieren der Konstanten
    for(auto it = uniqueExternals.begin(); it != uniqueExternals.end(); ){

        if((**it).isConstant){
            it = uniqueExternals.erase(it);
        } else {
            ++it;
        }
    }

    // Return string
    std::string res;

    //
    res += "C      Fortran Skript, generated with Baldur\nC\n";
    res += "C      unique external nodes :\n";

    //
    for(const auto& node : uniqueExternals){

        res += "C      | arg '" + getArgLabel(*node) + "', order [" + std::to_string(node->tensorOrder) + "], dimensions {";
        res += printPlainVector(node->dimensions, false);
        res += "}\n";
    }

    //
    for(const auto& node : generatedDeps){

        res += "C      | extConst '" + getArgLabel(*node) + "', order [" + std::to_string(node->tensorOrder) + "], dimensions {";
        res += printPlainVector(node->dimensions, false);
        res += "}\n";
    }

    //
    for(const auto& node : depsKeys){

        res += "C      | deps '" + getArgLabel(node) + "', order [" + std::to_string(node.tensorOrder) + "], dimensions {";
        res += printPlainVector(node.dimensions, false);
        res += "}\n";
    }

    if(generateExportHelpers){

        res += "\n";
        res += "C     Gibt die Groeße des 1D-Arrays zurück,\n";
        res += "C     dass über die Dimensionen projiziert wird\n";
        res += "      INTEGER FUNCTION TENS_SIZE(DIMS, RANK)\n";
        res += "      IMPLICIT REAL*8 (A-H,O-Z)\n";
        res += "      INTEGER DIMS(*), RANK, I, S\n";
        res += "      S = 1\n";
        res += "      DO 10 I = 1, RANK\n";
        res += "        S = S * DIMS(I)\n";
        res += "   10 CONTINUE\n";
        res += "      TENS_SIZE = S\n";
        res += "      RETURN\n";
        res += "      END\n";
        res += "\n";
        res += "C     Mapt Indices pro Dimension auf Index in\n";
        res += "C     1D-Array\n";
        res += "      INTEGER FUNCTION TENS_INDEX(DIMS, RANK, IDX)\n";
        res += "      IMPLICIT REAL*8 (A-H,O-Z)\n";
        res += "      INTEGER DIMS(*), RANK, IDX(*), I, S, STRIDE\n";
        res += "      S = 1\n";
        res += "      STRIDE = 1\n";
        res += "      DO 10 I = 1, RANK\n";
        res += "        S = S + (IDX(I)-1)*STRIDE\n";
        res += "        STRIDE = STRIDE*DIMS(I)\n";
        res += "   10 CONTINUE\n";
        res += "      TENS_INDEX = S\n";
        res += "      RETURN\n";
        res += "      END\n";
        res += "\n";
        res += "C     Erstellt einen Nulltensor in uebergenem\n";
        res += "C     Tensor 'A' für uebergebenen Dimension\n";
        res += "C     und Ordnung\n";
        res += "      SUBROUTINE CREATE_ZEROS(A, DIMS, RANK)\n";
        res += "      IMPLICIT REAL*8 (A-H,O-Z)\n";
        res += "      INTEGER DIMS(*), RANK, N, I\n";
        res += "      INTEGER TENS_SIZE\n";
        res += "      REAL*8 A(*)\n";
        res += "      N = TENS_SIZE(DIMS, RANK)\n";
        res += "      DO 10 I = 1, N\n";
        res += "        A(I) = 0.0D0\n";
        res += "   10 CONTINUE\n";
        res += "      RETURN\n";
        res += "      END\n";
        res += "\n";
        res += "C     Erstellt einen Einsentensor in uebergenem\n";
        res += "C     Tensor 'A' für uebergebenen Dimension\n";
        res += "C     und Ordnung\n";
        res += "      SUBROUTINE CREATE_ONES(A, DIMS, RANK)\n";
        res += "      IMPLICIT REAL*8 (A-H,O-Z)\n";
        res += "      INTEGER DIMS(*), RANK, N, I\n";
        res += "      INTEGER TENS_SIZE\n";
        res += "      REAL*8 A(*)\n";
        res += "      N = TENS_SIZE(DIMS, RANK)\n";
        res += "      DO 10 I = 1, N\n";
        res += "        A(I) = 1.0D0\n";
        res += "   10 CONTINUE\n";
        res += "      RETURN\n";
        res += "      END\n";
        res += "\n";
        res += "C     Erstellt einen Einheitstensor in uebergenem\n";
        res += "C     Tensor 'A' für uebergebenen Dimension\n";
        res += "C     und Ordnung\n";
        res += "      SUBROUTINE CREATE_IDENTITY(A, DIMS, RANK)\n";
        res += "      IMPLICIT REAL*8 (A-H,O-Z)\n";
        res += "      INTEGER MAXR\n";
        res += "      PARAMETER (MAXR=8)\n";
        res += "      REAL*8 A(*)\n";
        res += "      INTEGER DIMS(*), RANK, N, K, NTOT, I, M, POS\n";
        res += "      INTEGER TENS_SIZE\n";
        res += "      INTEGER IDX(MAXR), POW(MAXR)\n";
        res += "      N = DIMS(1)\n";
        res += "      K = RANK/2\n";
        res += "      NTOT = TENS_SIZE(DIMS, RANK)\n";
        res += "      DO 10 I = 1, NTOT\n";
        res += "        A(I) = 0.0D0\n";
        res += "   10 CONTINUE\n";
        res += "      POW(1) = 1\n";
        res += "      DO 20 M = 2, RANK\n";
        res += "        POW(M) = POW(M-1)*N\n";
        res += "   20 CONTINUE\n";
        res += "      DO 30 I = 1, K\n";
        res += "        IDX(I) = 1\n";
        res += "   30 CONTINUE\n";
        res += "   40 CONTINUE\n";
        res += "        POS = 1\n";
        res += "        DO 50 M = 1, K\n";
        res += "          POS = POS + (IDX(M)-1)*(POW(M)+POW(K+M))\n";
        res += "   50   CONTINUE\n";
        res += "        A(POS) = 1.0D0\n";
        res += "        M = 1\n";
        res += "   60   CONTINUE\n";
        res += "        IDX(M) = IDX(M) + 1\n";
        res += "        IF (IDX(M) .LE. N) GOTO 40\n";
        res += "        IDX(M) = 1\n";
        res += "        M = M + 1\n";
        res += "        IF (M .LE. K) GOTO 60\n";
        res += "      RETURN\n";
        res += "      END\n";
        res += "\n";
        res += "C     Erstellt einen Levi-Civita-Tensor in uebergenem\n";
        res += "C     Tensor 'A' für uebergebenen Dimension\n";
        res += "C     und Ordnung\n";
        res += "      SUBROUTINE CREATE_EPS(A)\n";
        res += "      IMPLICIT REAL*8 (A-H,O-Z)\n";
        res += "      REAL*8 A(*)\n";
        res += "      INTEGER I, J, K, POS, SGN\n";
        res += "      DO 10 I = 1, 27\n";
        res += "        A(I) = 0.0D0\n";
        res += "   10 CONTINUE\n";
        res += "      DO 20 I = 1, 3\n";
        res += "        DO 30 J = 1, 3\n";
        res += "          DO 40 K = 1, 3\n";
        res += "            POS = 1 + (I-1) + (J-1)*3 + (K-1)*9\n";
        res += "            SGN = (I-J)*(J-K)*(K-I)/2\n";
        res += "            A(POS) = DBLE(SGN)\n";
        res += "   40     CONTINUE\n";
        res += "   30   CONTINUE\n";
        res += "   20 CONTINUE\n";
        res += "      RETURN\n";
        res += "      END\n";
        res += "\n";
        res += "C     Erstellt in uebergenem Tensor 'B'\n";
        res += "C     die Macaulynormen der Elemente von 'A'\n";
        res += "C     fuer 'N' Elemente\n";
        res += "      SUBROUTINE ELWISE_MACAULAY(A, B, N)\n";
        res += "      IMPLICIT REAL*8 (A-H,O-Z)\n";
        res += "      REAL*8 A(*), B(*)\n";
        res += "      INTEGER N, I\n";
        res += "      DO 10 I = 1, N\n";
        res += "        IF (A(I) .GT. 0.0D0) THEN\n";
        res += "          B(I) = A(I)\n";
        res += "        ELSE\n";
        res += "          B(I) = 0.0D0\n";
        res += "        END IF\n";
        res += "   10 CONTINUE\n";
        res += "      RETURN\n";
        res += "      END\n";
        res += "\n";
        res += "C     Erstellt in uebergenem Tensor 'B'\n";
        res += "C     die Signumnormen der Elemente von 'A'\n";
        res += "C     fuer 'N' Elemente\n";
        res += "      SUBROUTINE ELWISE_SIGNUM(A, B, N)\n";
        res += "      IMPLICIT REAL*8 (A-H,O-Z)\n";
        res += "      REAL*8 A(*), B(*)\n";
        res += "      INTEGER N, I\n";
        res += "      DO 10 I = 1, N\n";
        res += "        IF (A(I) .GT. 0.0D0) THEN\n";
        res += "          B(I) = 1.0D0\n";
        res += "        ELSE IF (A(I) .LT. 0.0D0) THEN\n";
        res += "          B(I) = -1.0D0\n";
        res += "        ELSE\n";
        res += "          B(I) = 0.0D0\n";
        res += "        END IF\n";
        res += "   10 CONTINUE\n";
        res += "      RETURN\n";
        res += "      END\n";
        res += "\n";
        res += "C     Erstellt in uebergenem Tensor 'B'\n";
        res += "C     die Frobeniusnorm von 'A'\n";
        res += "C     fuer 'N' Elemente\n";
        res += "      SUBROUTINE FROBENIUS(A, B, N)\n";
        res += "      IMPLICIT REAL*8 (A-H,O-Z)\n";
        res += "      REAL*8 A(*), B(*)\n";
        res += "      INTEGER N, I\n";
        res += "      REAL*8 S\n";
        res += "\n";
        res += "      S = 0.0D0\n";
        res += "\n";
        res += "      DO 10 I = 1, N\n";
        res += "        S = S + A(I)**2\n";
        res += "   10 CONTINUE\n";
        res += "\n";
        res += "      B(1) = DSQRT(S)\n";
        res += "\n";
        res += "      RETURN\n";
        res += "      END\n";
        res += "\n";
        res += "C     gibt einen Tensor vollstaendig aus\n";
        res += "      SUBROUTINE PRINT_TENSOR(A, DIMS, RANK, NAME)\n";
        res += "      IMPLICIT REAL*8 (A-H,O-Z)\n";
        res += "      CHARACTER*(*) NAME\n";
        res += "      REAL*8 A(*)\n";
        res += "      INTEGER DIMS(*), RANK, N, I, COL\n";
        res += "      INTEGER TENS_SIZE\n";
        res += "      INTEGER PERLINE\n";
        res += "      PARAMETER (PERLINE=6)\n";
        res += "      N = TENS_SIZE(DIMS, RANK)\n";
        res += "      WRITE(*,*) NAME, '  rank=', RANK, ' dims=(',\n";
        res += "     &            (DIMS(I), I=1,RANK), ')  N=', N\n";
        res += "      COL = 0\n";
        res += "      DO 10 I = 1, N\n";
        res += "        WRITE(*,'(1X,ES13.5,$)') A(I)\n";
        res += "        COL = COL + 1\n";
        res += "        IF (COL .GE. PERLINE) THEN\n";
        res += "          WRITE(*,*)\n";
        res += "          COL = 0\n";
        res += "        END IF\n";
        res += "   10 CONTINUE\n";
        res += "      IF (COL .GT. 0) WRITE(*,*)\n";
        res += "      RETURN\n";
        res += "      END\n\n\n";
        res += "\n";
        res += "C     Erstellt einen Zufallstensor in uebergenem\n";
        res += "C     Tensor 'A' fuer uebergebenen Dimension\n";
        res += "C     und Ordnung (Werte gleichverteilt in [0,1))\n";
        res += "      SUBROUTINE CREATE_RANDOM(A, DIMS, RANK)\n";
        res += "      IMPLICIT REAL*8 (A-H,O-Z)\n";
        res += "      INTEGER DIMS(*), RANK, N\n";
        res += "      INTEGER TENS_SIZE\n";
        res += "      REAL*8 A(*)\n";
        res += "      N = TENS_SIZE(DIMS, RANK)\n";
        res += "      CALL RANDOM_NUMBER(A(1:N))\n";
        res += "      RETURN\n";
        res += "      END\n";
        res += "\n";
        res += "C     Erstellt Zufallstensor in [LOW, HIGH)\n";
        res += "      SUBROUTINE CREATE_RANDOM_RANGE(A, DIMS, RANK,\n";
        res += "     &                                LOW, HIGH)\n";
        res += "      IMPLICIT REAL*8 (A-H,O-Z)\n";
        res += "      INTEGER DIMS(*), RANK, N, I\n";
        res += "      INTEGER TENS_SIZE\n";
        res += "      REAL*8 LOW, HIGH\n";
        res += "      REAL*8 A(*)\n";
        res += "      N = TENS_SIZE(DIMS, RANK)\n";
        res += "      CALL RANDOM_NUMBER(A(1:N))\n";
        res += "      DO 10 I = 1, N\n";
        res += "        A(I) = LOW + A(I)*(HIGH-LOW)\n";
        res += "   10 CONTINUE\n";
        res += "      RETURN\n";
        res += "      END\n";
        res += "\n";
        res += "C     Macaulay-Klammer: liefert X, falls X > 0, sonst 0\n";
        res += "      REAL*8 FUNCTION MACAULAY(X)\n";
        res += "      REAL*8 X\n";
        res += "      IF (X .GT. 0.0D0) THEN\n";
        res += "        MACAULAY = X\n";
        res += "      ELSE\n";
        res += "        MACAULAY = 0.0D0\n";
        res += "      END IF\n";
        res += "      RETURN\n";
        res += "      END\n";
        res += "\n";
        res += "C     Signum-Funktion: liefert -1, 0 oder 1 je nach Vorzeichen von X\n";
        res += "      REAL*8 FUNCTION SIGNUM(X)\n";
        res += "      REAL*8 X\n";
        res += "      IF (X .GT. 0.0D0) THEN\n";
        res += "        SIGNUM = 1.0D0\n";
        res += "      ELSE IF (X .LT. 0.0D0) THEN\n";
        res += "        SIGNUM = -1.0D0\n";
        res += "      ELSE\n";
        res += "        SIGNUM = 0.0D0\n";
        res += "      END IF\n";
        res += "      RETURN\n";
        res += "      END\n";
        res += "\n";
        res += "C     Setzt einen festen Seed fuer reproduzierbare\n";
        res += "C     Zufallszahlen\n";
        res += "      SUBROUTINE SET_SEED(SEEDVAL)\n";
        res += "      IMPLICIT REAL*8 (A-H,O-Z)\n";
        res += "      INTEGER SEEDVAL\n";
        res += "      INTEGER, ALLOCATABLE :: SEED(:)\n";
        res += "      INTEGER N\n";
        res += "      CALL RANDOM_SEED(SIZE=N)\n";
        res += "      ALLOCATE(SEED(N))\n";
        res += "      SEED = SEEDVAL\n";
        res += "      CALL RANDOM_SEED(PUT=SEED)\n";
        res += "      DEALLOCATE(SEED)\n";
        res += "      RETURN\n";
        res += "      END\n";
                res += "\n";
        res += "C     Invertiert einen Tensor gerader Ordnung RANK (=2K),\n";
        res += "C     aufgefasst als lineare Abbildung zwischen Tensoren\n";
        res += "C     der Ordnung K (erste K Indizes = Zeilen, letzte K = Spalten).\n";
        res += "C     Ergebnis wird nach B geschrieben.\n";
        res += "      SUBROUTINE INVERT_TENSOR(A, B, DIMS, RANK)\n";
        res += "      IMPLICIT REAL*8 (A-H,O-Z)\n";
        res += "      INTEGER MAXN\n";
        res += "      PARAMETER (MAXN=64)\n";
        res += "      REAL*8 A(*), B(*)\n";
        res += "      INTEGER DIMS(*), RANK, K, I, J, M\n";
        res += "      INTEGER NROW, NCOL, PIVOTROW\n";
        res += "      REAL*8 FACTOR, PIVOT, TEMP\n";
        res += "      REAL*8 AUG(MAXN, 2*MAXN)\n";
        res += "\n";
        res += "C     Ordnung muss gerade sein\n";
        res += "      IF (MOD(RANK,2) .NE. 0) THEN\n";
        res += "        WRITE(*,*) 'INVERT_TENSOR: RANK muss gerade sein'\n";
        res += "        STOP\n";
        res += "      END IF\n";
        res += "\n";
        res += "      K = RANK/2\n";
        res += "\n";
        res += "C     NROW = Produkt der ersten K Dimensionen\n";
        res += "      NROW = 1\n";
        res += "      DO 10 I = 1, K\n";
        res += "        NROW = NROW * DIMS(I)\n";
        res += "   10 CONTINUE\n";
        res += "\n";
        res += "C     NCOL = Produkt der letzten K Dimensionen\n";
        res += "      NCOL = 1\n";
        res += "      DO 20 I = K+1, RANK\n";
        res += "        NCOL = NCOL * DIMS(I)\n";
        res += "   20 CONTINUE\n";
        res += "\n";
        res += "C     Matrix muss quadratisch sein\n";
        res += "      IF (NROW .NE. NCOL) THEN\n";
        res += "        WRITE(*,*) 'INVERT_TENSOR: nicht quadratisch, ',\n";
        res += "     &              NROW, ' x ', NCOL\n";
        res += "        STOP\n";
        res += "      END IF\n";
        res += "\n";
        res += "      IF (NROW .GT. MAXN) THEN\n";
        res += "        WRITE(*,*) 'INVERT_TENSOR: MAXN zu klein fuer ', NROW\n";
        res += "        STOP\n";
        res += "      END IF\n";
        res += "\n";
        res += "C     Erweiterte Matrix [A | I] aufbauen\n";
        res += "C     (A ist bereits im flachen Speicher exakt eine NROWxNCOL-Matrix)\n";
        res += "      DO 30 I = 1, NROW\n";
        res += "        DO 40 J = 1, NCOL\n";
        res += "          AUG(I, J) = A((J-1)*NROW + I)\n";
        res += "   40   CONTINUE\n";
        res += "        DO 50 J = 1, NCOL\n";
        res += "          IF (I .EQ. J) THEN\n";
        res += "            AUG(I, NCOL+J) = 1.0D0\n";
        res += "          ELSE\n";
        res += "            AUG(I, NCOL+J) = 0.0D0\n";
        res += "          END IF\n";
        res += "   50   CONTINUE\n";
        res += "   30 CONTINUE\n";
        res += "\n";
        res += "C     Gauss-Jordan-Elimination mit Zeilenpivotisierung\n";
        res += "      DO 60 M = 1, NROW\n";
        res += "        PIVOTROW = M\n";
        res += "        PIVOT = DABS(AUG(M,M))\n";
        res += "        DO 70 I = M+1, NROW\n";
        res += "          IF (DABS(AUG(I,M)) .GT. PIVOT) THEN\n";
        res += "            PIVOT = DABS(AUG(I,M))\n";
        res += "            PIVOTROW = I\n";
        res += "          END IF\n";
        res += "   70   CONTINUE\n";
        res += "\n";
        res += "        IF (PIVOT .LT. 1.0D-14) THEN\n";
        res += "          WRITE(*,*) 'INVERT_TENSOR: singulaer bei Spalte ', M\n";
        res += "          STOP\n";
        res += "        END IF\n";
        res += "\n";
        res += "        IF (PIVOTROW .NE. M) THEN\n";
        res += "          DO 80 J = 1, 2*NCOL\n";
        res += "            TEMP = AUG(M,J)\n";
        res += "            AUG(M,J) = AUG(PIVOTROW,J)\n";
        res += "            AUG(PIVOTROW,J) = TEMP\n";
        res += "   80     CONTINUE\n";
        res += "        END IF\n";
        res += "\n";
        res += "        FACTOR = AUG(M,M)\n";
        res += "        DO 90 J = 1, 2*NCOL\n";
        res += "          AUG(M,J) = AUG(M,J) / FACTOR\n";
        res += "   90   CONTINUE\n";
        res += "\n";
        res += "        DO 100 I = 1, NROW\n";
        res += "          IF (I .NE. M) THEN\n";
        res += "            FACTOR = AUG(I,M)\n";
        res += "            DO 110 J = 1, 2*NCOL\n";
        res += "              AUG(I,J) = AUG(I,J) - FACTOR*AUG(M,J)\n";
        res += "  110       CONTINUE\n";
        res += "          END IF\n";
        res += "  100   CONTINUE\n";
        res += "   60 CONTINUE\n";
        res += "\n";
        res += "C     Rechte Haelfte zurueck in B schreiben (gleiches Layout wie A)\n";
        res += "      DO 120 I = 1, NROW\n";
        res += "        DO 130 J = 1, NCOL\n";
        res += "          B((J-1)*NROW + I) = AUG(I, NCOL+J)\n";
        res += "  130   CONTINUE\n";
        res += "  120 CONTINUE\n";
        res += "\n";
        res += "      RETURN\n";
        res += "      END\n";
        res += "\n\n";
        res += "C     Invertiert einen Tensor der Ordnung 0 (Skalar),\n";
        res += "C     also B = 1/A\n";
        res += "      SUBROUTINE INVERT_SCALAR(A, B)\n";
        res += "      IMPLICIT REAL*8 (A-H,O-Z)\n";
        res += "      REAL*8 A, B\n";
        res += "\n";
        res += "      IF (DABS(A) .LT. 1.0D-14) THEN\n";
        res += "        WRITE(*,*) 'INVERT_SCALAR: Division durch Null'\n";
        res += "        STOP\n";
        res += "      END IF\n";
        res += "\n";
        res += "      B = 1.0D0 / A\n";
        res += "\n";
        res += "      RETURN\n";
        res += "      END\n";
        res += "\n";
        res += "\n";
        res += "C     Berechnet die Determinante eines Tensors gerader\n";
        res += "C     Ordnung RANK (=2K), aufgefasst als lineare Abbildung\n";
        res += "C     zwischen Tensoren der Ordnung K (analog zu\n";
        res += "C     INVERT_TENSOR). Ergebnis wird als Skalar DET\n";
        res += "C     zurueckgegeben.\n";
        res += "      SUBROUTINE DET_TENSOR(A, DET, DIMS, RANK)\n";
        res += "      IMPLICIT REAL*8 (A-H,O-Z)\n";
        res += "      INTEGER MAXN\n";
        res += "      PARAMETER (MAXN=64)\n";
        res += "      REAL*8 A(*)\n";
        res += "      INTEGER DIMS(*), RANK, K, I, J, M\n";
        res += "      INTEGER NROW, NCOL, PIVOTROW\n";
        res += "      REAL*8 FACTOR, PIVOT, TEMP, DET, SGN\n";
        res += "      REAL*8 MAT(MAXN, MAXN)\n";
        res += "\n";
        res += "C     Ordnung muss gerade sein\n";
        res += "      IF (MOD(RANK,2) .NE. 0) THEN\n";
        res += "        WRITE(*,*) 'DET_TENSOR: RANK muss gerade sein'\n";
        res += "        STOP\n";
        res += "      END IF\n";
        res += "\n";
        res += "      K = RANK/2\n";
        res += "\n";
        res += "C     NROW = Produkt der ersten K Dimensionen\n";
        res += "      NROW = 1\n";
        res += "      DO 10 I = 1, K\n";
        res += "        NROW = NROW * DIMS(I)\n";
        res += "   10 CONTINUE\n";
        res += "\n";
        res += "C     NCOL = Produkt der letzten K Dimensionen\n";
        res += "      NCOL = 1\n";
        res += "      DO 20 I = K+1, RANK\n";
        res += "        NCOL = NCOL * DIMS(I)\n";
        res += "   20 CONTINUE\n";
        res += "\n";
        res += "C     Matrix muss quadratisch sein\n";
        res += "      IF (NROW .NE. NCOL) THEN\n";
        res += "        WRITE(*,*) 'DET_TENSOR: nicht quadratisch, ',\n";
        res += "     &              NROW, ' x ', NCOL\n";
        res += "        STOP\n";
        res += "      END IF\n";
        res += "\n";
        res += "      IF (NROW .GT. MAXN) THEN\n";
        res += "        WRITE(*,*) 'DET_TENSOR: MAXN zu klein fuer ', NROW\n";
        res += "        STOP\n";
        res += "      END IF\n";
        res += "\n";
        res += "C     Matrix kopieren (gleiches Layout wie INVERT_TENSOR)\n";
        res += "      DO 30 I = 1, NROW\n";
        res += "        DO 40 J = 1, NCOL\n";
        res += "          MAT(I, J) = A((J-1)*NROW + I)\n";
        res += "   40   CONTINUE\n";
        res += "   30 CONTINUE\n";
        res += "\n";
        res += "      SGN = 1.0D0\n";
        res += "\n";
        res += "C     Gauss-Elimination mit Zeilenpivotisierung\n";
        res += "C     (ohne Normierung, nur obere Dreiecksform)\n";
        res += "      DO 60 M = 1, NROW\n";
        res += "        PIVOTROW = M\n";
        res += "        PIVOT = DABS(MAT(M,M))\n";
        res += "        DO 70 I = M+1, NROW\n";
        res += "          IF (DABS(MAT(I,M)) .GT. PIVOT) THEN\n";
        res += "            PIVOT = DABS(MAT(I,M))\n";
        res += "            PIVOTROW = I\n";
        res += "          END IF\n";
        res += "   70   CONTINUE\n";
        res += "\n";
        res += "        IF (PIVOT .LT. 1.0D-14) THEN\n";
        res += "          DET = 0.0D0\n";
        res += "          RETURN\n";
        res += "        END IF\n";
        res += "\n";
        res += "        IF (PIVOTROW .NE. M) THEN\n";
        res += "          DO 80 J = 1, NCOL\n";
        res += "            TEMP = MAT(M,J)\n";
        res += "            MAT(M,J) = MAT(PIVOTROW,J)\n";
        res += "            MAT(PIVOTROW,J) = TEMP\n";
        res += "   80     CONTINUE\n";
        res += "          SGN = -SGN\n";
        res += "        END IF\n";
        res += "\n";
        res += "        DO 100 I = M+1, NROW\n";
        res += "          FACTOR = MAT(I,M) / MAT(M,M)\n";
        res += "          DO 110 J = M, NCOL\n";
        res += "            MAT(I,J) = MAT(I,J) - FACTOR*MAT(M,J)\n";
        res += "  110     CONTINUE\n";
        res += "  100   CONTINUE\n";
        res += "   60 CONTINUE\n";
        res += "\n";
        res += "      DET = SGN\n";
        res += "      DO 120 I = 1, NROW\n";
        res += "        DET = DET * MAT(I,I)\n";
        res += "  120 CONTINUE\n";
        res += "\n";
        res += "      RETURN\n";
        res += "      END\n";
    }

    if(uniqueExternals.empty()){

        res += "      SUBROUTINE " + instanceLabel + " (RES)\n\n";
    }
    else{

        res += "      SUBROUTINE " + instanceLabel + " (RES, ";
        res += fprintPlainVector(uniqueExternals, [](const IndexNotatedTensorExpression* elem){ return elem -> label; }, false, ",\n" + std::string(5, ' ') + "& ");
        res += ")\n\n";
    }

    res += "      IMPLICIT REAL*8 (A-H,O-Z)\n\n";
    res += "      REAL*8 MACAULAY, SIGNUM\n\n";

    // Maximale Order bestimmen
    int maxOrder = 0;
    for(const auto* elem : uniqueExternals){
        maxOrder = std::max(maxOrder, elem->tensorOrder);
    }

    for(const auto& elem : generatedDeps){
        maxOrder = std::max(maxOrder, elem->tensorOrder);
    }

    for(const auto& elem : depsKeys){
        maxOrder = std::max(maxOrder, elem.tensorOrder);
    }
    
    // Berücksichtigung des aktuellen Members
    maxOrder = std::max(maxOrder, tensorOrder);

    //
    res += "      INTEGER DIMS(" + std::to_string(std::max(maxOrder, 1)) + ")\n\n";
    res += "      INTEGER RANK\n\n";

    // Dimension Assert
    res += "C     Dimension Asserts\n\n";

    res += "      ";
    
    // Dimension Asserts Ergebnis
    if(tensorOrder == 0){ res += "REAL*8 RES"; }
    else{ res += "REAL*8 RES" + printPlainVector(dimensions); }

    res += "\n\n";

    // Dimension Asserts der Unique Externals
    res += "      " + fprintPlainVector(uniqueExternals,
        [](const IndexNotatedTensorExpression* elem){

            if(elem -> tensorOrder == 0){ return "REAL*8 " + elem -> label; }
            return "REAL*8 " + elem -> label + printPlainVector(elem -> dimensions);
        }, false, "\n" + std::string(6, ' '));
    
    res += "\n\n";

    // Anlegen der Funktionalen Deps
    res += "C     Anlegen der Funktionalen Deps\n\n";
    
    res += "      " + fprintPlainVector(generatedDeps,
    [](const IndexNotatedTensorExpression* elem){

        if(elem->tensorOrder == 0){ return "REAL*8 " + getArgLabel(*elem); }
        return "REAL*8 " + getArgLabel(*elem) + printPlainVector(elem->dimensions);
    }, false, "\n" + std::string(6, ' '));

    res += "\n\n";

    res += "      " + fprintPlainVector(depsKeys,
    [](const IndexNotatedTensorExpression& elem){

        if(elem.tensorOrder == 0){ return "REAL*8 " + elem.label; }
        return "REAL*8 " + elem.label + printPlainVector(elem.dimensions);
    }, false, "\n" + std::string(6, ' '));

    //
    res += "\n\n";

    //
    std::vector<NotationIndex> combinedUniqueIndices;

    auto addUnique = [&combinedUniqueIndices](const NotationIndex& idx) {
        for (const auto& existing : combinedUniqueIndices) {
            if (existing == idx) {
                return;
            }
        }
        combinedUniqueIndices.push_back(idx);
    };

    // eigene Indizes
    for (const auto& idx : collectAllContainedIndices()) {
        addUnique(idx);
    }

    // Indizes aus depsKeys
    for (const auto& elem : depsKeys) {
        for (const auto& idx : elem.collectAllContainedIndices()) {
            addUnique(idx);
        }
    }

    // Indizes aus depsValues
    for (const auto& elem : depsValues) {
        for (const auto& idx : elem.collectAllContainedIndices()) {
            addUnique(idx);
        }
    }

    // Einzigartige Indices als Integer konstruieren
    res += "      " + fprintPlainVector(combinedUniqueIndices,
    [](const NotationIndex& elem){

        return "INTEGER IDX_" + std::to_string(elem);
    }, false, "\n" + std::string(6, ' '));

    res += "\n\n";

    // Zwischenergebnisse
    size_t endOfDecls = res.size();

    //
    res += "\n\n";

    res += "C     DIMS nullen\n\n";

    //
    for(int i = 0; i < maxOrder; i++){

        res += "      DIMS(" + std::to_string(i + 1) + ") = 0\n"; 
    }

    res += "      RANK=0\n\n";

    //
    res += "\n\n";

    //
    res += "C     Funktionale Nodes\n\n";

    // generatedDeps (functional Nodes ermitteln)
    for(const auto& node : generatedDeps){

        res += fortranApplyDims(*node);

        if(node -> label == "Identity"){

            res += "      CALL CREATE_IDENTITY(" + getArgLabel(*node) + ", DIMS, RANK)\n";
        }
        else if(node -> label == "zeros"){

            res += "      CALL CREATE_ZEROS(" + getArgLabel(*node) + ", DIMS, RANK)\n";
        }
        else if(node -> label == "ones"){

            res += "      CALL CREATE_ONES(" + getArgLabel(*node) + ", DIMS, RANK)\n";
        }
        else if(node -> label == "eps"){
            
            res += "      CALL CREATE_EPS(" + getArgLabel(*node) + ", DIMS, RANK)\n";
        }

        res += "\n";
    }

    //
    res += "\n\n";

    //
    size_t endOfAssignments = res.size();

    res += "\n\n";

    res += "C     Deps ermitteln\n\n";

    //
    for(size_t i = 0; i < depsKeys.size(); i++){

        const auto& key = depsKeys[i];
        const auto& val = depsValues[i];

        res += "\n\nC     Eval " + key.label + "\n\n";
        res += val.wrapTensorSequenceFortranString(key.label);
    }

    res += "\n\n";

    res += "C     Target ermitteln\n\n";

    //
    res += wrapTensorSequenceFortranString("RES");

    // Zwischenergebnis-Decls nachträglich einfügen
    res.insert(endOfDecls, "\n\n" + g_fortranDependencieDecls + "\n\n");

    // Zwischenergebnis-Assignments nachträglich einfügen
    endOfAssignments += ("\n\n" + g_fortranDependencieDecls + "\n\n").size();
    // res.insert(endOfAssignments, "\n\n" + g_fortranDependencieAssignments + "\n\n");

    res += "\n\n";

    res += "      RETURN\n      END\n\n\n";

    if(generateDebugCall){

        res += "      PROGRAM MAIN\n";
        res += "      IMPLICIT REAL*8 (A-H,O-Z)\n\n";

        // Maximale Order bestimmen
        int maxOrder = 0;
        for(const auto* elem : uniqueExternals){
            maxOrder = std::max(maxOrder, elem->tensorOrder);
        }
        
        // Berücksichtigung des aktuellen Members
        maxOrder = std::max(maxOrder, tensorOrder);

        //
        res += "      INTEGER DIMS(" + std::to_string(std::max(maxOrder, 1)) + ")\n\n";
        res += "      INTEGER RANK\n\n";

        if(tensorOrder == 0){ res += "      REAL*8 RES\n\n"; }
        else{ res += "      REAL*8 RES" + printPlainVector(dimensions) + "\n\n"; }

        res += "      " + fprintPlainVector(uniqueExternals,
        [](const IndexNotatedTensorExpression* elem){

            if(elem -> tensorOrder == 0){ return "REAL*8 " + elem -> label; }
            return "REAL*8 " + elem -> label + printPlainVector(elem -> dimensions);
        }, false, "\n" + std::string(6, ' '));

        res += "\n\n";

        // DIMS nullen
        res += "C     DIMS nullen\n\n";

        //
        for(int i = 0; i < maxOrder; i++){

            res += "      DIMS(" + std::to_string(i + 1) + ") = 0\n"; 
        }

        res += "      RANK=0\n\n";

        res += "C     Random Unique Externals\n\n";

        size_t randomLower = 0, randomHigher = 10;

        res += "      " + fprintPlainVector(uniqueExternals,
        [&](const IndexNotatedTensorExpression* elem){

            if(elem -> tensorOrder == 0){

                return "\n" + std::string("      CALL RANDOM_NUMBER(") + elem->label + ")" +
                       "\n      " + elem->label + " = " +
                       std::to_string(randomLower) + "D0 + " +
                       elem->label + "*(" +
                       std::to_string(randomHigher) + "D0 - " +
                       std::to_string(randomLower) + "D0)\n";
            }

            return "\n" + fortranApplyDims(*elem) + "\n      CALL CREATE_RANDOM_RANGE(" + elem->label +
                ", DIMS, RANK, " + std::to_string(randomLower) + "D0, " +
                std::to_string(randomHigher) + "D0)\n";
        }, false, "\n" + std::string(6, ' '));

        res += "\n\n";

        if(uniqueExternals.empty()){ res += "      CALL " + instanceLabel + " (RES)\n\n"; }
        else{

            res += "      CALL " + instanceLabel + " (RES, ";
            res += fprintPlainVector(uniqueExternals, [](const IndexNotatedTensorExpression* elem){ return elem -> label; }, false, ",\n" + std::string(5, ' ') + "& ");
            res += ")\n\n";
        }

        //
        if(tensorOrder == 0){

            res += "      WRITE(*,*) 'RES = ', RES\n\n";

        } else {

            res += fortranApplyDims(*this);

            res += "\n";
            res += "      CALL PRINT_TENSOR(RES, DIMS, " + std::to_string(tensorOrder) + ", 'RES')\n\n";
        }

        res += "      STOP\n";
        res += "      END\n";
    }

    //
    dependencieIdx = 0;
    g_fortranDependencieDecls = "C     __DECLS__\n\n";

    return res;
}
