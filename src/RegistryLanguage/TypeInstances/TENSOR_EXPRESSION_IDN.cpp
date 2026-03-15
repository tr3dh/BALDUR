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

            // Prüfen ob die Node schon in uniqueNodes enthalten ist
            for (const auto* u : uniqueNodes)
            {
                if (areEqualExternals(*u, node)){ return; }
                else if(node.isConstant){ return; }
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

        return node.label + "_ord" + std::to_string(node.tensorOrder) + "_dm" + printPlainVector(node.dimensions, false, "");
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

int maxExprComplexity = 25;
int criticalMaxExprComplexity = 30;

std::string IndexNotatedTensorExpression::wrapTensorSequenceTullioString() const{

    IndexNotatedTensorExpression copy = *this;
    return copy.generateTensorSequenceTullioString();
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

// Funktion sollte unter keinen umständen auf Object angewendet werden mit dem weiter gearbeitet werden soll
// dafür gibts die wrapper funktion
std::string IndexNotatedTensorExpression::generateTensorSequenceTullioString(size_t depth, bool forceSubstitution, bool useTensorNotation){

    // Werte so setzen dass sie Rekursive Funktion direkt beim ersten Durchlauf abbrechen
    static int dependencieIdx = -1;
    static std::string dependencieDecls = "__INVALIDDECLS__", dependencieAssignment = "__INVALIDDECLS__";
    static bool terminate = true;

    // >> Setup der Werte für jeden einzelnen Aufruf der Funktionen für einen frischen Ausdruck
    if(depth == 0){

        dependencieIdx = 0;
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
            case IndexNotationOperator::Inversion:{

                //
                std::string jlFuncLabel;

                if(Operator == IndexNotationOperator::Macaulay){ jlFuncLabel = "macaulay"; }
                else if(Operator == IndexNotationOperator::Signum){ jlFuncLabel = "signum"; }
                else if(Operator == IndexNotationOperator::Determinant){ jlFuncLabel = "det"; }
                else if(Operator == IndexNotationOperator::Frobenius){ jlFuncLabel = "frobenius"; }
                else if(Operator == IndexNotationOperator::Sqrt){ jlFuncLabel = "sqrt"; }
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
    if(Relation == TkType::Operator && getNumOfNodes() > criticalMaxExprComplexity){

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
            (useTOps ? "\n\t@tensor opt=true " : "\n\t") + asExternalNode("res").generateTensorSequenceTullioString(1) + ((returnScalar && useTOps) ? "[]" : "") + (useTOps ? " := " : " = ") + res + "\n\n\treturn res";
    }
    else if((Relation == TkType::Operator && getNumOfNodes() > maxExprComplexity) || forceSubstitution){

        RETURNING_ASSERT(getNumOfNodes() <= criticalMaxExprComplexity, "...", "");

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
std::string IndexNotatedTensorExpression::toJuliaString(const std::string& instanceLabel) const {

    //
    RETURNING_ASSERT(containsDimensions(), "Ohne Dimensionsberücksichtigung kann Julia Skript nicht erstellt werden","");

    // Unique External Nodes
    auto uniqueExternals = getUniqueExternalNodes();

    // Return string
    std::string res;

    //
    res += "# Julia Skript\n#\n";
    res += "# unique external nodes :\n";

    //
    for(const auto& node : getUniqueExternalNodes()){

        res += "# | arg '" + getArgLabel(*node) + "', order [" + std::to_string(node->tensorOrder) + "], dimensions {";
        res += printPlainVector(node->dimensions, false);
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

    //
    res += "\n";
    res += "function " + instanceLabel + "(";

    //
    bool filledInFirstExternal = false;

    //
    for(const auto& node : getUniqueExternalNodes()){

        if(isFunctionalNode(*node)){

            continue;
        }

        res += filledInFirstExternal ? ", " + node->label : node->label;
        filledInFirstExternal = true;
    }

    // Abhängigkeiten des Indexnotierten Ausdrucks >> unique External Nodes

    res += ")\n\n";

    //
    auto externalNodes = getUniqueExternalNodes();

    //
    for(const auto& node : externalNodes){

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

    res += wrapTensorSequenceTullioString();
    res += "\n\n";

    //
    res += "end\n\n";

    if(generateDebugCall){

        res += "start_time = time()\nres = " + instanceLabel + "(";

        //
        for(auto it = externalNodes.begin(); it != externalNodes.end(); ){

            if(isFunctionalNode(**it)){
                it = externalNodes.erase(it);
            } else {
                ++it;
            }
        }

        for(auto it = externalNodes.begin(); it != externalNodes.end(); ++it) {

            auto expr = *it;

            //
            res += "rand" + printPlainVector(expr->dimensions);
            
            //
            if(std::next(it) != externalNodes.end()) {
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
        registerFunction("setGenerateJLDebugCall", {BOOL::typeIndex},
            [__functionLabel__ = "setGenerateJLDebugCall", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(BOOL, 0);

                generateDebugCall = arg0->getMember();
        },
        {});

        return true;
    }
}