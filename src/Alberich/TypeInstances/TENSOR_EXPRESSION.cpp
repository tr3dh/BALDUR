#include "TENSOR_EXPRESSION.h"
#include "PermutationGenerator.h"

bool unwrapOperands = false;

bool g_compareTemplateDependencies = false;

std::map<TensorExpressionOperator, std::string> TensorExpressionOperatorStrings = {

    {TensorExpressionOperator::None, "INV_OPS"},

    {TensorExpressionOperator::Addition, "+"},
    {TensorExpressionOperator::Subtraction, "-"},
    {TensorExpressionOperator::Multiplication, "*"},

    {TensorExpressionOperator::DotProduct, "."},
    {TensorExpressionOperator::ContractingDotProduct, ".n"},
    {TensorExpressionOperator::CrossProduct, "x"},
    {TensorExpressionOperator::DyadicProduct, "(x)"},
    {TensorExpressionOperator::CrossingDoubleContraction, ":"},
    {TensorExpressionOperator::MirroringDoubleContraction, ".."},
};

std::map<TensorExpressionOperator, void (TensorExpression::*)(const TensorExpression&)> operatorMemberFunctions = {

    {TensorExpressionOperator::Addition, &TensorExpression::addAssign},
    {TensorExpressionOperator::Subtraction, &TensorExpression::subAssign},
    {TensorExpressionOperator::DotProduct, &TensorExpression::dotProductAssign},
    {TensorExpressionOperator::ContractingDotProduct, &TensorExpression::contractingDotProductAssign},
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
    {TensorExpressionOperator::Frobenius, &TensorExpression::frobeniusAssign},
    {TensorExpressionOperator::Zeros, &TensorExpression::zerosAssign},
    {TensorExpressionOperator::Ones, &TensorExpression::onesAssign},
    {TensorExpressionOperator::Identity, &TensorExpression::identityAssign},
    {TensorExpressionOperator::Macaulay, &TensorExpression::macaulayAssign},
    {TensorExpressionOperator::Signum, &TensorExpression::signumAssign},
    {TensorExpressionOperator::Sqrt, &TensorExpression::sqrtAssign},
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

TensorExpression TensorExpression::asExternalNode(const std::string& label){

    return TensorExpression(label, tensorOrder, dimensions);
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
    else if(lhsIsTemplate && rhsIsTemplate){

        // Wenn Instance und Arg Template vorhanden soll Instance Template überladen werden
        if((lhs.isInstanceTemplate() && rhs.isArgTemplate()) || (lhs.isArgTemplate() && rhs.isInstanceTemplate())){

            return lhs.isInstanceTemplate();
        }
        else if((lhs.isConstantTemplate() && rhs.isArgTemplate()) || (lhs.isArgTemplate() && rhs.isConstantTemplate())){

            return lhs.isConstantTemplate();
        }

        // Der Vergleich zwischen Instance und Constant Template ist tatsächlich irrelevant da sie sich
        // anderweitig ausreichend unterscheiden und die Überladung trivial ist
    }

    //
    if (lhs.Relation != rhs.Relation){ return lhs.Relation < rhs.Relation; }
    if (lhs.Operator != rhs.Operator){ return lhs.Operator < rhs.Operator; }

    if(lhs.isConstant && rhs.isConstant && lhs.value != rhs.value){ return lhs.value > rhs.value; }
    else if(lhs.isConstant && !rhs.isConstant && rhs.Relation == TkType::Argument){ return true; }
    else if(!lhs.isConstant && lhs.Relation == TkType::Argument && rhs.isConstant){ return false; }

    if((!lhsIsTemplate && !rhsIsTemplate && lhs.label != rhs.label)){
        return lhs.label < rhs.label;
    }

    // Überladung nach Tensorstufe
    if(lhs.tensorOrder != rhs.tensorOrder){ return lhs.tensorOrder > rhs.tensorOrder; }

    //
    const auto& a = lhs.children;
    const auto& b = rhs.children;

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
    if(lhs.label != rhs.label){

        return lhs.label < rhs.label;
    }

    //
    if(lhs.containsDimensions() && rhs.containsDimensions()){
        
        return lhs.dimensions < rhs.dimensions;
    }

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

std::vector<const TensorExpression*> TensorExpression::getUniqueExternalNodes() const{

    //
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

    return uniqueNodes;
}

size_t TensorExpression::getNumOfUniqueNodes() const{

    return getUniqueExternalNodes().size();
}

size_t TensorExpression::getNumOfNodes() const{

    size_t numOfNodes = children.size();

    for(const auto& child : children){

        numOfNodes += child.getNumOfNodes();
    }

    return numOfNodes;
}

size_t TensorExpression::getNumOfExternalNodes() const{

    if(children.empty()){ return 1; }

    size_t numOfExternalNodes = 0;

    for(const auto& child : children){

        numOfExternalNodes += child.getNumOfExternalNodes();
    }

    return numOfExternalNodes;
}

// Statics
void TensorExpression::replaceBySubstitutions(TensorExpression& expr, const substitutionMap& subsMap){

    // Konstanten müssen nicht ersetzt werden
    if(expr.isConstant && !expr.isConstantTemplate()){ return; }
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

// Statics
void TensorExpression::rawReplaceBySubstitutions(TensorExpression& expr, const substitutionMap& subsMap){

    for(const auto& [subsKey, subsVal] : subsMap){
        
        //
        if(expr == subsKey){

            expr = subsVal;
            return;
        }
    }

    for(auto& child : expr.children){

        rawReplaceBySubstitutions(child, subsMap);
    }
}

bool TensorExpression::assembleSubstitutionMap(const TensorExpression& tmplExpr, const TensorExpression& expr, substitutionMap& subsMap, bool disableLog){

    bool res = true;

    RETURNING_ASSERT(tmplExpr == expr, "Ungleiche Operanden für Template Substitution " + tmplExpr.toString() + " " + expr.toString(), false);
    // RETURNING_ASSERT(tmplExpr.children.size() == expr.children.size() || tmplExpr.children.size() == 0,
    //                  "Ungleiche Operanden Childs für Template Substitution", false);

    if(!tmplExpr.isTemplate()){ 
        
        return true;
    }

    bool tmplIsTemplatedNode = tmplExpr.isTemplatedNode() || tmplExpr.isConstantTemplate() || tmplExpr.isInstanceTemplate() || tmplExpr.isArgTemplate();

    // Einkommentieren für Tmpl Dependencies
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    if(g_compareTemplateDependencies){

        if(tmplExpr.tensorOrder == -1 &&
            ((tmplExpr.Operator == TensorExpressionOperator::Zeros && expr.label == "zeros") ||
            (tmplExpr.Operator == TensorExpressionOperator::Ones && expr.label == "ones") ||
            (tmplExpr.Operator == TensorExpressionOperator::Identity && expr.label == "Identity"))){

            return true;
        }
    }

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    // LOG << tmplExpr.toString() << " <> " << expr.toString() << " repl? " << tmplIsTemplatedNode << endl;

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

            res = res && assembleSubstitutionMap(tmplExpr.children[i], expr.children[i], subsMap, true);
        }
    }
    else{

        size_t N = tmplExpr.children.size();
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

                res = res && assembleSubstitutionMap(tmplExpr.children[i], expr.children[allPermutations[matchingPermutation][i]], subsMap, true);
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

TensorExpression::TensorExpression(const std::string& labelIn, int tensorOrderIn, const std::vector<int>& dimensionsIn) :
    TensorExpression(labelIn, tensorOrderIn){

    //
    RETURNING_ASSERT(tensorOrder == dimensionsIn.size(), "build von " + toString() + " failed",);

    //
    dimensions = dimensionsIn;
}

TensorExpression::TensorExpression(float valueIn) : value(valueIn){

    Relation = TkType::Argument;
    tensorOrder = 0;
    isConstant = true;
}

bool TensorExpression::containsDimensions() const {

    return dimensions.size() == tensorOrder;
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

bool TensorExpression::contains(const TensorExpression& other) const{

    if(*this == other){ return true; }
    else{

        for(const auto& child : children){

            if(child.contains(other)){
                return true;
            }
        }
    }

    return false;
}

int TensorExpression::countOccurences(const TensorExpression& other) const{

    // Gleichheit entspricht einem Vorkommens
    // Bei Ungleichheit mit einer externen Node (Argument) kann diese keine Vorkommen enthalten
    if(*this == other){ return 1; }
    else if(Relation == TkType::Argument){ return 0; }

    //
    int occurences = 0;

    //
    for(const auto& child : children){

        occurences += child.countOccurences(other);
    }

    return occurences;
}

//
bool TensorExpression::isUnWrapped() const{

    //
    if(children.size() > 2){
        
        return true;
    }

    //
    for(const auto& child : children){

        if(child.isUnWrapped()){

            return true;
        }
    }

    //
    return false;
}

//
bool TensorExpression::isWrapped() const{

    return !isUnWrapped();
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

TensorExpression TensorExpression::wrap() const{

    bool storedFlag = unwrapOperands;
    unwrapOperands = false;

    TensorExpression res = rebuild();
    unwrapOperands = storedFlag;

    return res;
}

void TensorExpression::simplify(){

    const char spinner[] = {'|', '/', '-', '\\'};
    int spinnerIdx = 0;
    int step = 0;

    LOG << "\r" << spinner[spinnerIdx % 4] << " - smplf step " << step << std::flush;
    spinnerIdx++;
    step++;

    while(simplifyOnce()){

        LOG << "\r" << spinner[spinnerIdx % 4] << " - smplf step " << step << std::flush;
        spinnerIdx++;
        step++;
    }

    LOG << "\r" << std::string(100, ' ') << "\r" << std::flush;
}

//
bool TensorExpression::simplifyOnce(){

    bool IsRepresentableByTemplate;

    RETURNING_ASSERT(!isTemplate(), "Templates sind nicht für Simplifizierungen ausgelegt", false);

    for(const auto& [k, v] : tensorExpressionSimplifications){

        if(k == *this){

            // LOG << "found tmpl " << k.toString(1) << " <> " << toString(1) << endl;

            auto prevOrder = tensorOrder;

            if(!k.isTemplate()){

                // RETURNING_ASSERT(prevOrder == v.tensorOrder, "Inkonsistentes Umformungstemplate", false);
                *this = v;

                return true;
            }
            else{

                IsRepresentableByTemplate = true;

                // Checke ob der Ausdruck durch das Template auch für mehrfache Vorkommen einzelner TemplateInstanzen
                // zb. <A> .. <B> .. <A> repräsentiert werden kann
                static substitutionMap subsMap = {};
                subsMap.clear();

                bool isRepresentationConsistent = true
                    && assembleSubstitutionMap(k, *this, subsMap, true);

                if(!isRepresentationConsistent){

                    continue;
                }

                // Aufstellen einer Substitutionstabelle mit <template label : Ersetzungs TensorExpression>
                // Kopie des Template Ausdrucks, in diesem findet die Ersetzung statt
                // >> Rebuild der ursprünglichen Kopie (nicht mehr templatiert)

                // *this <> k | result <> v

                // result rekursiv durchlaufen und substituieren wenn nötig

                // Einkommentieren für Tmpl Dependencies
                // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

                if(g_compareTemplateDependencies){

                    if(k.containsTemplateDependencie()){

                        // disable Assertion Logging
                        DISABLE_ASSERTION_LOGGING();

                        // Konsistenz Check
                        bool isRepresentationDependencieSafe = true;
                        TensorExpression source = k;

                        // for(const auto& [k, v] : subsMap){

                        //     LOG << k.toString() << " <> " << v.toString() << endl;
                        // }

                        //
                        replaceBySubstitutions(source, subsMap);
                        source = source.rebuild();

                        //
                        if(!(*this == source)){
                            isRepresentationDependencieSafe = false;
                        }

                        //
                        RESET_ASSERTION_LOGGING();

                        //
                        if(!isRepresentationDependencieSafe){ continue; }
                    }
                }

                // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

                TensorExpression res = v;
                replaceBySubstitutions(res, subsMap);

                //
                *this = res.rebuild();

                return true;
            }
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
    RETURNING_ASSERT(tensorOrder == other.tensorOrder || tensorOrder == -1 || other.tensorOrder == -1,
        "Addition von Tensoren unterschiedlicher Stufe versucht : " + toString(0) + " | " + other.toString(0),);
    
    //
    if(!isValid()){

        *this = other;
        return;
    }
    else if(!other.isValid())
    {
        return;
    }

    bool thisIsDestinedOperation = Relation == TkType::Operator && Operator == operation;
    bool thisIsConstantInstance = isConstant && !isConstantTemplate();
    bool otherIsConstantInstance = other.isConstant && !other.isConstantTemplate();

    // Wenn zwei Konstantennodes beaufschlagt werden kann wert einfach direkt bearbeitet werden
    if(thisIsConstantInstance && otherIsConstantInstance){

        value += other.value;
        return;
    }
    // Wenn Operation der aufgerufenen entspricht und ein child eine nicht template konstante ist
    // wird Wert einfach beaufschlagt
    else if(thisIsDestinedOperation && otherIsConstantInstance){

        for(auto& child : children){

            if(child.isConstant && !child.isConstantTemplate()){

                child.value += other.value;
                return;
            }
        }
    }

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
        dimensions = children.begin()->dimensions;
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
    const TensorExpression& otherMember = children.back();

    // Aufgrund des movIntoSelf ist tensorOrder eh -1 wenn erster Operand -1 als tensorOrder hat
    if(otherMember.tensorOrder == -1){

        tensorOrder = -1;
    }

    // Wenn Dimensionen von einem Operanden nicht berücksicht werden berücksichtigt das Ergebnis sie auch nicht
    if(tensorOrder > -1 && !(containsDimensions() && other.containsDimensions())){ dimensions.clear(); }
    
    // Regeln und Logik falls beide Operanden die Dimensionen berücksichtigen
    else if(tensorOrder > -1){

        RETURNING_ASSERT(dimensions == otherMember.dimensions, "...",);
    }
}

//
void TensorExpression::subAssign(const TensorExpression& other){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::Subtraction;

    // ASSERTS
    RETURNING_ASSERT(tensorOrder == other.tensorOrder || tensorOrder == -1 || other.tensorOrder == -1, "Addition von Tensoren unterschiedlicher Stufe versucht",);

    //
    if(!isValid()){

        *this = other;
        return;
    }
    else if(!other.isValid())
    {
        return;
    }

    bool thisIsDestinedOperation = Relation == TkType::Operator && Operator == operation;
    bool thisIsConstantInstance = isConstant && !isConstantTemplate();
    bool otherIsConstantInstance = other.isConstant && !other.isConstantTemplate();

    // Wenn zwei Konstantennodes beaufschlagt werden kann wert einfach direkt bearbeitet werden
    if(thisIsConstantInstance && otherIsConstantInstance){

        value -= other.value;
        return;
    }
    // Wenn Operation der aufgerufenen entspricht und ein child eine nicht template konstante ist
    // wird Wert einfach beaufschlagt
    else if(thisIsDestinedOperation && otherIsConstantInstance){

        for(auto& child : children){

            if(child.isConstant && !child.isConstantTemplate()){

                child.value -= other.value;
                return;
            }
        }
    }

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
        dimensions = children.begin()->dimensions;
    }

    //
    children.emplace_back(copySelf ? children.back() : other);

    // Anpassen TensorOrder
    // hier nicht nötig aufgrund von Addition
    // ...

    //
    const TensorExpression& otherMember = children.back();

    // Aufgrund des movIntoSelf ist tensorOrder eh -1 wenn erster Operand -1 als tensorOrder hat
    if(otherMember.tensorOrder == -1){

        tensorOrder = -1;
    }

    // Wenn Dimensionen von einem Operanden nicht berücksicht werden berücksichtigt das Ergebnis sie auch nicht
    if(tensorOrder > -1 && !(containsDimensions() && other.containsDimensions())){ dimensions.clear(); }
    
    // Regeln und Logik falls beide Operanden die Dimensionen berücksichtigen
    else if(tensorOrder > -1){

        RETURNING_ASSERT(dimensions == otherMember.dimensions, "...",);
    }
}

void TensorExpression::mulAssign(const TensorExpression& other){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::Multiplication;

    // // ASSERTS
    // RETURNING_ASSERT(tensorOrder == 0 || other.tensorOrder == 0 || tensorOrder == -1 || other.tensorOrder == -1, "Skalar Multiplikation ohne Skalar versucht",);

    //
    if(tensorOrder > 0 && other.tensorOrder > 0){

        return dyadProductAssign(other);
    }

    //
    if(!isValid()){

        *this = other;
        return;
    }
    else if(!other.isValid())
    {
        return;
    }

    bool thisIsDestinedOperation = Relation == TkType::Operator && Operator == operation;
    bool thisIsConstantInstance = isConstant && !isConstantTemplate();
    bool otherIsConstantInstance = other.isConstant && !other.isConstantTemplate();

    // Wenn zwei Konstantennodes beaufschlagt werden kann wert einfach direkt bearbeitet werden
    if(thisIsConstantInstance && otherIsConstantInstance){

        value *= other.value;
        return;
    }
    // Wenn Operation der aufgerufenen entspricht und ein child eine nicht template konstante ist
    // wird Wert einfach beaufschlagt
    else if(thisIsDestinedOperation && otherIsConstantInstance){

        for(auto& child : children){

            if(child.isConstant && !child.isConstantTemplate()){

                child.value *= other.value;
                return;
            }
        }
    }

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
        dimensions = children.begin()->dimensions;
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
    const TensorExpression& otherMember = children.back();

    // Aufgrund des movIntoSelf ist tensorOrder eh -1 wenn erster Operand -1 als tensorOrder hat
    if(otherMember.tensorOrder == -1){

        tensorOrder = -1;
    }

    // Wenn Dimensionen von einem Operanden nicht berücksicht werden berücksichtigt das Ergebnis sie auch nicht
    if(tensorOrder > -1 && !(containsDimensions() && other.containsDimensions())){

        dimensions.clear();

        tensorOrder = otherMember.tensorOrder > tensorOrder ? otherMember.tensorOrder : tensorOrder;
    }

    // Regeln und Logik falls beide Operanden die Dimensionen berücksichtigen
    else if(tensorOrder > -1){

        dimensions = dimensions.empty() ? otherMember.dimensions : dimensions;
        tensorOrder = otherMember.tensorOrder > tensorOrder ? otherMember.tensorOrder : tensorOrder;
    }
}

void TensorExpression::dotProductAssign(const TensorExpression& other){
    
    //
    static TensorExpressionOperator operation = TensorExpressionOperator::DotProduct;

    // ASSERTS
    RETURNING_ASSERT((tensorOrder > 0 && other.tensorOrder > 0) || tensorOrder == -1 || other.tensorOrder == -1,
    "Tensoren mit Stufe kleiner 1 and Skalarprodukt beteiligt " + toString() + " | " + other.toString(),);

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
        dimensions = children.begin()->dimensions;
    }

    //
    children.emplace_back(copySelf ? children.back() : other);

    // Anpassen TensorOrder

    //
    const TensorExpression& otherMember = children.back();

    // Aufgrund des movIntoSelf ist tensorOrder eh -1 wenn erster Operand -1 als tensorOrder hat
    if(otherMember.tensorOrder == -1){

        tensorOrder = -1;
    }

    // Wenn Dimensionen von einem Operanden nicht berücksicht werden berücksichtigt das Ergebnis sie auch nicht
    if(tensorOrder > -1 && !(containsDimensions() && other.containsDimensions())){

        dimensions.clear();

        tensorOrder = tensorOrder + otherMember.tensorOrder - 2;
    }

    // Regeln und Logik falls beide Operanden die Dimensionen berücksichtigen
    else if(tensorOrder > -1){

        // Für Reversed Check : std::make_reverse_iterator(otherMember.dimensions.begin() + contractNIndices)
        RETURNING_ASSERT(std::equal(dimensions.end() - 1, dimensions.end(), otherMember.dimensions.begin()), "...",);

        dimensions.erase(dimensions.end() - 1, dimensions.end());
        dimensions.insert(dimensions.end(), otherMember.dimensions.begin() + 1, otherMember.dimensions.end());

        tensorOrder = tensorOrder + otherMember.tensorOrder - 2;
    }
}

void TensorExpression::contractingDotProductAssign(const TensorExpression& other){
    
    //
    static TensorExpressionOperator operation = TensorExpressionOperator::ContractingDotProduct;

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
        dimensions = children.begin()->dimensions;
    }

    //
    children.emplace_back(copySelf ? children.back() : other);

    // Anpassen TensorOrder

    //
    const TensorExpression& otherMember = children.back();

    // Aufgrund des movIntoSelf ist tensorOrder eh -1 wenn erster Operand -1 als tensorOrder hat
    if(otherMember.tensorOrder == -1){

        tensorOrder = -1;
    }

    // Wenn Dimensionen von einem Operanden nicht berücksicht werden berücksichtigt das Ergebnis sie auch nicht
    if(tensorOrder > -1 && !(containsDimensions() && other.containsDimensions())){

        dimensions.clear();

        tensorOrder = tensorOrder + otherMember.tensorOrder - 2 * std::min(tensorOrder, otherMember.tensorOrder);
    }

    // Regeln und Logik falls beide Operanden die Dimensionen berücksichtigen
    else if(tensorOrder > -1){

        //
        int contractIndices = std::min(tensorOrder, otherMember.tensorOrder);

        // Für Reversed Check : std::make_reverse_iterator(otherMember.dimensions.begin() + contractNIndices)
        RETURNING_ASSERT(std::equal(dimensions.end() - contractIndices, dimensions.end(), otherMember.dimensions.begin()), "...",);

        dimensions.erase(dimensions.end() - contractIndices, dimensions.end());
        dimensions.insert(dimensions.end(), otherMember.dimensions.begin() + contractIndices, otherMember.dimensions.end());

        tensorOrder = tensorOrder + otherMember.tensorOrder - 2 * contractIndices;
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
        dimensions = children.begin()->dimensions;
    }

    //
    children.emplace_back(copySelf ? children.back() : other);

    //
    const TensorExpression& otherMember = children.back();

    // Anpassen TensorOrder
    // Aufgrund des movIntoSelf ist tensorOrder eh -1 wenn erster Operand -1 als tensorOrder hat
    if(otherMember.tensorOrder == -1){

        tensorOrder = -1;
    }

    // Wenn Dimensionen von einem Operanden nicht berücksicht werden berücksichtigt das Ergebnis sie auch nicht
    if(tensorOrder > -1 && !(containsDimensions() && other.containsDimensions())){ dimensions.clear(); }
    
    // Regeln und Logik falls beide Operanden die Dimensionen berücksichtigen
    else if(tensorOrder > -1){

        RETURNING_ASSERT(dimensions == otherMember.dimensions, "...",);
    }
}

void TensorExpression::dyadProductAssign(const TensorExpression& other){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::DyadicProduct;

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
        dimensions = children.begin()->dimensions;
    }

    //
    children.emplace_back(copySelf ? children.back() : other);

    // Anpassen TensorOrder
    const TensorExpression& otherMember = children.back();

    // Aufgrund des movIntoSelf ist tensorOrder eh -1 wenn erster Operand -1 als tensorOrder hat
    if(otherMember.tensorOrder == -1){

        tensorOrder = -1;
    }

    // Wenn Dimensionen von einem Operanden nicht berücksicht werden berücksichtigt das Ergebnis sie auch nicht
    if(tensorOrder > -1 && !(containsDimensions() && other.containsDimensions())){

        dimensions.clear();

        tensorOrder = tensorOrder + otherMember.tensorOrder;
    }

    // Regeln und Logik falls beide Operanden die Dimensionen berücksichtigen
    else if(tensorOrder > -1){

        //
        int contractIndices = std::min(tensorOrder, otherMember.tensorOrder);

        //
        dimensions.insert(dimensions.end(), otherMember.dimensions.begin(), otherMember.dimensions.end());

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
        dimensions = children.begin()->dimensions;
    }

    //
    children.emplace_back(copySelf ? children.back() : other);

    // Anpassen TensorOrder

    // Anpassen TensorOrder
    const TensorExpression& otherMember = children.back();

    // Aufgrund des movIntoSelf ist tensorOrder eh -1 wenn erster Operand -1 als tensorOrder hat
    if(otherMember.tensorOrder == -1){

        tensorOrder = -1;
    }

    // Wenn Dimensionen von einem Operanden nicht berücksicht werden berücksichtigt das Ergebnis sie auch nicht
    if(tensorOrder > -1 && !(containsDimensions() && other.containsDimensions())){

        dimensions.clear();

        tensorOrder = tensorOrder + otherMember.tensorOrder - 4;
    }

    // Regeln und Logik falls beide Operanden die Dimensionen berücksichtigen
    else if(tensorOrder > -1){

        //
        int contractIndices = 2;

        //
        RETURNING_ASSERT(std::equal(dimensions.end() - contractIndices, dimensions.end(), std::reverse_iterator(otherMember.dimensions.begin() + contractIndices)),
            "Crossing contraction dimensions don't match",);

        dimensions.erase(dimensions.end() - contractIndices, dimensions.end());
        dimensions.insert(dimensions.end(), otherMember.dimensions.begin() + contractIndices, otherMember.dimensions.end());

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
        dimensions = children.begin()->dimensions;
    }

    //
    children.emplace_back(copySelf ? children.back() : other);

    // Anpassen TensorOrder
    const TensorExpression& otherMember = children.back();

    // Aufgrund des movIntoSelf ist tensorOrder eh -1 wenn erster Operand -1 als tensorOrder hat
    if(otherMember.tensorOrder == -1){

        tensorOrder = -1;
    }

    // Wenn Dimensionen von einem Operanden nicht berücksicht werden berücksichtigt das Ergebnis sie auch nicht
    if(tensorOrder > -1 && !(containsDimensions() && other.containsDimensions())){

        dimensions.clear();

        tensorOrder = tensorOrder + otherMember.tensorOrder - 4;
    }

    // Regeln und Logik falls beide Operanden die Dimensionen berücksichtigen
    else if(tensorOrder > -1){

        //
        int contractIndices = 2;

        // Für Reversed Check : std::make_reverse_iterator(otherMember.dimensions.begin() + contractNIndices)
        RETURNING_ASSERT(std::equal(dimensions.end() - contractIndices, dimensions.end(), otherMember.dimensions.begin()), "...",);

        dimensions.erase(dimensions.end() - contractIndices, dimensions.end());
        dimensions.insert(dimensions.end(), otherMember.dimensions.begin() + contractIndices, otherMember.dimensions.end());

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

    if(children.begin()->containsDimensions()){

        dimensions = children.begin()->dimensions;
        std::reverse(dimensions.begin(), dimensions.end());
    }
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

    if(children.begin()->containsDimensions()){

        dimensions = children.begin()->dimensions;
    }
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

    // dimensions spielen hier keine rolle da ausdruck skalar ist >> beschreibende dimensions entsprechen default
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
    
    // die n letzten Dimensionen werden kontrahiert
    // >> dimensions.end() - contractNIndices bis dimensions.end()

    if(children.begin()->containsDimensions()){

        RETURNING_ASSERT(std::all_of(children.begin()->dimensions.end() - contractIndices, children.begin()->dimensions.end(),
            [&](int dim) { return dim == children.begin()->dimensions.back(); }), "...",);

        dimensions = children.begin()->dimensions;
        dimensions.erase(dimensions.end() - (contractNIndices + 1), dimensions.end());
    }
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

    // dimensions spielen hier keine rolle da ausdruck skalar ist >> beschreibende dimensions entsprechen default
}

void TensorExpression::frobeniusAssign(){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::Frobenius;

    //
    moveSelfIntoFirstChild();

    // node erneut Aufsetzen
    Relation = TkType::Operator;
    Operator = operation;
    tensorOrder = 0;

    // dimensions spielen hier keine rolle da ausdruck skalar ist >> beschreibende dimensions entsprechen default
}

void TensorExpression::macaulayAssign(){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::Macaulay;

    //
    RETURNING_ASSERT(tensorOrder < 1, "...",);

    //
    moveSelfIntoFirstChild();

    // node erneut Aufsetzen
    Relation = TkType::Operator;
    Operator = operation;
    tensorOrder = 0;

    // dimensions spielen hier keine rolle da ausdruck skalar ist >> beschreibende dimensions entsprechen default
}

void TensorExpression::signumAssign(){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::Signum;

    //
    RETURNING_ASSERT(tensorOrder < 1, "...",);

    //
    moveSelfIntoFirstChild();

    // node erneut Aufsetzen
    Relation = TkType::Operator;
    Operator = operation;
    tensorOrder = 0;

    // dimensions spielen hier keine rolle da ausdruck skalar ist >> beschreibende dimensions entsprechen default
}

void TensorExpression::sqrtAssign(){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::Sqrt;

    //
    RETURNING_ASSERT(tensorOrder < 1, "...",);

    //
    moveSelfIntoFirstChild();

    // node erneut Aufsetzen
    Relation = TkType::Operator;
    Operator = operation;
    tensorOrder = 0;

    // dimensions spielen hier keine rolle da ausdruck skalar ist >> beschreibende dimensions entsprechen default
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

    if(children.begin()->containsDimensions()){

        dimensions = children.begin()->dimensions;
    }
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
        tensorOrder = children.begin()->tensorOrder;

        // da tensorOrder < 0 kommt der Ausdruck eh ohne valide dimensions
    }
    else if(containsDimensions()){
        
        *this = TensorExpression("zeros", tensorOrder, dimensions);
    }
    else{
        
        *this = TensorExpression("zeros", tensorOrder);
    }
}

void TensorExpression::onesAssign(){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::Ones;

    if(tensorOrder < 0){

        //
        moveSelfIntoFirstChild();

        // node erneut Aufsetzen
        Relation = TkType::Operator;
        Operator = operation;
        tensorOrder = children.begin()->tensorOrder;
    }
    else if(containsDimensions()){
        
        *this = TensorExpression("ones", tensorOrder, dimensions);
    }
    else{

        *this = TensorExpression("ones", tensorOrder);
    }
}

void TensorExpression::identityAssign(){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::Identity;

    if(tensorOrder < 0){

        //
        moveSelfIntoFirstChild();

        // node erneut Aufsetzen
        Relation = TkType::Operator;
        Operator = operation;
        tensorOrder = children.begin()->tensorOrder;
    }
    else if(containsDimensions()){
        
        *this = TensorExpression("Identity", tensorOrder, dimensions);
    }
    else{
        
        *this = TensorExpression("Identity", tensorOrder);
    }
}

bool TensorExpression::isValid() const{

    return Relation != TkType::None;
}

bool TensorExpression::isCommutativ() const{

    if(Relation != TkType::Operator){ return false; }

    if(Operator == TensorExpressionOperator::Addition || Operator == TensorExpressionOperator::Multiplication){
        return true;
    }

    return false;
}

bool TensorExpression::operator==(const TensorExpression& other) const {

    // LOG << toString(1) << " == " << other.toString(1) << " ???? " << endl;

    // Check ob gleiche Instanz
    if(this == &other){ return true; }

    // wenn ein member des Abgleichs eine templated node (Standardtemplate) ist und der andere kein Template
    // >> template und einsetzungs parameter
    if(((isTemplatedNode() && !other.isTemplate()) || (!isTemplate() && other.isTemplatedNode())) &&
       (tensorOrder == other.tensorOrder || (tensorOrder == -1 || other.tensorOrder == -1))){

        return true;
    }

    // wenn ein member des Abgleichs eine templated node (Standardtemplate) ist und der andere kein Template
    // >> template und einsetzungs parameter
    if(((isArgTemplate() && !other.isTemplate() && other.children.size() < 1) || (!isTemplate() && children.size() < 1 && other.isArgTemplate()))){

        return true;
    }

    // Einkommentieren für Tmpl Dependencies
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    if(g_compareTemplateDependencies){

        //
        bool lhsIsTmplDepn = isTemplateDependencie();
        bool rhsIsTmplDepn = other.isTemplateDependencie();

        //
        if(lhsIsTmplDepn != rhsIsTmplDepn){

            //
            const TensorExpression& tmplDep = lhsIsTmplDepn ? *this : other;
            const TensorExpression& expr = lhsIsTmplDepn ? other : *this;

            //
            if(expr.Relation != TkType::Argument || expr.isTemplate()){

                return false;
            }

            if(tmplDep.Operator == TensorExpressionOperator::Identity && expr.label == "Identity"){
                return true;
            }
            else if(tmplDep.Operator == TensorExpressionOperator::Zeros && expr.label == "zeros"){
                return true;
            }
            else if(tmplDep.Operator == TensorExpressionOperator::Ones && expr.label == "ones"){
                return true;
            }

            return false;
        }
    }

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    if(Relation != other.Relation){ return false; }
    if(Operator != other.Operator){ return false; }
    if(isConstant && other.isConstant && !isConstantTemplate() && !other.isConstantTemplate() && (value != other.value)){ return false; }
    if(isConstant && other.isConstantTemplate() || isConstantTemplate() && other.isConstant){ return true; }

    if(label != other.label){ return false; }
    if(tensorOrder != -1 && other.tensorOrder != -1 && tensorOrder != other.tensorOrder){ return false; }

    if(containsDimensions() && other.containsDimensions() && dimensions != other.dimensions){ return false; }

    if(children.size() != other.children.size()){ return false; }
    
    // Fallback für Nodes die keine Childs haben bislang aber identisch waren (relation, label, dimensions, etc. gleich)
    if(children.size() == 0 && other.children.size() == 0){ return true; }

    bool equal = true;

    for(size_t childIdx = 0; childIdx < children.size(); childIdx++){
        
        if(!(children[childIdx] == other.children[childIdx])){
            
            // LOG << children[childIdx].toString() << other.children[childIdx].toString() << endl;
            // LOG << (children[childIdx] == other.children[childIdx]) << endl;

            equal = false;
        } 
    }

    if(equal){ return true; }
    else if(!isCommutativ()){

        return equal;
    }

    // auto sortedChildren = unwrap().children;
    // auto sortedOtherChildren = other.unwrap().children;

    // // for(const auto& child : sortedChildren){ LOG << child.toString(1) << " | "; }
    // // LOG << endl;
    // // for(const auto& child : sortedOtherChildren){ LOG << child.toString(1) << " | "; }
    // // LOG << endl;

    // std::sort(sortedChildren.begin(), sortedChildren.end());
    // std::sort(sortedOtherChildren.begin(), sortedOtherChildren.end());

    // // for(const auto& child : sortedChildren){ LOG << child.toString(1) << " | "; }
    // // LOG << endl;
    // // for(const auto& child : sortedOtherChildren){ LOG << child.toString(1) << " | "; }
    // // LOG << endl;

    // for(size_t i = 0; i < sortedChildren.size(); i++){
        
    //     // LOG << sortedChildren[i].toString() << " == " << sortedOtherChildren[i].toString() << " = " << (sortedChildren[i] == sortedOtherChildren[i]) << endl;
    //     if(!(sortedChildren[i] == sortedOtherChildren[i])){
    //         return false;
    //     }
    // }

    // Umstellung von Abgleich sortierter Childs zu Permutationsgenerierung 
    // Aufgrund der Bandbreite der Templates die einander überladen müssen (Überladung nutzt ebenfalls <)
    // ist Sortierung nicht mehr trivial

    size_t N = children.size();
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

        for(size_t i = 0; i < children.size(); i++){

            if(!(children[i] == other.children[perm[i]])){

                permutationIsMatching = false;
                break;
            }
        }

        if(permutationIsMatching){

            matchingPermutation = permIdx;
            break;
        }
    }

    return permutationIsMatching;
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

    if(children.begin()->containsDimensions() && children.back().containsDimensions()){

        dimensions = children.begin()->dimensions;
        dimensions.insert(dimensions.end(), children.back().dimensions.begin(), children.back().dimensions.end());
    }
}

void TensorExpression::diffAssign(const TensorExpression& other){

    //
    static TensorExpressionOperator operation = TensorExpressionOperator::Diff;

    //
    bool copySelf = false;
    
    //
    static substitutionMap subsMap = {};

    //
    bool IsRepresentableByTemplate = false;
    auto it = tensorExpressionDiffTemplates.begin();

    // Check ob Expression durch ein abgespeichertes Template repräsentiert werden kann
    for (; it != tensorExpressionDiffTemplates.end(); ++it) {

        if (it->first.first == *this && it->first.second == other) 
        {
            //
            // LOG << "Template found " << it->first.first.toString() << " / " << it->first.second.toString() << " => " << it->second.toString() << endl;

            // Checke ob der Ausdruck durch das Template auch für mehrfache Vorkommen einzelner TemplateInstanzen
            // zb. <A> .. <B> .. <A> repräsentiert werden kann
            // static substitutionMap subsMap = {};
            subsMap.clear();

            // result rekursiv durchlaufen und substituieren wenn nötig
            bool isRepresentationConsistent = true
                && assembleSubstitutionMap(it->first.first, *this, subsMap, true)
                && assembleSubstitutionMap(it->first.second, other, subsMap, true);

            //
            if(!isRepresentationConsistent){ continue; }

            // Einkommentieren für Tmpl Dependencies
            // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

            if(g_compareTemplateDependencies){

                // Konsistenz Check

                if(it->first.first.containsTemplateDependencie() || it->first.second.containsDimensions()){

                    //
                    DISABLE_ASSERTION_LOGGING();

                    bool isRepresentationDependencieSafe = true;

                    TensorExpression sourceA = it->first.first;
                    TensorExpression sourceB = it->first.second;

                    // for(const auto& [k, v] : subsMap){

                    //     LOG << k.toString() << " <> " << v.toString() << endl;
                    // }

                    //
                    replaceBySubstitutions(sourceA, subsMap);
                    replaceBySubstitutions(sourceB, subsMap);
                    sourceA = sourceA.rebuild();
                    sourceB = sourceB.rebuild();

                    //
                    if((!(*this == sourceA)) || !(other == sourceB)){ isRepresentationDependencieSafe = false; }

                    //
                    RESET_ASSERTION_LOGGING();

                    //
                    if(!isRepresentationDependencieSafe){ continue; }
                }
            }

            // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

            if(isRepresentationConsistent){ // && isRepresentationDependencieSafe){

                IsRepresentableByTemplate = true;
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

        // LOG << it->first.first.toString() << " / " << it->first.second.toString() << " => " << it->second.toString() << endl;

        // result rekursiv durchlaufen und substituieren wenn nötig
        TensorExpression res = it->second;

        //
        // substitutionMap subsMap;
        // assembleSubstitutionMap(it->first.first, *this, subsMap);
        // assembleSubstitutionMap(it->first.second, other, subsMap);

        // for(const auto& [k, v] : subsMap){

        //     LOG << k.toString() << " <> " << v.toString() << endl;
        // }

        //
        replaceBySubstitutions(res, subsMap);

        //
        *this = res.rebuild();
    }
    else if(*this == other){

        if(containsDimensions() && other.containsDimensions()){

            std::vector<int> tmpDims = dimensions;
            tmpDims.insert(tmpDims.end(), other.dimensions.begin(), other.dimensions.end());
            *this = TensorExpression("Identity", 2 * this->tensorOrder, tmpDims);
        }
        else{

            *this = TensorExpression("Identity", 2 * this->tensorOrder);
        }
    }
    else if(((Relation == TkType::Argument && other.Relation == TkType::Argument) ||
            (Operator == TensorExpressionOperator::Diff))){ //  || Operator == TensorExpressionOperator::Multiplication

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

void TensorExpression::diffAssign(const TensorExpression& other, int times){

    //
    RETURNING_ASSERT(times >= 0, "Angabe der Ableitungsanzahl muss >= 0 sein",);

    //
    for(int i = 0; i < times; i++){

        diffAssign(other);
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

void TensorExpression::convertToConstantTemplate(const std::string& labelIn){

    //
    *this = TensorExpression(minCnstLimit);
    label = labelIn;
}

void TensorExpression::convertToArgTemplate(){

    isArgTmpl = true;
}

bool TensorExpression::isTemplatedNode() const{

    if(Relation == TkType::Container && Operator == TensorExpressionOperator::Arbitary){

        return true;
    }

    return false;
}

bool TensorExpression::isTemplate() const{

    //
    if(isTemplatedNode() || isConstantTemplate() || isInstanceTemplate() || isArgTemplate()){
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

bool TensorExpression::isInstanceTemplate() const {

    return Relation == TkType::Argument && tensorOrder < 0 && isArgTmpl == false;
}

bool TensorExpression::isArgTemplate() const{

    return isArgTmpl;
}

bool TensorExpression::isTemplateDependencie() const{

    if(!isTemplate() || isInstanceTemplate()){

        return false;
    }

    if(tensorOrder == -1 && Relation == TkType::Operator && 
        (Operator == TensorExpressionOperator::Zeros || Operator == TensorExpressionOperator::Ones ||
         Operator == TensorExpressionOperator::Identity)){

        return true;
    }

    return false;
}

//
bool TensorExpression::containsTemplateDependencie() const{

    if(isTemplateDependencie()){

        return true;
    }

    for(const auto& child : children){

        if(child.isTemplateDependencie()){
            return true;
        }
    }

    return false;
}

//
std::string TensorExpression::toString(size_t depth) const{

    //
    std::string res;

    res += depth == 0 ? "TensorExpression[" + std::to_string(tensorOrder) + "]" : "";

    if(depth == 0 && containsDimensions()){

        res += "(";
        for(const auto& dim : dimensions){
            res += std::to_string(dim) + ",";
        }
        res += ")";
    }

    res += depth == 0 ? " = " : "";

    // Argument node
    if(Relation == TkType::Argument && isInstanceTemplate()){

        res += "insttmpl<" + label + ">";
    }
    else if(Relation == TkType::Argument && isArgTemplate()){

        res += "argtmpl<" + label + ">";
    }
    else if(Relation == TkType::Argument && !isConstant){

        res += label;
    }
    else if(Relation == TkType::Argument && isConstantTemplate()){

        res += "<CnstTmpl<" + label + ">>";
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
        res += depth > 0 ? "" : "";
    }
    // durch Operator verknüpfte Child nodes
    else if(Relation == TkType::Operator && Operator == TensorExpressionOperator::Diff && children.size() == 2){

        res += "diff(" + children[0].toString(depth+1) + " / " + children[1].toString(depth+1) + ")";        
    }
    else if(isTemplatedNode()){

        res += "<" + label + ">";     
    }
    else{

        res += "Invalid Expr";
    }

    // Auskommentieren für Logging ohne Dimensions und Stufen angabe
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    // res += depth > 0 ? "[" + std::to_string(tensorOrder) + "]" : "";

    // if(depth > 0 && containsDimensions()){

    //     res += "(";
    //     for(const auto& dim : dimensions){
    //         res += std::to_string(dim) + ",";
    //     }
    //     res += ")";
    // }

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

    bool TENSOR_EXPRESSION::setUpClass(){

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

        // Konstruktoren
        registerFunction("tExpr", {STRING::typeIndex, INT::typeIndex, ARGS::typeIndex},
            [__functionLabel__ = "tExpr", __numArgs__ = 3](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_RETURN(TENSOR_EXPRESSION, 0);
                GET_ARG(STRING, 0); GET_ARG(INT, 1); GET_ARG(ARGS, 2);

                //
                std::vector<int> dimensions = {};
                dimensions.reserve(arg2->getMember().size());

                //
                for(auto& arg : arg2->getMember()){

                    RETURNING_ASSERT(arg.getTypeIndex() == INT::typeIndex, "...",);
                    dimensions.emplace_back(static_cast<INT*>(arg.getVariableRef().getData())->getMember());
                }

                // schreiben in returns
                ret0->getMember() = TensorExpression(arg0->getMember(), arg1->getMember(), dimensions);
        },
        {TENSOR_EXPRESSION::typeIndex});

        // Konstruktoren
        registerFunction("tExpr", {STRING::typeIndex, ARGS::typeIndex},
            [__functionLabel__ = "tExpr", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_RETURN(TENSOR_EXPRESSION, 0);
                GET_ARG(STRING, 0); GET_ARG(ARGS, 1);

                //
                std::vector<int> dimensions = {};
                dimensions.reserve(arg1->getMember().size());

                //
                for(auto& arg : arg1->getMember()){

                    RETURNING_ASSERT(arg.getTypeIndex() == INT::typeIndex, "...",);
                    dimensions.emplace_back(static_cast<INT*>(arg.getVariableRef().getData())->getMember());
                }

                // schreiben in returns
                ret0->getMember() = TensorExpression(arg0->getMember(), dimensions.size(), dimensions);
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
        registerFunction("tExprTmpl", {STRING::typeIndex},
            [__functionLabel__ = "tExprTmpl", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_RETURN(TENSOR_EXPRESSION, 0);
                GET_ARG(STRING, 0);

                // schreiben in returns
                ret0->getMember() = TensorExpression(arg0->getMember(), -1);
                ret0->getMember().convertToTemplate();
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
        registerFunction("tExprInstTmpl", {STRING::typeIndex},
            [__functionLabel__ = "tExprInstTmpl", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_RETURN(TENSOR_EXPRESSION, 0);
                GET_ARG(STRING, 0);

                // schreiben in returns
                ret0->getMember() = TensorExpression(arg0->getMember(), -1);
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

        // Konstruktoren
        registerFunction("tExprCnstTmpl", {STRING::typeIndex},
            [__functionLabel__ = "tExprCnstTmpl", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(STRING, 0);
                GET_RETURN(TENSOR_EXPRESSION, 0);

                // schreiben in returns
                ret0->getMember() = TensorExpression();
                ret0->getMember().convertToConstantTemplate(arg0->getMember());
        },
        {TENSOR_EXPRESSION::typeIndex});

        // Konstruktoren
        registerMemberFunction(TENSOR_EXPRESSION::typeIndex, "asExternalNode", {STRING::typeIndex},
            [__functionLabel__ = "asExternalNode", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_MEMBER(TENSOR_EXPRESSION);
                GET_ARG(STRING, 0);
                GET_RETURN(TENSOR_EXPRESSION, 0);
                
                // schreiben in returns
                ret0->getMember() = mb->getMember().asExternalNode(arg0->getMember());
        },
        {TENSOR_EXPRESSION::typeIndex});

        // Konstruktoren
        registerMemberFunction(TENSOR_EXPRESSION::typeIndex, "getOrder", {},
            [__functionLabel__ = "getOrder", __numArgs__ = 0](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_MEMBER(TENSOR_EXPRESSION);
                GET_RETURN(INT, 0);
                
                // schreiben in returns
                ret0->getMember() = mb->getMember().tensorOrder;
        },
        {INT::typeIndex});

        // Konstruktoren
        registerMemberFunction(TENSOR_EXPRESSION::typeIndex, "getLabel", {},
            [__functionLabel__ = "getLabel", __numArgs__ = 0](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_MEMBER(TENSOR_EXPRESSION);
                GET_RETURN(STRING, 0);
                
                // schreiben in returns
                ret0->getMember() = mb->getMember().label;
        },
        {STRING::typeIndex});

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

        registerFunction("__contractingDotProductAssign__", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__contractingDotProductAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                TensorExpression& member0 = arg0->getMember();
                TensorExpression& member1 = arg1->getMember();

                member0.contractingDotProductAssign(member1);
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
        registerFunction("ones", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "ones", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                if(inputs[0]->isLValue()){ returns[0].cloneIntoRValue(inputs[0]->getVariableRef()); }
                else{ returns[0].moveIntoRValue(inputs[0]->getVariableRef()); }

                GET_RETURN(TENSOR_EXPRESSION, 0);
                ret0->getMember().onesAssign();
        },
        {TENSOR_EXPRESSION::typeIndex});

        //
        registerFunction("identity", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "identity", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                if(inputs[0]->isLValue()){ returns[0].cloneIntoRValue(inputs[0]->getVariableRef()); }
                else{ returns[0].moveIntoRValue(inputs[0]->getVariableRef()); }

                GET_RETURN(TENSOR_EXPRESSION, 0);
                ret0->getMember().identityAssign();
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

        //
        registerFunction("frobenius", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "frobenius", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                if(inputs[0]->isLValue()){ returns[0].cloneIntoRValue(inputs[0]->getVariableRef()); }
                else{ returns[0].moveIntoRValue(inputs[0]->getVariableRef()); }

                GET_RETURN(TENSOR_EXPRESSION, 0);
                ret0->getMember().frobeniusAssign();
        },
        {TENSOR_EXPRESSION::typeIndex});

        //
        registerFunction("macaulay", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "macaulay", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                if(inputs[0]->isLValue()){ returns[0].cloneIntoRValue(inputs[0]->getVariableRef()); }
                else{ returns[0].moveIntoRValue(inputs[0]->getVariableRef()); }

                GET_RETURN(TENSOR_EXPRESSION, 0);
                ret0->getMember().macaulayAssign();
        },
        {TENSOR_EXPRESSION::typeIndex});
        
        //
        registerFunction("signum", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "signum", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                if(inputs[0]->isLValue()){ returns[0].cloneIntoRValue(inputs[0]->getVariableRef()); }
                else{ returns[0].moveIntoRValue(inputs[0]->getVariableRef()); }

                GET_RETURN(TENSOR_EXPRESSION, 0);
                ret0->getMember().signumAssign();
        },
        {TENSOR_EXPRESSION::typeIndex});

        //
        registerFunction("sqrt", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "sqrt", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                if(inputs[0]->isLValue()){ returns[0].cloneIntoRValue(inputs[0]->getVariableRef()); }
                else{ returns[0].moveIntoRValue(inputs[0]->getVariableRef()); }

                GET_RETURN(TENSOR_EXPRESSION, 0);
                ret0->getMember().sqrtAssign();
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
        registerFunction("diff", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex, INT::typeIndex},
            [__functionLabel__ = "diff", __numArgs__ = 3](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                //
                returns[0].getVariableRef().clone(inputs[0]->getVariableRef());

                // Returns
                GET_RETURN(TENSOR_EXPRESSION, 0);
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1); GET_ARG(INT, 2);

                ret0->getMember().diffAssign(arg1->getMember(), arg2->getMember());
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
        registerMemberFunction(TENSOR_EXPRESSION::typeIndex, "getNumOfNodes", {},
            [__functionLabel__ = "getNumOfNodes", __numArgs__ = 0](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;
            
                // schreiben in returns
                GET_MEMBER(TENSOR_EXPRESSION);
                GET_RETURN(INT, 0);

                //
                ret0->getMember() = static_cast<int>(mb->getMember().getNumOfNodes());
        },
        {INT::typeIndex});

        //
        registerMemberFunction(TENSOR_EXPRESSION::typeIndex, "getNumOfExternalNodes", {},
            [__functionLabel__ = "getNumOfNodes", __numArgs__ = 0](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;
            
                // schreiben in returns
                GET_MEMBER(TENSOR_EXPRESSION);
                GET_RETURN(INT, 0);

                //
                ret0->getMember() = static_cast<int>(mb->getMember().getNumOfExternalNodes());
        },
        {INT::typeIndex});

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

        registerFunction("removeEmplacedDiffTemplate", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "removeSetDiffTemplate", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); 
                GET_ARG(TENSOR_EXPRESSION, 1);

                TensorExpression& member0 = arg0->getMember();
                TensorExpression& member1 = arg1->getMember();
                
                auto key = std::make_pair(member0, member1);
                size_t removed = tensorExpressionDiffTemplates.erase(key);
                
                RETURNING_ASSERT(removed > 0, "Kein Differential für gegebenes Tensortemplatepaar gefunden : " + member0.toString() + "|" + member1.toString(),);
        },
        {});

        registerFunction("removeEmplacedDiff", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "removeEmplacedDiff", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); 
                GET_ARG(TENSOR_EXPRESSION, 1);

                TensorExpression& member0 = arg0->getMember();
                TensorExpression& member1 = arg1->getMember();
                
                auto key = std::make_pair(member0, member1);
                size_t removed = tensorExpressionDiffs.erase(key);
                
                RETURNING_ASSERT(removed > 0, "Kein Differential für gegebenes Tensortemplatepaar gefunden : " + member0.toString() + "|" + member1.toString(),);
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

        registerFunction("removeEqual", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "removeEqual", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); 

                TensorExpression& member0 = arg0->getMember();

                auto key = member0;
                size_t removed = tensorExpressionSimplifications.erase(key);
                
                RETURNING_ASSERT(removed > 0, "Kein Differential für gegebenes Tensortemplatepaar gefunden : " + member0.toString(),);
        },
        {});

        //
        registerFunction("logPresetDiffs", {},
            [__functionLabel__ = "logPresetDiffs", __numArgs__ = 0](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                LOG << "Found " << tensorExpressionDiffs.size() << " registered Diffs :" << endl;

                for(const auto& [k,v] : tensorExpressionDiffs){

                    LOG << "diff[ " << k.first.toString(1) << ", " << k.second.toString(1) << " ] = " << v.toString(1) << endl;
                }

                LOG << endl;
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
        registerFunction("simplified", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "simplified", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_RETURN(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 0)

                TensorExpression copy(arg0->getMember());  // ← Copy Constructor
                copy.simplify();
                ret0->getMember() = std::move(copy);
        },
        {TENSOR_EXPRESSION::typeIndex});

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
        registerFunction("getUnwrapOperands", {},
            [__functionLabel__ = "getUnwrapOperands", __numArgs__ = 0](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                //
                GET_RETURN(BOOL, 0);

                //
                ret0->getMember() = unwrapOperands;
        },
        {BOOL::typeIndex});

        //
        registerFunction("setCompareTemplateDependencies", {BOOL::typeIndex},
            [__functionLabel__ = "setCompareTemplateDependencies", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                //
                GET_ARG(BOOL, 0);

                //
                g_compareTemplateDependencies = arg0->getMember();
        },
        {});

        //
        registerFunction("getCompareTemplateDependencies", {},
            [__functionLabel__ = "getCompareTemplateDependencies", __numArgs__ = 0](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                //
                GET_RETURN(BOOL, 0);

                //
                ret0->getMember() = g_compareTemplateDependencies;
        },
        {BOOL::typeIndex});

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

        //
        registerFunction("wrap", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "wrap", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_RETURN(TENSOR_EXPRESSION, 0);
                GET_ARG(TENSOR_EXPRESSION, 0)

                ret0->getMember() = arg0->getMember().wrap();
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

        // Operatoren
        registerMemberFunction(TENSOR_EXPRESSION::typeIndex, "replace", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "replace", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

                GET_MEMBER(TENSOR_EXPRESSION);
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                //
                TensorExpression::substitutionMap subsMap;
                subsMap.try_emplace(arg0->getMember(), arg1->getMember());

                // schreiben in returns
                TensorExpression::rawReplaceBySubstitutions(mb->getMember(), subsMap);
                mb->getMember() = mb->getMember().rebuild();
        },
        {});

        // Operatoren
        registerMemberFunction(TENSOR_EXPRESSION::typeIndex, "getReplaced", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "getReplaced", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_MEMBER(TENSOR_EXPRESSION);
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);
                GET_RETURN(TENSOR_EXPRESSION, 0);

                //
                TensorExpression::substitutionMap subsMap;
                subsMap.try_emplace(arg0->getMember(), arg1->getMember());

                //
                ret0->getMember() = mb->getMember();

                // schreiben in returns
                TensorExpression::rawReplaceBySubstitutions(ret0->getMember(), subsMap);
                ret0->getMember() = ret0->getMember().rebuild();
        },
        {TENSOR_EXPRESSION::typeIndex});

        // Operatoren
        registerMemberFunction(TENSOR_EXPRESSION::typeIndex, "getOrder", {},
            [__functionLabel__ = "getReplaced", __numArgs__ = 0](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_MEMBER(TENSOR_EXPRESSION);
                GET_RETURN(INT, 0);

                ret0->getMember() = mb->getMember().tensorOrder;
        },
        {INT::typeIndex});

        // Konstruktoren
        registerFunction("tExprArgTmpl", {STRING::typeIndex},
            [__functionLabel__ = "tExprArgTmpl", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_RETURN(TENSOR_EXPRESSION, 0);
                GET_ARG(STRING, 0);

                // schreiben in returns
                ret0->getMember() = TensorExpression(arg0->getMember(), -1);
                ret0->getMember().convertToArgTemplate();
        },
        {TENSOR_EXPRESSION::typeIndex});

        // Konstruktoren
        registerFunction("tExprArgTmpl", {STRING::typeIndex, INT::typeIndex},
            [__functionLabel__ = "tExprArgTmpl", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_RETURN(TENSOR_EXPRESSION, 0);
                GET_ARG(STRING, 0); GET_ARG(INT, 1);

                // schreiben in returns
                ret0->getMember() = TensorExpression(arg0->getMember(), arg1->getMember());
                ret0->getMember().convertToArgTemplate();
        },
        {TENSOR_EXPRESSION::typeIndex});
        
        // Operatoren
        registerMemberFunction(TENSOR_EXPRESSION::typeIndex, "containsTmplDependencie", {},
            [__functionLabel__ = "getReplaced", __numArgs__ = 0](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_MEMBER(TENSOR_EXPRESSION);
                GET_RETURN(BOOL, 0);

                ret0->getMember() = mb->getMember().containsTemplateDependencie();
        },
        {BOOL::typeIndex});

        // Operatoren
        registerMemberFunction(TENSOR_EXPRESSION::typeIndex, "isTmplDependencie", {},
            [__functionLabel__ = "getReplaced", __numArgs__ = 0](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_MEMBER(TENSOR_EXPRESSION);
                GET_RETURN(BOOL, 0);

                ret0->getMember() = mb->getMember().isTemplateDependencie();
        },
        {BOOL::typeIndex});

        // Operatoren
        registerMemberFunction(TENSOR_EXPRESSION::typeIndex, "contains", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "contains", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_MEMBER(TENSOR_EXPRESSION);
                GET_ARG(TENSOR_EXPRESSION, 0);
                GET_RETURN(BOOL, 0);

                ret0->getMember() = mb->getMember().contains(arg0->getMember());
        },
        {BOOL::typeIndex});

        // Operatoren
        registerMemberFunction(TENSOR_EXPRESSION::typeIndex, "isWrapped", {},
            [__functionLabel__ = "isWrapped", __numArgs__ = 0](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_MEMBER(TENSOR_EXPRESSION);
                GET_RETURN(BOOL, 0);

                ret0->getMember() = mb->getMember().isWrapped();
        },
        {BOOL::typeIndex});

        // Operatoren
        registerMemberFunction(TENSOR_EXPRESSION::typeIndex, "isUnwrapped", {},
            [__functionLabel__ = "isUnwrapped", __numArgs__ = 0](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                GET_MEMBER(TENSOR_EXPRESSION);
                GET_RETURN(BOOL, 0);

                ret0->getMember() = mb->getMember().isUnWrapped();
        },
        {BOOL::typeIndex});

        return true;
    }
}