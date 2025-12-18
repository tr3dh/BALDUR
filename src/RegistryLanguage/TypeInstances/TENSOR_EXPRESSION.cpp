#include "TENSOR_EXPRESSION.h"
#include "PermutationGenerator.h"

bool unwrapOperands = false;

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
    {TensorExpressionOperator::Determinant, "^det"},
};

std::map<TensorExpressionOperator, void (TensorExpression::*)(const TensorExpression&)> operatorMemberFunctions = {

    {TensorExpressionOperator::Addition, &TensorExpression::addAssign},
    {TensorExpressionOperator::Subtraction, &TensorExpression::subAssign},
    {TensorExpressionOperator::DotProduct, &TensorExpression::dotProductAssign},
    {TensorExpressionOperator::CrossProduct, &TensorExpression::crossProductAssign},
    {TensorExpressionOperator::DyadicProduct, &TensorExpression::dyadProductAssign},
    {TensorExpressionOperator::Multiplication, &TensorExpression::mulAssign},
    {TensorExpressionOperator::Diff, &TensorExpression::diffAssign},
    {TensorExpressionOperator::CrossingDoubleContraction, &TensorExpression::crossingDoubleContractionAssign},
    {TensorExpressionOperator::MirroringDoubleContraction, &TensorExpression::mirroringDoubleContractionAssign},
};

std::map<TensorExpressionOperator, void (TensorExpression::*)()> singleArgOperatorMemberFunctions{

    {TensorExpressionOperator::Trace, &TensorExpression::traceAssign},
    {TensorExpressionOperator::Inversion, &TensorExpression::inverseAssign},
    {TensorExpressionOperator::Transposition, &TensorExpression::transposeAssign},
    {TensorExpressionOperator::Section, &TensorExpression::sectionAssign},
    {TensorExpressionOperator::Determinant, &TensorExpression::determinantAssign},
    {TensorExpressionOperator::Zeros, &TensorExpression::zerosAssign},
};

std::map<std::pair<TensorExpression, TensorExpression>, TensorExpression> tensorExpressionDiffs = {};
std::map<std::pair<TensorExpression, TensorExpression>, TensorExpression> tensorExpressionDiffTemplates = {};

std::map<TensorExpression, TensorExpression> tensorExpressionSimplifications = {};

int TensorExpression::minCnstLimit = std::numeric_limits<int>::min();

// Substitutionsmap muss zusätzlich nach den labels der templatierten Nodes unterscheiden sonst
// kann nur eine templatierte Node einer Stufe (oder einmal -1) in der Map stehen
bool SubstitutionComparator::operator()(const TensorExpression& a, const TensorExpression& b) const{

    if (a < b) return true;
    if (b < a) return false;

    return a.label < b.label;
}

void moveSelfIntoFirstChild(TensorExpression& node){

    TensorExpression tmp = std::move(node);

    node = TensorExpression();
    node.children.emplace_back(std::move(tmp));
}

// bei Rückgabe von true wird lhs vor rhs sortiert bei false andersherum
bool operator<(const TensorExpression& lhs, const TensorExpression& rhs)
{
    // LOG << "SINGLE::operator< aufgerufen " << lhs.toString(1) << " " << rhs.toString(1) << endl;
    // LOG << std::flush;

    bool lhsIsTemplate = lhs.isTemplate(), rhsIsTemplate = rhs.isTemplate();

    //
    if (lhsIsTemplate != rhsIsTemplate){ return !lhsIsTemplate; }
     
    //
    if (lhs.Relation != rhs.Relation){ return lhs.Relation < rhs.Relation; }
    if (lhs.Operator != rhs.Operator){ return lhs.Operator < rhs.Operator; }

    if(lhs.isConstant && rhs.isConstant && lhs.value != rhs.value){ return lhs.value > rhs.value; }
    else if(lhs.isConstant && !rhs.isConstant && rhs.Relation == TkType::Argument){ return true; }
    else if(!lhs.isConstant && lhs.Relation == TkType::Argument && rhs.isConstant){ return false; }

    if(!lhsIsTemplate && !rhsIsTemplate && lhs.label != rhs.label){
        return lhs.label < rhs.label;
    }

    // Überladung nach Tensorstufe
    if(lhs.tensorOrder != rhs.tensorOrder){ return lhs.tensorOrder > rhs.tensorOrder; }

    //
    const auto& a = lhs.children;
    const auto& b = rhs.children;

    if (a.size() != b.size()){

        return a.size() < b.size();
    }

    // Hier kommt es zur Unterscheidung identischer Template Ausdrücke, die sich nur noch durch Doppelnennungen
    // einzelner Nodex unterscheiden, also zb. A .. B .. C und A .. B .. A
    // >> Zählen der einzigartigen Template Nodes über eine SubstitutionMap
    // >> der Eintrag mit weniger Einträgen in der Map also weniger einzigartigen Einträgen
    //    überlädt den anderen, da er eine Spezifizierung gegenüber dem anderen darstellt
    // zb. A .. B .. C ist eine algemeinere Form als A .. B .. A und soll deshalb im Template Register weiter hintenstehen
    //     stände es weiter vorne, würden Ausdrüche die mit A .. B .. A repräsentiert werden können Gleichheit mit mit
    //     A .. B .. C auslösen. Sie würden zwar trotzdem beim Substitutionsmap abgleich dann aussortiert werden,
    //     aber die korrekte Überladung spart Zeit 

    // den Teil mit den total nodes evtl auslagern ?? >> nicht nur für templates relevant
    if(lhsIsTemplate && rhsIsTemplate){

        size_t lhsNumOfTotalNodes = lhs.getNumOfNodes();
        size_t rhsNumOfTotalNodes = rhs.getNumOfNodes();

        if(lhsNumOfTotalNodes != rhsNumOfTotalNodes){

            return lhsNumOfTotalNodes > rhsNumOfTotalNodes;
        }

        //
        size_t lhsNumUniqueNodes = lhs.getNumOfUniqueNodes();
        size_t rhsNumUniqueNodes = rhs.getNumOfUniqueNodes();

        // den ausdruck mit weniger unique nodes nach vorne sortieren (überladen)
        if(lhsNumUniqueNodes < rhsNumUniqueNodes){ return true; }
        else if(lhsNumUniqueNodes > rhsNumUniqueNodes){ return false; }
    }

    for (size_t i = 0; i < a.size(); ++i) {
        
        if (a[i] < b[i]) return true;
        if (b[i] < a[i]) return false;
    }

    // // Fallback
    // if(lhsIsTemplate && rhsIsTemplate && lhs.label != rhs.label){

    //     return lhs.label < rhs.label;
    // }

    //
    // _ERROR << lhs.toString() << " < " << rhs.toString() << " kann nicht aufgelöst werden" << endl;

    //
    return false;

    // wäre Fallback falls etwas nicht sortiert wird
    // return lhs.label < rhs.label;
}

// Nur möglich wenn mindestens einer der Typen im pair dein eigener Typ ist
bool operator<(const std::pair<TensorExpression, TensorExpression>& lhs, const std::pair<TensorExpression, TensorExpression>& rhs) {

    // LOG << "PAIR::operator< aufgerufen" << " " << lhs.first.toString() << " " << rhs.first.toString() << endl;
    // LOG << std::flush;

    // Standard-Vergleich
    if(lhs.first < rhs.first){ return true; }
    if(rhs.first < lhs.first){ return false; }
    
    // first ist gleich, vergleiche second
    if(lhs.second < rhs.second){ return true; }
    if(rhs.second < lhs.second){ return false; }

    //
    TensorExpression lhsFirst = lhs.first;
    lhsFirst.dyadProductAssign(lhs.second);
    
    TensorExpression rhsFirst = rhs.first;
    rhsFirst.dyadProductAssign(rhs.second);

    //
    size_t lhsNodes = lhsFirst.getNumOfUniqueNodes(), rhsNodes = rhsFirst.getNumOfUniqueNodes();

    //
    if(lhsNodes != rhsNodes){ return lhsNodes < rhsNodes; }

    return false;
}

size_t TensorExpression::getNumOfUniqueNodes() const{

    std::vector<const TensorExpression*> uniqueNodes;

    // rekursive DFS Funktion
    std::function<void(const TensorExpression&)> dfs = [&](const TensorExpression& node)
    {
        // Prüfen ob die Node schon in uniqueNodes enthalten ist
        for (const auto* u : uniqueNodes)
        {
            if (structurallyEqual(*u, node))
                return;
        }

        // Node ist einzigartig
        uniqueNodes.push_back(&node);

        // weiter durch childs iterieren
        for (const auto& child : node.children)
        {
            dfs(child);
        }
    };

    dfs(*this);

    return uniqueNodes.size();
}

size_t TensorExpression::getNumOfNodes() const{

    size_t numOfNodes = children.size();

    for(const auto& child : children){

        numOfNodes += child.getNumOfNodes();
    }

    return numOfNodes;
}

// Statics
void TensorExpression::replaceBySubstitutions(TensorExpression& expr, const substitutionMap& subsMap){

    // Konstanten müssen nicht ersetzt werden
    if(expr.isConstant){ return; }
    // if(expr.Relation == TkType::Argument && !expr.isTemplate()){ return; }
    if(!expr.isTemplate()){ return; }

    for(const auto& [subsKey, subsVal] : subsMap){
        
        //
        if(expr == subsKey){

            expr = subsVal;
            return;
        }
    }

    for(auto& child : expr.children){

        replaceBySubstitutions(child, subsMap);
    }
}

bool TensorExpression::assembleSubstitutionMap(const TensorExpression& tmplExpr, const TensorExpression& expr, substitutionMap& subsMap, bool disableLog){

    bool res = true;

    RETURNING_ASSERT(tmplExpr == expr, "Ungleiche Operanden für Template Substitution", false);
    RETURNING_ASSERT(tmplExpr.children.size() == expr.children.size() || tmplExpr.children.size() == 0,
                     "Ungleiche Operanden Childs für Template Substitution", false);

    if(!tmplExpr.isTemplate()){ 
        
        return true;
    }

    bool tmplIsTemplatedNode = tmplExpr.isTemplatedNode();

    if(tmplIsTemplatedNode && subsMap.try_emplace(tmplExpr, expr).second){}
    else if(tmplIsTemplatedNode && subsMap[tmplExpr] != expr){

        if(!disableLog){

            RETURNING_ASSERT(TRIGGER_ASSERT,
                "Wiedersprüchliche Substitutions Argumente übergeben, Für " + tmplExpr.toString() + " hinterlegt : " +
                subsMap[tmplExpr].toString() + " inkonsistentes Vorkommen : " + expr.toString(), false);
        }
            
        return false;
    }
    else if(tmplIsTemplatedNode){

    }
    else if(!tmplExpr.isCommutativ()){

        for(size_t i = 0; i < tmplExpr.children.size(); i++){

            res = res && assembleSubstitutionMap(tmplExpr.children[i], expr.children[i], subsMap);
        }
    }
    else{

        size_t N = 2;
        std::vector<size_t> indices(N);
        for(size_t i = 0; i < N; i++) indices[i] = i;

        std::vector<bool> used(N, false);
        std::vector<size_t> current(N);
        std::vector<std::vector<size_t>> allPermutations;

        size_t matchingPermutation = allPermutations.size();
        bool permutationIsMatching = true;

        generateIndexCombinations(indices, used, current, allPermutations, 0);

        // // Ausgabe
        // for(const auto& perm : allPermutations){

        //     for(size_t x : perm) std::cout << x << " ";
        //     LOG << endl;
        // }

        //
        for(size_t permIdx = 0; permIdx < allPermutations.size(); permIdx++){

            const auto& perm = allPermutations[permIdx];
            permutationIsMatching = true;

            for(size_t i = 0; i < tmplExpr.children.size(); i++){

                if(!(tmplExpr.children[i] == expr.children[perm[i]])){

                    permutationIsMatching = false;
                    break;
                }
            }

            if(permutationIsMatching){

                matchingPermutation = permIdx;
                break;
            }
        }

        if(matchingPermutation < allPermutations.size()){

            for(size_t i = 0; i < tmplExpr.children.size(); i++){

                res = res && assembleSubstitutionMap(tmplExpr.children[i], expr.children[allPermutations[matchingPermutation][i]], subsMap);
            }
        }
    }
    
    return res;
}

//
bool TensorExpression::structurallyEqual(const TensorExpression& a, const TensorExpression& b)
{
    // Template vs non-template
    if (a.Relation != b.Relation) return false;
    if (a.Operator != b.Operator) return false;

    // Label nur relevant bei konkreten Ausdrücken
    if (a.label != b.label) return false;

    if (a.tensorOrder != b.tensorOrder) return false;
    if (a.contractNIndices != b.contractNIndices) return false;

    if (a.children.size() != b.children.size()) return false;

    // Rekursive Prüfung aller Child-Nodes
    for (size_t i = 0; i < a.children.size(); ++i)
    {
        if (!structurallyEqual(a.children[i], b.children[i]))
            return false;
    }

    return true;
}

// Default Konstruktion
TensorExpression::TensorExpression() = default;

// Konstruktion einer Arg node
TensorExpression::TensorExpression(const std::string& labelIn, int tensorOrderIn) : label(labelIn), tensorOrder(tensorOrderIn){

    Relation = TkType::Argument;
}

TensorExpression::TensorExpression(float valueIn) : value(valueIn){

    Relation = TkType::Argument;
    tensorOrder = 0;
    isConstant = true;
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

//
TensorExpression TensorExpression::rebuild() const{

    TensorExpression res;

    if(Relation == TkType::Argument || isTemplatedNode()){

        res = *this;
        return res;
    }

    RETURNING_ASSERT(children.size() > 0, "Rebuild für Nicht Arg node mit 0 childs nicht möglich : " + toString(), {});
    res = children[0].rebuild();

    if(Relation == TkType::Operator && operatorMemberFunctions.contains(Operator)){

        for (size_t i = 1; i < children.size(); i++) {

            (res.*operatorMemberFunctions[Operator])(children[i].rebuild());
        }
    }
    else if(Operator == TensorExpressionOperator::Trace && contractNIndices > 0){

        res.traceAssign(contractNIndices);
    }
    else if(singleArgOperatorMemberFunctions.contains(Operator)){

        (res.*singleArgOperatorMemberFunctions[Operator])();
    }
    else{

        LOG << toString() << endl;
        RETURNING_ASSERT(TRIGGER_ASSERT, "Rebuild nicht möglich", res);
    }

    return res;
}

TensorExpression TensorExpression::unwrap() const{

    bool storedFlag = unwrapOperands;
    unwrapOperands = true;

    TensorExpression res = rebuild();
    unwrapOperands = storedFlag;

    return res;
}

void TensorExpression::simplify(){

    while(simplifyOnce()){}
}

//
bool TensorExpression::simplifyOnce(){

    for(const auto& [k, v] : tensorExpressionSimplifications){

        if(k == *this){

            auto prevOrder = tensorOrder;
            
            RETURNING_ASSERT(prevOrder == v.tensorOrder, "Inkonsistentes Umformungstemplate", false);
            
            *this = v;
            return true;
        }
    }

    bool expressionChanged = false;

    for(auto& child : children){

        expressionChanged = child.simplifyOnce() || expressionChanged;
    }

    return expressionChanged;
}

// Operatoren
void TensorExpression::addAssign(const TensorExpression& other){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::Addition;

    // ASSERTS
    RETURNING_ASSERT(tensorOrder == other.tensorOrder || tensorOrder == -1 || other.tensorOrder == -1, "Addition von Tensoren unterschiedlicher Stufe versucht",);

    //
    bool copySelf = false;

    //
    if(!unwrapOperands || (Relation != TkType::Operator || (Relation == TkType::Operator && Operator != operation))){

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
    if(unwrapOperands && (children.back().Relation == TkType::Operator && children.back().Operator == operation)){

        // sichere Kopie
        std::vector<TensorExpression> tempChildren = children.back().children;
        children.pop_back();
        children.insert(children.end(), std::make_move_iterator(tempChildren.begin()), std::make_move_iterator(tempChildren.end()));
    }

    // Anpassen TensorOrder
    // hier nicht nötig aufgrund von Addition
    // ...

    //
    const TensorExpression& otherMember = (copySelf ? children.back() : other);

    // Aufgrund des movIntoSelf ist tensorOrder eh -1 wenn erster Operand -1 als tensorOrder hat
    if(otherMember.tensorOrder == -1){
        tensorOrder = -1;
    }
}

//
void TensorExpression::subAssign(const TensorExpression& other){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::Subtraction;

    // ASSERTS
    RETURNING_ASSERT(tensorOrder == other.tensorOrder || tensorOrder == -1 || other.tensorOrder == -1, "Addition von Tensoren unterschiedlicher Stufe versucht",);

    //
    bool copySelf = false;

    //
    if(!unwrapOperands || (Relation != TkType::Operator || (Relation == TkType::Operator && Operator != operation))){

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

    //
    const TensorExpression& otherMember = (copySelf ? children.back() : other);

    // Aufgrund des movIntoSelf ist tensorOrder eh -1 wenn erster Operand -1 als tensorOrder hat
    if(otherMember.tensorOrder == -1){
        tensorOrder = -1;
    }
}

void TensorExpression::mulAssign(const TensorExpression& other){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::Multiplication;

    // ASSERTS
    RETURNING_ASSERT(tensorOrder == 0 || other.tensorOrder == 0 || tensorOrder == -1 || other.tensorOrder == -1, "Skalar Multiplikation ohne Skalar versucht",);

    //
    bool copySelf = false;

    //
    if(!unwrapOperands || (Relation != TkType::Operator || (Relation == TkType::Operator && Operator != operation))){

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
    if(unwrapOperands && (children.back().Relation == TkType::Operator && children.back().Operator == operation
        && (children.back().tensorOrder == 0 || tensorOrder == 0))){

        // sichere Kopie
        std::vector<TensorExpression> tempChildren = children.back().children;
        children.pop_back();
        children.insert(children.end(), std::make_move_iterator(tempChildren.begin()), std::make_move_iterator(tempChildren.end()));
    }

    // Anpassen TensorOrder
    
    //
    const TensorExpression& otherMember = (copySelf ? children.back() : other);

    // Aufgrund des movIntoSelf ist tensorOrder eh -1 wenn erster Operand -1 als tensorOrder hat
    if(tensorOrder == -1){}
    else if(otherMember.tensorOrder == -1){ tensorOrder = -1; }
    else{
        tensorOrder = otherMember.tensorOrder > tensorOrder ? otherMember.tensorOrder : tensorOrder;
    }
}

void TensorExpression::dotProductAssign(const TensorExpression& other){
    
    //
    static TensorExpressionOperator operation = TensorExpressionOperator::DotProduct;

    // ASSERTS
    RETURNING_ASSERT((tensorOrder > 0 && other.tensorOrder > 0) || tensorOrder == -1 || other.tensorOrder == -1, "Tensoren mit Stufe kleiner 1 and Skalarprodukt beteiligt",);

    //
    bool copySelf = false;

    //
    if(!unwrapOperands || (Relation != TkType::Operator || (Relation == TkType::Operator && Operator != operation))){

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

    //
    const TensorExpression& otherMember = (copySelf ? children.back() : other);

    // Aufgrund des movIntoSelf ist tensorOrder eh -1 wenn erster Operand -1 als tensorOrder hat
    if(tensorOrder == -1){}
    else if(otherMember.tensorOrder == -1){ tensorOrder = -1; }
    else{
        tensorOrder = tensorOrder + otherMember.tensorOrder - 2;
    }
}

void TensorExpression::crossProductAssign(const TensorExpression& other){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::CrossProduct;

    // ASSERTS
    RETURNING_ASSERT((tensorOrder == 1 || tensorOrder == -1) && (other.tensorOrder == 1 || other.tensorOrder == -1), "Tensoren für Vektorprodukt müssen Vektoren sein",);

    //
    bool copySelf = false;

    //
    if(!unwrapOperands || (Relation != TkType::Operator || (Relation == TkType::Operator && Operator != operation))){

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

    //
    const TensorExpression& otherMember = (copySelf ? children.back() : other);

    // Anpassen TensorOrder
    // Aufgrund des movIntoSelf ist tensorOrder eh -1 wenn erster Operand -1 als tensorOrder hat
    if(tensorOrder == -1){}
    else if(otherMember.tensorOrder == -1){ tensorOrder = -1; }
}

void TensorExpression::dyadProductAssign(const TensorExpression& other){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::DyadicProduct;

    // ASSERTS
    RETURNING_ASSERT((tensorOrder > 1 && other.tensorOrder > 1) || tensorOrder == -1 || other.tensorOrder == -1, "Tensoren mit Stufe kleiner 2 ...",);

    //
    bool copySelf = false;

    //
    if(!unwrapOperands || (Relation != TkType::Operator || (Relation == TkType::Operator && Operator != operation))){

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
    const TensorExpression& otherMember = (copySelf ? children.back() : other);

    // Aufgrund des movIntoSelf ist tensorOrder eh -1 wenn erster Operand -1 als tensorOrder hat
    if(tensorOrder == -1){}
    else if(otherMember.tensorOrder == -1){ tensorOrder = -1; }
    else{
        tensorOrder = tensorOrder + otherMember.tensorOrder;
    }
}

void TensorExpression::mirroringDoubleContractionAssign(const TensorExpression& other){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::MirroringDoubleContraction;

    // ASSERTS
    RETURNING_ASSERT((tensorOrder > 1 || tensorOrder == -1) && (other.tensorOrder > 1 || other.tensorOrder == -1), "Tensoren mit Stufe kleiner 1 and ... beteiligt",);

    //
    bool copySelf = false;

    //
    if(!unwrapOperands || (Relation != TkType::Operator || (Relation == TkType::Operator && Operator != operation))){

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

    // Anpassen TensorOrder
    const TensorExpression& otherMember = (copySelf ? children.back() : other);

    // Aufgrund des movIntoSelf ist tensorOrder eh -1 wenn erster Operand -1 als tensorOrder hat
    if(tensorOrder == -1){}
    else if(otherMember.tensorOrder == -1){ tensorOrder = -1; }
    else{
        tensorOrder = tensorOrder + otherMember.tensorOrder - 4;
    }
}

void TensorExpression::crossingDoubleContractionAssign(const TensorExpression& other){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::CrossingDoubleContraction;

    // ASSERTS
    RETURNING_ASSERT((tensorOrder > 1 || tensorOrder == -1) && (other.tensorOrder > 1 || other.tensorOrder == -1), "Tensoren mit Stufe kleiner 1 and ... beteiligt",);

    //
    bool copySelf = false;

    //
    if(!unwrapOperands || (Relation != TkType::Operator || (Relation == TkType::Operator && Operator != operation))){

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
    const TensorExpression& otherMember = (copySelf ? children.back() : other);

    // Aufgrund des movIntoSelf ist tensorOrder eh -1 wenn erster Operand -1 als tensorOrder hat
    if(tensorOrder == -1){}
    else if(otherMember.tensorOrder == -1){ tensorOrder = -1; }
    else{
        tensorOrder = tensorOrder + otherMember.tensorOrder - 4;
    }
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
    RETURNING_ASSERT(tensorOrder > contractIndices || tensorOrder == -1, "Tensor Dimension nicht groß genug für Trace mit angegebener Kontraktion",);

    //
    moveSelfIntoFirstChild();

    // node erneut Aufsetzen
    Relation = TkType::Operator;
    Operator = operation;
    tensorOrder = children.back().tensorOrder != -1 ? children.back().tensorOrder - (contractIndices + 1) : -1;
    contractNIndices = contractIndices;
}

void TensorExpression::determinantAssign(){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::Determinant;

    //
    moveSelfIntoFirstChild();

    // node erneut Aufsetzen
    Relation = TkType::Operator;
    Operator = operation;
    tensorOrder = 0;
}

void TensorExpression::sectionAssign(){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::Section;

    //
    moveSelfIntoFirstChild();

    // node erneut Aufsetzen
    Relation = TkType::Operator;
    Operator = operation;
    tensorOrder = children.back().tensorOrder;
}

void TensorExpression::zerosAssign(){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::Zeros;

    if(tensorOrder < 0){

        //
        moveSelfIntoFirstChild();

        // node erneut Aufsetzen
        Relation = TkType::Operator;
        Operator = operation;
        tensorOrder = 0;
    }
    else{
        
        *this = TensorExpression("zeros", tensorOrder);
    }
}

bool TensorExpression::isCommutativ() const{

    if(Relation != TkType::Operator){ return false; }

    if(Operator == TensorExpressionOperator::Addition || Operator == TensorExpressionOperator::Multiplication){
        return true;
    }

    return false;
}

bool TensorExpression::operator==(const TensorExpression& other) const {

    // Check ob gleiche Instanz
    if(this == &other){ return true; }

    if(((isTemplatedNode() && !other.isTemplate()) || (!isTemplate() && other.isTemplatedNode())) &&
       (tensorOrder == other.tensorOrder || (tensorOrder == -1 || other.tensorOrder == -1))){

        return true;
    }

    if(Relation != other.Relation){ return false; }
    if(Operator != other.Operator){ return false; }
    if(isConstant && other.isConstant && !isConstantTemplate() && !other.isConstantTemplate() && (value != other.value)){ return false; }

    if(label != other.label){ return false; }

    if(tensorOrder != -1 && other.tensorOrder != -1 && tensorOrder != other.tensorOrder){ return false; }

    if(children.size() != other.children.size()){ return false; }

    bool equal = true;

    for(size_t childIdx = 0; childIdx < children.size(); childIdx++){
        
        if(!(children[childIdx] == other.children[childIdx])){
            
            equal = false;
        } 
    }

    if(equal){ return true; }
    else if(!isCommutativ()){ return equal; }

    auto sortedChildren = unwrap().children;
    auto sortedOtherChildren = other.unwrap().children;

    // for(const auto& child : sortedChildren){ LOG << child.toString(1) << " | "; }
    // LOG << endl;
    // for(const auto& child : sortedOtherChildren){ LOG << child.toString(1) << " | "; }
    // LOG << endl;

    std::sort(sortedChildren.begin(), sortedChildren.end());
    std::sort(sortedOtherChildren.begin(), sortedOtherChildren.end());

    // for(const auto& child : sortedChildren){ LOG << child.toString(1) << " | "; }
    // LOG << endl;
    // for(const auto& child : sortedOtherChildren){ LOG << child.toString(1) << " | "; }
    // LOG << endl;

    for(size_t i = 0; i < sortedChildren.size(); i++){
        
        // LOG << sortedChildren[i].toString() << " == " << sortedOtherChildren[i].toString() << " = " << (sortedChildren[i] == sortedOtherChildren[i]) << endl;
        if(!(sortedChildren[i] == sortedOtherChildren[i])){
            return false;
        }
    }

    return true;
}

void TensorExpression::rawDiffAssign(const TensorExpression& other){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::Diff;

    bool copySelf = false;

    // mov
    if(this == &other){ copySelf = true; }
    moveSelfIntoFirstChild();

    // node erneut Aufsetzen
    Relation = TkType::Operator;
    Operator = operation;

    //
    children.emplace_back(copySelf ? children.back() : other);

    tensorOrder = (children.begin()->tensorOrder < 0 || children.back().tensorOrder < 0) ? -1 :
                    children.begin()->tensorOrder + children.back().tensorOrder;
}

void TensorExpression::diffAssign(const TensorExpression& other){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::Diff;

    //
    bool copySelf = false;
    
    //
    bool IsRepresentableByTemplate = false;
    auto it = tensorExpressionDiffTemplates.begin();

    // Check ob Expression durch ein abgespeichertes Template repräsentiert werden kann
    for (; it != tensorExpressionDiffTemplates.end(); ++it) {

        if (it->first.first == *this && it->first.second == other) 
        {
            IsRepresentableByTemplate = true;

            // Checke ob der Ausdruck durch das Template auch für mehrfache Vorkommen einzelner TemplateInstanzen
            // zb. <A> .. <B> .. <A> repräsentiert werden kann
            static substitutionMap subsMap = {};
            subsMap.clear();

            bool isRepresentationConsistent = true
                && assembleSubstitutionMap(it->first.first, *this, subsMap)
                && assembleSubstitutionMap(it->first.second, other, subsMap);

            if(isRepresentationConsistent){ 

                break;
            }
        }
    }

    //
    if(tensorExpressionDiffs.contains(std::make_pair(*this, other))){

        *this = tensorExpressionDiffs[std::make_pair(*this, other)];
    }
    else if(IsRepresentableByTemplate){

        // Aufstellen einer Substitutionstabelle mit <template label : Ersetzungs TensorExpression>
        // Kopie des Template Ausdrucks, in diesem findet die Ersetzung statt
        // >> Rebuild der ursprünglichen Kopie (nicht mehr templatiert)

        // *this <> it->first.first | other <> it->first.second | result <> it->second

        // result rekursiv durchlaufen und substituieren wenn nötig
        TensorExpression res = it->second;

        //
        substitutionMap subsMap;
        assembleSubstitutionMap(it->first.first, *this, subsMap);
        assembleSubstitutionMap(it->first.second, other, subsMap);

        // for(const auto& [k, v] : subsMap){

        //     LOG << k.toString() << " <> " << v.toString() << endl;
        // }

        //
        replaceBySubstitutions(res, subsMap);

        //
        *this = res.rebuild();
    }
    else if(*this == other){

        *this = TensorExpression("Identity", this->tensorOrder);
    }
    else if(((Relation == TkType::Argument && other.Relation == TkType::Argument) ||
            (Operator == TensorExpressionOperator::Diff || Operator == TensorExpressionOperator::Multiplication))){

        rawDiffAssign(other);
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
        else{

            rawDiffAssign(other);
        }
    }
    else{

        rawDiffAssign(other);
    }
}

//
void TensorExpression::convertToTemplate(){

    // Dead Ends Templatisieren
    if(Relation == TkType::Argument){

        Relation = TkType::Container;
        Operator = TensorExpressionOperator::Arbitary;
    }

    for(auto& child : children){

        child.convertToTemplate();
    }
}

void TensorExpression::convertToConstantTemplate(){

    //
    *this = TensorExpression(minCnstLimit);
}

bool TensorExpression::isTemplatedNode() const{

    if(Relation == TkType::Container && Operator == TensorExpressionOperator::Arbitary){

        return true;
    }

    return false;
}

bool TensorExpression::isTemplate() const{

    //
    if(isTemplatedNode()){
        return true;
    }

    for(const auto& child : children){

        if(child.isTemplate()){
            return true;
        }
    }

    return false;
}

bool TensorExpression::isConstantTemplate() const {

    if(!isConstant){ return false; }
    
    return value == minCnstLimit;
}

//
std::string TensorExpression::toString(size_t depth) const{

    //
    std::string res;

    res += depth == 0 ? "TensorExpression[" + std::to_string(tensorOrder) + "] = " : "";

    // Argument node
    if(Relation == TkType::Argument && !isConstant){

        res += label + "[" + std::to_string(tensorOrder) + "]";
    }
    else if(Relation == TkType::Argument && isConstantTemplate()){

        res += "<CnstTmpl>[0]";
    }
    // Constant node
    else if(Relation == TkType::Argument){

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(6) << value;
        std::string str = oss.str();

        // Entferne trailing zeros
        str.erase(str.find_last_not_of('0') + 1, std::string::npos);

        // Entferne trailing '.' wenn vorhanden
        if(str.back() == '.') str.pop_back();

        res += str;
    }
    // Container
    else if(Relation == TkType::Operator && children.size() == 1){

        if(TensorExpressionOperatorStrings.contains(Operator)){

            res += "(" + children.begin()->toString(depth+1) + ")" + TensorExpressionOperatorStrings[Operator] + " ";
        }
        else{

            res += std::string(magic_enum::enum_name(Operator)) + "(" + children.begin()->toString(depth+1) + ")";
        }
        
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
    else if(isTemplatedNode()){

        res += "<" + label + ">";  
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

        //
        registerFunction("tExpr", {INT::typeIndex},
            [__functionLabel__ = "tExpr", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_RETURN(TENSOR_EXPRESSION, 0);
                GET_ARG(INT, 0);

                // schreiben in returns
                ret0->getMember() = TensorExpression(static_cast<double>(arg0->getMember()));
        },
        {TENSOR_EXPRESSION::typeIndex});

        //
        registerFunction("tExpr", {DOUBLE::typeIndex},
            [__functionLabel__ = "tExpr", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_RETURN(TENSOR_EXPRESSION, 0);
                GET_ARG(DOUBLE, 0);

                // schreiben in returns
                ret0->getMember() = TensorExpression(arg0->getMember());
        },
        {TENSOR_EXPRESSION::typeIndex});

        // Konstruktoren
        registerFunction("tExprTmpl", {STRING::typeIndex, INT::typeIndex},
            [__functionLabel__ = "tExprTmpl", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_RETURN(TENSOR_EXPRESSION, 0);
                GET_ARG(STRING, 0); GET_ARG(INT, 1);

                // schreiben in returns
                ret0->getMember() = TensorExpression(arg0->getMember(), arg1->getMember());
                ret0->getMember().convertToTemplate();
        },
        {TENSOR_EXPRESSION::typeIndex});

        // Konstruktoren
        registerFunction("tExprCnstTmpl", {},
            [__functionLabel__ = "tExprCnstTmpl", __numArgs__ = 0](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_RETURN(TENSOR_EXPRESSION, 0);

                // schreiben in returns
                ret0->getMember() = TensorExpression();
                ret0->getMember().convertToConstantTemplate();
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

        //
        registerFunction("__addAssign__", {DOUBLE::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__addAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                DOUBLE* formerMb = static_cast<DOUBLE*>(inputs[0]->getData()); 

                //
                inputs[0]->getVariableRef().constructByObject(new TENSOR_EXPRESSION(new TensorExpression(formerMb->getMember())));

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                TensorExpression& member0 = arg0->getMember();
                TensorExpression& member1 = arg1->getMember();

                member0.addAssign(member1);
        },
        {});

        //
        registerFunction("__addAssign__", {TENSOR_EXPRESSION::typeIndex, DOUBLE::typeIndex},
            [__functionLabel__ = "__addAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(DOUBLE, 1);

                TensorExpression& member0 = arg0->getMember();

                member0.addAssign(TensorExpression(arg1->getMember()));
        },
        {});

        //
        registerFunction("__addAssign__", {INT::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__addAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                INT* formerMb = static_cast<INT*>(inputs[0]->getData()); 

                //
                inputs[0]->getVariableRef().constructByObject(new TENSOR_EXPRESSION(new TensorExpression(static_cast<double>(formerMb->getMember()))));

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                TensorExpression& member0 = arg0->getMember();
                TensorExpression& member1 = arg1->getMember();

                member0.addAssign(member1);
        },
        {});

        //
        registerFunction("__addAssign__", {TENSOR_EXPRESSION::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__addAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(INT, 1);

                TensorExpression& member0 = arg0->getMember();

                member0.addAssign(TensorExpression(static_cast<double>(arg1->getMember())));
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

        //
        registerFunction("__subAssign__", {DOUBLE::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__subAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                DOUBLE* formerMb = static_cast<DOUBLE*>(inputs[0]->getData()); 

                //
                inputs[0]->getVariableRef().constructByObject(new TENSOR_EXPRESSION(new TensorExpression(formerMb->getMember())));

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                TensorExpression& member0 = arg0->getMember();
                TensorExpression& member1 = arg1->getMember();

                member0.subAssign(member1);
        },
        {});

        //
        registerFunction("__subAssign__", {TENSOR_EXPRESSION::typeIndex, DOUBLE::typeIndex},
            [__functionLabel__ = "__subAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(DOUBLE, 1);

                TensorExpression& member0 = arg0->getMember();

                member0.subAssign(TensorExpression(arg1->getMember()));
        },
        {});

        //
        registerFunction("__subAssign__", {INT::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__subAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                INT* formerMb = static_cast<INT*>(inputs[0]->getData()); 

                //
                inputs[0]->getVariableRef().constructByObject(new TENSOR_EXPRESSION(new TensorExpression(static_cast<double>(formerMb->getMember()))));

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                TensorExpression& member0 = arg0->getMember();
                TensorExpression& member1 = arg1->getMember();

                member0.subAssign(member1);
        },
        {});

        //
        registerFunction("__subAssign__", {TENSOR_EXPRESSION::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__subAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(INT, 1);

                TensorExpression& member0 = arg0->getMember();

                member0.subAssign(TensorExpression(static_cast<double>(arg1->getMember())));
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

        //
        registerFunction("__mulAssign__", {DOUBLE::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__mulAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                DOUBLE* formerMb = static_cast<DOUBLE*>(inputs[0]->getData()); 

                //
                inputs[0]->getVariableRef().constructByObject(new TENSOR_EXPRESSION(new TensorExpression(formerMb->getMember())));

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                TensorExpression& member0 = arg0->getMember();
                TensorExpression& member1 = arg1->getMember();

                member0.mulAssign(member1);
        },
        {});

        //
        registerFunction("__mulAssign__", {TENSOR_EXPRESSION::typeIndex, DOUBLE::typeIndex},
            [__functionLabel__ = "__mulAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(DOUBLE, 1);

                TensorExpression& member0 = arg0->getMember();

                member0.mulAssign(TensorExpression(arg1->getMember()));
        },
        {});

        //
        registerFunction("__mulAssign__", {INT::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__mulAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                INT* formerMb = static_cast<INT*>(inputs[0]->getData()); 

                //
                inputs[0]->getVariableRef().constructByObject(new TENSOR_EXPRESSION(new TensorExpression(static_cast<double>(formerMb->getMember()))));

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                TensorExpression& member0 = arg0->getMember();
                TensorExpression& member1 = arg1->getMember();

                member0.mulAssign(member1);
        },
        {});

        //
        registerFunction("__mulAssign__", {TENSOR_EXPRESSION::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__mulAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(INT, 1);

                TensorExpression& member0 = arg0->getMember();

                member0.mulAssign(TensorExpression(static_cast<double>(arg1->getMember())));
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
        registerFunction("inverse", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "inverse", __numArgs__ = 1](FREG_ARGS){

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
        registerFunction("transpose", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "transpose", __numArgs__ = 1](FREG_ARGS){

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
        registerFunction("zeros", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "zeros", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                if(inputs[0]->isLValue()){ returns[0].cloneIntoRValue(inputs[0]->getVariableRef()); }
                else{ returns[0].moveIntoRValue(inputs[0]->getVariableRef()); }

                GET_RETURN(TENSOR_EXPRESSION, 0);
                ret0->getMember().zerosAssign();
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

        //
        registerFunction("trace", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "trace", __numArgs__ = 1](FREG_ARGS){

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

        //
        registerFunction("det", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "det", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                if(inputs[0]->isLValue()){ returns[0].cloneIntoRValue(inputs[0]->getVariableRef()); }
                else{ returns[0].moveIntoRValue(inputs[0]->getVariableRef()); }

                GET_RETURN(TENSOR_EXPRESSION, 0);
                ret0->getMember().determinantAssign();
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

        registerFunction("trace", {TENSOR_EXPRESSION::typeIndex, INT::typeIndex},
            [__functionLabel__ = "trace", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                if(inputs[0]->isLValue()){ returns[0].cloneIntoRValue(inputs[0]->getVariableRef()); }
                else{ returns[0].moveIntoRValue(inputs[0]->getVariableRef()); }

                GET_RETURN(TENSOR_EXPRESSION, 0); GET_ARG(INT, 1);
                
                ret0->getMember().traceAssign(arg1->getMember());
        },
        {TENSOR_EXPRESSION::typeIndex});

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
        registerFunction("__sectionAssign__", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__sectionAssign__", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                if(inputs[0]->isLValue()){ returns[0].cloneIntoRValue(inputs[0]->getVariableRef()); }
                else{ returns[0].moveIntoRValue(inputs[0]->getVariableRef()); }

                GET_RETURN(TENSOR_EXPRESSION, 0);
                ret0->getMember().sectionAssign();
        },
        {TENSOR_EXPRESSION::typeIndex});

        //
        registerFunction("diff", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "diff", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                //
                returns[0].getVariableRef().clone(inputs[0]->getVariableRef());

                // Returns
                GET_RETURN(TENSOR_EXPRESSION, 0);
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                ret0->getMember().diffAssign(arg1->getMember());
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
        registerMemberFunction(TENSOR_EXPRESSION::typeIndex, "toTemplate", {},
            [__functionLabel__ = "toTemplate", __numArgs__ = 0](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;
            
                // schreiben in returns
                returns[0].cloneIntoRValue(member->getVariableRef());

                //
                GET_RETURN(TENSOR_EXPRESSION, 0);

                ret0->getMember().convertToTemplate();

        },
        {TENSOR_EXPRESSION::typeIndex});

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
                
                // RETURNING_ASSERT((member0.isTemplate() || member1.isTemplate()) && member2.isTemplate(),
                //                   "In übergebenen Termen sind keine templatierten Nodes vorhanden",);

                RETURNING_ASSERT(tensorExpressionDiffTemplates.try_emplace(std::make_pair(member0, member1), member2).second,
                                 "Differential für gegebenes Tensortemplatepaar bereits gesetzt : " +
                                 member0.toString() + "|" + member1.toString() + " -> " + member2.toString(),);
        },
        {});

        //
        registerFunction("setEqual", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "setEqual", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                TensorExpression& member0 = arg0->getMember(), member1 = arg1->getMember();

                RETURNING_ASSERT(tensorExpressionSimplifications.try_emplace(member0, member1).second,
                                 "Simplification für gegebenes Tensorpaar bereits gesetzt",);
        },
        {});

        //
        registerFunction("logDiffTemplates", {},
            [__functionLabel__ = "logDiffTemplates", __numArgs__ = 0](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                LOG << "Found " << tensorExpressionDiffTemplates.size() << " registered Diff Templates :" << endl;

                for(const auto& [k,v] : tensorExpressionDiffTemplates){

                    LOG << "diff[ " << k.first.toString(1) << ", " << k.second.toString(1) << " ] = " << v.toString(1) << endl;
                }

                LOG << endl;
        },
        {});

        //
        registerFunction("logSimplifications", {},
            [__functionLabel__ = "logSimplifications", __numArgs__ = 0](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                LOG << "Found " << tensorExpressionSimplifications.size() << " registered Simplifications :" << endl;

                for(const auto& [k,v] : tensorExpressionSimplifications){

                    LOG << k.toString(1) << " => " << v.toString(1) << endl;
                }

                LOG << endl;
        },
        {});

        //
        registerFunction("rebuild", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "rebuild", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_RETURN(TENSOR_EXPRESSION, 0);
                GET_ARG(TENSOR_EXPRESSION, 0)

                ret0->getMember() = arg0->getMember().rebuild();
        },
        {TENSOR_EXPRESSION::typeIndex});

        //
        registerFunction("simplifyOnce", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "simplifyOnce", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_RETURN(BOOL, 0);
                GET_ARG(TENSOR_EXPRESSION, 0)

                ret0->getMember() = arg0->getMember().simplifyOnce();
        },
        {BOOL::typeIndex});

        //
        registerFunction("simplify", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "simplify", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_ARG(TENSOR_EXPRESSION, 0)

                arg0->getMember().simplify();
        },
        {});

        //
        registerFunction("setUnwrapOperands", {BOOL::typeIndex},
            [__functionLabel__ = "setUnwrapOperands", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                //
                GET_ARG(BOOL, 0);

                //
                unwrapOperands = arg0->getMember();
        },
        {});

        //
        registerFunction("unwrap", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "unwrap", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_RETURN(TENSOR_EXPRESSION, 0);
                GET_ARG(TENSOR_EXPRESSION, 0)

                ret0->getMember() = arg0->getMember().unwrap();
        },
        {TENSOR_EXPRESSION::typeIndex});

        // Operatoren
        registerFunction("__smaller__", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__smaller__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                
                returns.emplace_back();
                returns[returns.size() - 1].constructRValueByObject(constructRegisteredType(functionReturnTypes[0]));

                // Returns | Inputs
                BOOL* ret0 = static_cast<BOOL*>(returns[returns.size()-1].getVariableRef().getData());

                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                // schreiben in returns
                ret0->getMember() = arg0->getMember() < arg1->getMember();
        },
        {BOOL::typeIndex});

        return true;
    }
}