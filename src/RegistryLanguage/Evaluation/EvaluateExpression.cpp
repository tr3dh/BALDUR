#include "EvaluateExpression.h"

//
TypeIndex& DeclaringStructByIndex = g_TypeRegister.typeCounter;

//
std::map<std::string, ExitCase> g_ExitCasesByKeyword{
    {"break", ExitCase::Break},
    {"continue", ExitCase::Continue},
    {"return", ExitCase::Return}
};

//
std::ostream& operator<<(std::ostream& os, ExitCase exit){
    
    os << magic_enum::enum_name(exit);
    return os;
}

//
EvalResultPtrVec convertEvalResultsToPtrVec(EvalResultVec& resVec){

    //
    EvalResultPtrVec memberPtrs;
    memberPtrs.reserve(resVec.size());

    for(auto& m : resVec){

        memberPtrs.emplace_back(&m);
    }

    return memberPtrs;
}

//
Variable* constructVariable(const std::string& variableName, Scope& scope, TypeIndex typeIndex, bool constructAsReference){

    //
    RETURNING_ASSERT(!scope.containsVariableInline(variableName),
        "Variable " + variableName + " existiert bereits im Scope", nullptr);

    // initialisierung der leeren Variable unter dem entsprechenden namen
    Variable* variablePtr = scope.constructAndReturnVariable(variableName);

    !constructAsReference ? 
        variablePtr->constructByObject(constructRegisteredType(typeIndex)) :
        variablePtr->reference(&g_nullRefs[typeIndex]);

    return variablePtr;
}

//
void getAttrib(EvalResult& res, const ASTNode& attrib){

    //
    RETURNING_ASSERT(res.isLValue(), "Attrib Recip ist ken lvalue",);
    Variable* attribPtr = res.getVariableRef().getData()->getAttrib(attrib.argument);

    RETURNING_ASSERT(attribPtr != nullptr, "Attribute " + attrib.argument + " ist nicht auffindbar",);
    res.setLValue(attribPtr);
}

//
Variable* getStaticAttrib(const ASTNode& cls, const ASTNode& attrib){

    //
    RETURNING_ASSERT(typeForKeywordExists(cls.argument), "",nullptr);

    //
    TypeIndex tpIdx = getTypeIndexByKeyword(cls.argument);
    RETURNING_ASSERT(g_staticScopes.contains(tpIdx), "",nullptr);

    //
    RETURNING_ASSERT(g_staticScopes[tpIdx].containsVariableInline(attrib.argument), "No Attrib : " + attrib.argument,nullptr);

    //
    return &g_staticScopes[tpIdx].variableTable[attrib.argument];
}

bool ActiveScopesContainingVariable(Variable* variablePtr, Scope& currentScope){

    bool res;
    res = currentScope.containsVariable(variablePtr);

    if(res){ return res; }

    for(auto& [idx, scope] : g_staticScopes){

        res = scope.containsVariable(variablePtr);

        if(res){ return res; }
    }

    return false;
}

//
std::pair<bool, Variable*> ActiveScopesContainingDataVariableOrReference(IObject* dataPtr, Scope& currentScope){

    std::pair<bool, Variable*> pair;
    pair = currentScope.containsDataVariableOrReference(dataPtr);

    if(pair.first){ return pair; }

    for(auto& [idx, scope] : g_staticScopes){

        pair = scope.containsDataVariableOrReference(dataPtr);

        if(pair.first){ return pair; }
    }

    return std::make_pair(false, nullptr);
}

//
ProcessingResult evaluateExpression(const ASTNode& node, Scope& scope, Scope& returnToScope, Context context){

    //
    ProcessingResult prcResult;

    //
    ASTNode* prevEvaluatedNode = g_currentlyEvaluatedNode;
    g_currentlyEvaluatedNode = const_cast<ASTNode*>(&node);

    //
    switch(node.Relation){

    case(TkType::String):{

        prcResult.evalResults.emplace_back(new types::STRING(node.argument));
        break;
    }
    case(TkType::Constant):{

        switch(node.constantType){

            // Cast in INT
            case(ConstantType::INT):
            case(ConstantType::SIZE):
                prcResult.evalResults.emplace_back(new types::INT(static_cast<int>(node.value)));
                break;

            // kein Cast, bestehender double wird übergeben
            case(ConstantType::FLOAT):
            case(ConstantType::DOUBLE):
                prcResult.evalResults.emplace_back(new types::DOUBLE(node.value));
                break;
            
            default:{

                break;
            }
        }

        // return Variable(node.value);
        break;
    }
    case(TkType::Argument):{

        if(g_ExitCasesByKeyword.contains(node.argument)){

            prcResult.exit = g_ExitCasesByKeyword[node.argument];
        }
        else if(valueForKeywordExists(node.argument)){

            prcResult.evalResults.emplace_back(constructFromKeyword(node.argument));
        }
        else if(context == Context::ASSIGN_RIGHTSIDE){

            // ist Object das einem anderen als Wert zugewiesen wird
            RETURNING_ASSERT(scope.containsVariable(node.argument), "Variable : '" + node.argument + "', die zugewiesen werden soll ist nicht im scope vorhanden", {});

            //
            prcResult.evalResults.emplace_back();
            prcResult.evalResults[0].setLValue(scope.getVariable(node.argument));
        }
        else{

            // default verhalten für Zuweisung als Wert und alle anderen Fälle
            if(!scope.containsVariable(node.argument)){ constructVariable(node.argument, scope, types::VOID::typeIndex, false); }
            
            //
            prcResult.evalResults.emplace_back();
            prcResult.evalResults[0].setLValue(scope.getVariable(node.argument));
        }

        break;
    }
    case(TkType::Operator):{

        const std::string& Operator = g_lengthSortedLexerOperators[node.Operator];

        if(Operator == COLON){

            ProcessingResult res;

            for(size_t childIdx = 0; childIdx < node.children.size(); childIdx++){

                //
                res = evaluateExpression(node.children[childIdx], scope, returnToScope, context);
                
                //
                if(res.exit != ExitCase::None){

                    prcResult.append(res);
                    break;
                }
                else if(res.evalResults.size() == 1 && res.evalResults[0].getTypeIndex() == types::BOOL::typeIndex &&
                   static_cast<types::BOOL*>(res.evalResults[0].getVariableRef().getData())->getMember()){

                    size_t offset = 1;

                    while(childIdx < node.children.size() - offset &&
                            node.children[childIdx + offset].children.size() > 0 &&
                            node.children[childIdx + offset].children[0].argument == "else"){
                        
                        offset++;
                    }

                   childIdx += offset - 1;
                }
            }
        }
        else if(Operator == KOMMA){

            //
            ProcessingResult paramResults;

            // Wie Params
            for(size_t childIdx = 0; childIdx < node.children.size(); childIdx++){

                //
                const ASTNode& child = node.children[childIdx];
                paramResults = evaluateExpression(child, scope, returnToScope, context);

                // RETURNING_ASSERT(paramResults.evalResults.size() == 1, "Param Section Eintrag gibt ungleich ein shared EvalResult zurück", {});

                prcResult.append(paramResults);
            }
        }
        // else if(Operator == "->"){

        //     RETURNING_ASSERT(node.children.size() == 2, "",{});
        //     RETURNING_ASSERT(node.children[0].Relation == TkType::Argument, "",{});
        //     RETURNING_ASSERT(node.children[1].Relation == TkType::Chain, "", {});
            
        //     const ASTNode& member = node.children[0];
        //     const ASTNode& memberFunc = node.children[1].children[0];
        //     const ASTNode& paramsNd = node.children[1].children[1];

        //     ProcessingResult res = evaluateExpression(member, scope, returnToScope, context);
        //     ProcessingResult params = evaluateExpression(paramsNd, scope, returnToScope, context);

        //     callMemberFunction(memberFunc.argument, prcResult.evalResults, convertEvalResultsToPtrVec(params.evalResults), scope, &res.evalResults[0]);
        // }
        else if(Operator == "->"){

            prcResult = evaluateExpression(node.children[0], scope, scope, context);

            RETURNING_ASSERT(prcResult.evalResults.size() == 1, "Attrib Acessors sind nicht für Mult Syntax ausgelegt",{});
            EvalResult& baseMember = prcResult.evalResults[0];

            for(size_t argIdx = 1; argIdx < node.children.size(); argIdx++){
               
                //
                const ASTNode& child = node.children[argIdx];

                // getMember
                if(child.Relation == TkType::Listing && child.children[0].Relation != TkType::Argument && child.children[0].Relation != TkType::Chain){

                    for(size_t listingArgIdx = 1; listingArgIdx < child.children[0].children.size(); listingArgIdx++){

                        prcResult.evalResults.emplace_back();
                        prcResult.evalResults.back().copy(baseMember);   
                    }

                    for(size_t listingArgIdx = 0; listingArgIdx < child.children[0].children.size(); listingArgIdx++){

                        //
                        const auto& listingArg = child.children[0].children[listingArgIdx];

                        //
                        if(listingArg.Relation == TkType::Argument){
                            
                            getAttrib(prcResult.evalResults[listingArgIdx], listingArg);
                        }
                        // exec MemberFunc
                        else if(listingArg.children.size() == 2){

                            // functionLabel : node.argument
                            const std::string& functionLabel = listingArg.children[0].argument;
                            const ASTNode& params = listingArg.children[1];

                            //
                            ProcessingResult paramRes = evaluateExpression(params, scope, returnToScope, context);

                            //
                            ProcessingResult res;

                            callMemberFunction(functionLabel, res.evalResults, convertEvalResultsToPtrVec(paramRes.evalResults), scope, &baseMember);
                            
                            prcResult.evalResults[listingArgIdx] = std::move(res.evalResults[0]);
                        }

                    }
                }
                // getMember
                else if(child.Relation == TkType::Argument){ getAttrib(baseMember, child); }
                else if(child.Relation == TkType::Listing && child.children[0].Relation == TkType::Argument){ getAttrib(baseMember, child.children[0]); }
                // exec MemberFunc
                else if(child.children.size() == 2){

                    // functionLabel : node.argument
                    const std::string& functionLabel = child.children[0].argument;
                    const ASTNode& params = child.children[1];

                    //
                    ProcessingResult paramRes = evaluateExpression(params, scope, returnToScope, context);

                    //
                    ProcessingResult res;

                    callMemberFunction(functionLabel, res.evalResults, convertEvalResultsToPtrVec(paramRes.evalResults), scope, &baseMember);
                    
                    prcResult.evalResults.clear();
                    prcResult.append(res);
                }
                else if(child.Relation == TkType::Listing && child.children[0].children.size() == 2){

                    // functionLabel : node.argument
                    const std::string& functionLabel = child.children[0].children[0].argument;
                    const ASTNode& params = child.children[0].children[1];

                    //
                    ProcessingResult paramRes = evaluateExpression(params, scope, returnToScope, context);

                    //
                    ProcessingResult res;
                    callMemberFunction(functionLabel, res.evalResults, convertEvalResultsToPtrVec(paramRes.evalResults), scope, &baseMember);
                    
                    prcResult.evalResults.clear();
                    prcResult.append(res);
                }
                else{ RETURNING_ASSERT(TRIGGER_ASSERT, "Invalid Attrib Acessor template",{}); };
            }
        }
        else if(Operator == ">>"){

            RETURNING_ASSERT(node.children.size() == 2, "", {});
            RETURNING_ASSERT(node.children[0].Relation == TkType::Argument, "", {});
            
            bool isFunctionCall = node.children[1].children.size() == 2;

            if(node.children[1].Relation == TkType::Listing &&
                (node.children[1].children[0].Relation == TkType::Argument || node.children[1].children[0].Relation == TkType::Chain)){

                isFunctionCall = node.children[1].children[0].children.size() == 2;
            }

            if(node.children[1].Relation == TkType::Listing && node.children[1].children[0].Relation != TkType::Argument && node.children[1].children[0].Relation != TkType::Chain){

                for(size_t listingArgIdx = 0; listingArgIdx < node.children[1].children[0].children.size(); listingArgIdx++){

                    //
                    const auto& listingArg = node.children[1].children[0].children[listingArgIdx];

                    //
                    bool ListingArgIsFunctionCall = listingArg.children.size() == 2;

                    if(!ListingArgIsFunctionCall){

                        prcResult.evalResults.emplace_back();

                        Variable* varPtr = getStaticAttrib(node.children[0], listingArg);
                        RETURNING_ASSERT(varPtr != nullptr, "", {});

                        prcResult.evalResults.back().setLValue(varPtr);
                    }
                    else{

                        const ASTNode& params = listingArg.children[1];
                        ProcessingResult paramRes = evaluateExpression(params, scope, scope, context);

                        ProcessingResult tmpRes;
                        callStaticFunction(getTypeIndexByKeyword(node.children[0].argument), listingArg.children[0].argument,
                            tmpRes.evalResults, convertEvalResultsToPtrVec(paramRes.evalResults), scope);

                        prcResult.append(tmpRes, true);
                    }
                }
            }
            else if(!isFunctionCall && node.children[1].Relation == TkType::Listing){
                
                prcResult.evalResults.emplace_back();

                Variable* varPtr = getStaticAttrib(node.children[0], node.children[1].children[0]);
                RETURNING_ASSERT(varPtr != nullptr, "", {});

                prcResult.evalResults.back().setLValue(varPtr);
            }
            else if(!isFunctionCall){

                prcResult.evalResults.emplace_back();

                Variable* varPtr = getStaticAttrib(node.children[0], node.children[1]);
                RETURNING_ASSERT(varPtr != nullptr, "", {});

                prcResult.evalResults.back().setLValue(varPtr);
            }
            else if(node.children[1].Relation == TkType::Listing){
            
                const ASTNode& params = node.children[1].children[0].children[1];
                ProcessingResult paramRes = evaluateExpression(params, scope, scope, context);

                callStaticFunction(getTypeIndexByKeyword(node.children[0].argument), node.children[1].children[0].children[0].argument,
                    prcResult.evalResults, convertEvalResultsToPtrVec(paramRes.evalResults), scope);
            }
            else{

                const ASTNode& params = node.children[1].children[1];
                ProcessingResult paramRes = evaluateExpression(params, scope, scope, context);

                callStaticFunction(getTypeIndexByKeyword(node.children[0].argument), node.children[1].children[0].argument,
                    prcResult.evalResults, convertEvalResultsToPtrVec(paramRes.evalResults), scope);
            }
        }
        else if(g_OneArgOperations.contains(Operator) && node.children.size() == 1){

            ProcessingResult res = evaluateExpression(node.children[0], scope, returnToScope, Context::ASSIGN_RIGHTSIDE);

            //
            for(size_t resIdx = 0; resIdx < res.evalResults.size(); resIdx++){

                ProcessingResult tmpRes;
                callFunction(g_OneArgOperations[Operator], tmpRes.evalResults, {&res.evalResults[resIdx]}, scope);

                prcResult.append(tmpRes);
            }
        }
        else if(g_TwoArgOperations.contains(Operator)){
            
            RETURNING_ASSERT(node.children.size() == 2,
                "Verknüpfung über zweiseitige Operatoren von ungleich 2 child nodes nicht möglich", {});

            ProcessingResult leftSide = evaluateExpression(node.children[0], scope, returnToScope, Context::ASSIGN_LEFTSIDE);
            RETURNING_ASSERT(!leftSide.evalResults.empty(), "Linke Seite der Zuweisung ist leer", {});

            ProcessingResult rightSide = evaluateExpression(node.children[1], scope, returnToScope, Context::ASSIGN_RIGHTSIDE);
            RETURNING_ASSERT(!rightSide.evalResults.empty(), "Rechte Seite der Zuweisung ist leer", {});

            if(leftSide.evalResults.size() == rightSide.evalResults.size()){

                //
                ProcessingResult tmpRes;

                //
                for(size_t childIdx = 0; childIdx < leftSide.evalResults.size(); childIdx++){

                    //
                    tmpRes.clear();

                    callFunction(g_TwoArgOperations[Operator], tmpRes.evalResults,
                        { &leftSide.evalResults[childIdx], &rightSide.evalResults[childIdx] }, scope);

                    prcResult.append(tmpRes);
                }
            }
            // Problematisch da rvalue aus evalresult beim assign weggemovt wird und dann nur noch als
            // invalide Variable zur Verfügung steht
            //
            // else if(rightSide.size() == 1){

            //     //
            //     for(size_t childIdx = 0; childIdx < leftSide.size(); childIdx++){

            //         callFunction(g_TwoArgOperations[Operator], prcResult.evalResults, { &leftSide[childIdx], &rightSide[0] });
            //     }
            // }
            else{

                RETURNING_ASSERT(TRIGGER_ASSERT, "Seitengrößen für 2 Arg Operation", {});

                // // Könnte etwas verwirrend wirken
                // // wirklich reinnehmen ??

                // // Wenn auf jeder seite der zwei Arg Operation unterschiedlich viele EvalResults sind
                // // Kartesisches Produkt >> Jedes EvalRes links mit jedem EvalRes rechts kombinieren
                // ProcessingResult tmpRes;

                // for(size_t leftIdx = 0; leftIdx < leftSide.evalResults.size(); leftIdx++){
                //     for(size_t rightIdx = 0; rightIdx < rightSide.evalResults.size(); rightIdx++){

                //         tmpRes.clear();

                //         callFunction(g_TwoArgOperations[Operator], tmpRes.evalResults,
                //             { &leftSide.evalResults[leftIdx], &rightSide.evalResults[rightIdx] }, scope);

                //         prcResult.append(tmpRes);
                //     }
                // }
            }
        }
        // argChainOperators enthält den Operator der Operation die mehrere Argumente verknüpft und die Funktion
        // über die diese jeweils zwei argumente verknüpft
        // für lange Operationsketten so im Gegensatz zum klassichen AST etwas effizient
        // Aufgrund nur einer eventuellen Kopie und sonst immer beaufschlagung mit __...assign__
        else if(g_ArgChainOperations.contains(Operator)){

            //
            RETURNING_ASSERT(node.children.size() > 1, "Zu wenig childs", {});

            //
            ProcessingResult res, tmpRes;
            prcResult = evaluateExpression(node.children[0], scope, returnToScope, context);

            //
            ProcessingResult rvarRefs;
            rvarRefs.evalResults.resize(prcResult.evalResults.size());

            //
            for(size_t idx = 0; idx < prcResult.evalResults.size(); idx++){

                prcResult.evalResults[idx].constructRValueByContainedLValue();
                rvarRefs.evalResults[idx].setLValue(&prcResult.evalResults[idx].getVariableRef());
            }

            //
            for(size_t childIdx = 1; childIdx < node.children.size(); childIdx++){

                //
                tmpRes = evaluateExpression(node.children[childIdx], scope, returnToScope, Context::ASSIGN_RIGHTSIDE);
                RETURNING_ASSERT(prcResult.evalResults.size() == tmpRes.evalResults.size(), "Ungleiche Größen in Seiten beim 2 s ops",{});
                
                //
                for(size_t resIdx = 0; resIdx < prcResult.evalResults.size(); resIdx++){

                    //
                    // if(prcResult.evalResults[resIdx].isLValue()){

                    //     prcResult.evalResults[resIdx].constructRValueByContainedLValue();
                    // }

                    //
                    callFunction(g_ArgChainOperations[Operator], res.evalResults, {&rvarRefs.evalResults[resIdx], &tmpRes.evalResults[resIdx]}, scope);
                    RETURNING_ASSERT(res.evalResults.empty(), "Verkettende Funktion bearbeitet nur übergebene member und darf nichts zurückgeben",{});
                }
            }
        }

        break;
    }
    case TkType::Chain:{

        if(node.children.size() > 0 && node.children[0].argument == "return"){

            //
            RETURNING_ASSERT(node.children.size() == 2, "return Statement hat ungleich zwei childs",{});
            
            // return des zweiten childs
            prcResult = evaluateExpression(node.children[1], scope, returnToScope, context);
            prcResult.exit = ExitCase::Return;

            //
            for(auto& res : prcResult.evalResults){

                // check ob lvalues im parent Scope des Scopes als Variablen bekannt
                // >> ansonsten clone nötig

                // für rvalue variablen >> continue
                if(res.isRValue() && !res.getVariableRef().isReference()){

                    continue;
                }
                // Wenn Scope Variable // reference enthält
                else if(ActiveScopesContainingVariable(&res.getVariableRef(), returnToScope)){  // returnToScope.containsVariable(&res.getVariableRef())){
                    
                    continue;
                }
                // Wenn Scope Variable (Referenz) NICHT enthält aber referenzierte Variable
                else if(res.getVariableRef().isReference() && ActiveScopesContainingDataVariableOrReference(res.getData(), returnToScope).first){ // returnToScope.containsDataVariableOrReference(res.getData()).first){
                    
                    if(res.isRValue()){ continue; }

                    // finden der Referenzierten Variable und rvalue referenz zurückgeben
                    res.variable = Variable();
                    res.variable.reference(*res.variablePtr);
                    res.variablePtr = nullptr;
                    
                    continue;
                }
                // Wenn Scope Variable (rvalue, Referenz) NICHT enthält und auch NICHT referenzierte Variable
                else if(res.isRValue() && res.getVariableRef().isReference()){

                    //
                    res.variable.ownedObject = res.variable.referencedObject->get()->clone();
                    res.variable.referencedObject = nullptr;

                    //
                    continue;
                }

                // result ist rvalue variable >> clone
                res.constructRValueByContainedLValue();
            }
        }
        else if(IsForLoop(node)){

            const ASTNode& paramSection = node.children[1];
            const ASTNode& initialValues = paramSection.children[0],
                breakCondition = paramSection.children[1], increment = paramSection.children[2];

            const ASTNode& executableSection = node.children[2];

            Scope loopScope;
            loopScope.parent = &scope;

            evaluateExpression(initialValues, loopScope, returnToScope, context);

            ProcessingResult breakConditionRes, SectionRes;
            bool stayInLoop = true;
            bool firstFrame = true;

            while(true){

                // Abbruchbedingung checken
                breakConditionRes = evaluateExpression(breakCondition, loopScope, returnToScope, context);

                RETURNING_ASSERT(breakConditionRes.evalResults.size() == 1, "", {});
                RETURNING_ASSERT(breakConditionRes.evalResults[0].getTypeIndex() == types::BOOL::typeIndex, "", {});

                //
                stayInLoop = static_cast<types::BOOL*>(breakConditionRes.evalResults[0].getData())->getMember();

                // abbrechen wenn erforderlich
                if(!stayInLoop){ break; }

                // Schleifen Sektion ausfürhen
                SectionRes = evaluateExpression(executableSection, loopScope, returnToScope, !firstFrame ? context : Context::FIRST_LOOP_FRAME);
                if(firstFrame){ firstFrame = false; }

                //
                switch(SectionRes.exit){
                    
                    case(ExitCase::Break):{
                        
                        stayInLoop = false;
                        break;
                    }
                    case(ExitCase::Return):{

                        return SectionRes;
                        break;
                    }
                    default:{

                        break;
                    }
                }

                // abbrechen wenn erforderlich
                if(!stayInLoop){ break; }

                // inkrement aufrufen
                evaluateExpression(increment, loopScope, returnToScope, context);
            }
        }
        else if(IsWhileLoop(node)){

            RETURNING_ASSERT(node.children[1].Relation == TkType::Params, "",{});
            RETURNING_ASSERT(node.children[2].Relation == TkType::Section, "",{});

            const ASTNode& paramSection = node.children[1];
            RETURNING_ASSERT(paramSection.children.size() >= 1, "", {});

            const ASTNode& breakCondition = paramSection.children[paramSection.children.size() - 1];

            const ASTNode& executableSection = node.children[2];

            Scope loopScope;
            loopScope.parent = &scope;

            ProcessingResult breakConditionRes, SectionRes;
            bool stayInLoop = true;
            bool firstFrame = true;

            // initialize
            evaluateExpression(paramSection, loopScope, returnToScope, context);

            //
            while(true){

                // Abbruchbedingung checken
                breakConditionRes = evaluateExpression(breakCondition, loopScope, returnToScope, context);

                RETURNING_ASSERT(breakConditionRes.evalResults.size() == 1, "", {});
                RETURNING_ASSERT(breakConditionRes.evalResults[0].getTypeIndex() == types::BOOL::typeIndex, "", {});

                stayInLoop = static_cast<types::BOOL*>(breakConditionRes.evalResults[0].getData())->getMember();

                // abbrechen wenn erforderlich
                if(!stayInLoop){
                    break;
                }

                // Schleifen Sektion ausfürhen
                SectionRes = evaluateExpression(executableSection, loopScope, returnToScope, !firstFrame ? context : Context::FIRST_LOOP_FRAME);
                if(firstFrame){ firstFrame = false; }

                //
                switch(SectionRes.exit){
                    
                    case(ExitCase::Break):{
                        
                        stayInLoop = false;
                        break;
                    }
                    case(ExitCase::Return):{

                        return SectionRes;
                        break;
                    }
                    default:{

                        break;
                    }
                }

                // abbrechen wenn erforderlich
                if(!stayInLoop){ break; }
            }
        }
        else if(IsIfStatement(node)){

            //
            const ASTNode& paramSection = node.children[node.children.size() - 2];
            const std::string& functionLabel = node.children[node.children.size() - 3].argument;
            const ASTNode& section = node.children[node.children.size() - 1];

            //
            ProcessingResult paramResults = evaluateExpression(paramSection, scope, returnToScope, Context::ASSIGN_RIGHTSIDE);

            //
            callFunction(functionLabel, prcResult.evalResults, convertEvalResultsToPtrVec(paramResults.evalResults), scope);

            RETURNING_ASSERT(prcResult.evalResults.size() == 1, "If Statement gibt mehr als ein return zurück",{});

            //
            bool executeSection = static_cast<types::BOOL*>(prcResult.evalResults[0].getVariableRef().getData())->getMember();

            ProcessingResult SectionRes;

            //
            if(executeSection){

                Scope childScope;
                childScope.parent = &scope;

                SectionRes = evaluateExpression(section, childScope, returnToScope, context);
            }

            if(SectionRes.exit != ExitCase::None){

                prcResult.append(SectionRes);
            }
        }
        else if(IsElseSection(node)){
            
            ProcessingResult SectionRes;

            Scope childScope;
            childScope.parent = &scope;

            SectionRes = evaluateExpression(node.children[1], childScope, returnToScope, context);

            if(SectionRes.exit != ExitCase::None){

                prcResult.append(SectionRes);
            }
        }
        else if(IsStaticSection(node)){
            
            if(context == Context::FIRST_LOOP_FRAME){

                ProcessingResult SectionRes = evaluateExpression(node.children[1], scope, returnToScope, context);

                if(SectionRes.exit != ExitCase::None){

                    prcResult.append(SectionRes);
                }
            }
            else if(context == Context::DECL_STRUCT){

                // attribute in Static Section sollen in den static Scope der struct Decls konstruiert werden
                // static scope ist *scope.parent || STRUCT::staticScopes[DeclareStructByIndex]
                ProcessingResult SectionRes = evaluateExpression(node.children[1], *scope.parent, returnToScope, Context::DECL_STRUCT_STATIC);

                if(SectionRes.exit != ExitCase::None){

                    prcResult.append(SectionRes);
                }
            }
            // else{

            //     return {};
            // }
        }
        else if(IsFunctionCall(node)){

            //
            const std::string& functionLabel = node.children[0].argument;

            //
            ProcessingResult params = evaluateExpression(node.children[1], scope, returnToScope, Context::ASSIGN_RIGHTSIDE);

            //
            // RETURNING_ASSERT(node.children[1].children.size() == params.size(),
            // "In Funktion Call enthaltene Argumentanzahl stimmt nicht mit Rückgabeargumentanzahl der Paramsection überein", {});

            //
            callFunction(functionLabel, prcResult.evalResults, convertEvalResultsToPtrVec(params.evalResults), scope);
        }
        else if(IsConstructionCall(node)){

            //
            TypeIndex constructType = INVALID_TYPE_INDEX;
            bool constructReference = false;

            //
            if(node.children.size() == 2 && typeForKeywordExists(node.children[0].argument)){

                constructType = getTypeIndexByKeyword(node.children[0].argument);
            }
            else if(node.children.size() == 2 && node.children[0].argument == "ref"){
                
                constructType = types::VOID::typeIndex;
                constructReference = true;
            }
            else if(node.children.size() == 3 && typeForKeywordExists(node.children[0].argument) && node.children[1].argument == "ref"){

                constructType = getTypeIndexByKeyword(node.children[0].argument);
                constructReference = true;
            }

            //
            RETURNING_ASSERT(g_nullRefs.contains(constructType), "Es wird versucht invalider Type zu konstruieren",{});

            //
            const ASTNode& constructVariables = node.children[node.children.size() - 1];

            if(constructVariables.Relation == TkType::Argument){

                prcResult.evalResults.emplace_back();
                prcResult.evalResults[0].setLValue(
                    constructVariable(constructVariables.argument, scope, constructType, constructReference));
            }
            if(constructVariables.Relation == TkType::Listing && constructVariables.children[0].Relation == TkType::Argument){

                prcResult.evalResults.emplace_back();
                prcResult.evalResults[0].setLValue(
                    constructVariable(constructVariables.children[0].argument, scope, constructType, constructReference));
            }
            else if(constructVariables.Relation == TkType::Listing){

                RETURNING_ASSERT(constructVariables.children.size() == 1 &&
                    constructVariables.children[0].Relation == TkType::Operator &&
                    constructVariables.children[0].Operator == g_kommaOperatorIndex, "", {});

                // eigenliche Childnodes des Listing befinden sich in listing.children[0].children da das Listing im
                // Gegensatz zu dem Params nicht automatisch den nach dem Komma Operator gesplittete Inhalt enthält sondern
                // selbst nach dem primären Operator sucht
                // >> listing.child[0] ist Komma Operation mit variablen namen als eintrag
                prcResult.evalResults.resize(constructVariables.children[0].children.size());

                for(size_t childIdx = 0; childIdx < constructVariables.children[0].children.size(); childIdx++){

                    const ASTNode& varNode = constructVariables.children[0].children[childIdx];

                    RETURNING_ASSERT(varNode.Relation == TkType::Argument, "",{});

                    prcResult.evalResults[childIdx].setLValue(
                        constructVariable(varNode.argument, scope, constructType, constructReference));
                }
            }
        }
        else if(node.children.size() == 4 && node.children[0].argument == "decl"){

            //
            const std::string& functionLabel = node.children[1].argument;

            //
            const ASTNode& params = node.children[2];
            const ASTNode& section = node.children[3];

            //
            std::vector<TypeIndex> argIndices = {};
            argIndices.reserve(params.children.size());

            //
            Scope paramScope;

            //
            ProcessingResult paramRes = evaluateExpression(params, paramScope, paramScope, Context::ASSIGN_LEFTSIDE);
            argIndices.reserve(paramRes.evalResults.size());

            //
            TypeIndex idx;
            for(auto& arg : paramRes.evalResults){

                idx = arg.getVariableRef().getData()->getTypeIndex();

                argIndices.emplace_back(idx == types::VOID::typeIndex ? IObject::ARBITATRY_TYPE : idx);
            }

            // //
            // for(auto& arg : params.children){

                // // >> nur argname
                // if(arg.children.size() == 0){

                //     argIndices.emplace_back(IObject::ARBITATRY_TYPE);
                // }
                // else if(arg.children[0].argument == "ref"){

                //     argIndices.emplace_back(IObject::ARBITATRY_TYPE);
                // }
                // else{

                //     argIndices.emplace_back(getTypeIndexByKeyword(arg.children[0].argument));
                // }
            // }

            // Member Func
            if(context == Context::DECL_STRUCT){

                registerMemberFunction(DeclaringStructByIndex, functionLabel, argIndices,
                    [__functionLabel__ = functionLabel, __numArgs__ = argIndices.size(),
                    __argIndices__ = argIndices, params, section, &scope, __script__ = g_currentlyEvaluatedScript
                    ](FREG_ARGS){

                        // 
                        bool argsInvolved = std::find(__argIndices__.begin(), __argIndices__.end(), types::ARGS::typeIndex)
                                            != __argIndices__.end();

                        // Asserts
                        ASSERT_IS_MEMBER_FUNCTION;
                        if(!argsInvolved){ ASSERT_HAS_N_INPUT_ARGS(__numArgs__); }

                        //
                        Script* prevScript = g_currentlyEvaluatedScript;
                        g_currentlyEvaluatedScript = __script__;

                        //
                        Scope functionScope;
                        functionScope.parent = &static_cast<STRUCT*>(member->getVariableRef().getData())->attribScope;

                        // mit params befüllen
                        ProcessingResult paramRes = evaluateExpression(params, functionScope, returnToScope, Context::NONE);

                        //
                        Variable* self = functionScope.constructAndReturnVariable("this");
                        self->reference(member->getVariableRef());

                        if(!argsInvolved){ RETURNING_ASSERT(paramRes.evalResults.size() == inputs.size(), "",); }

                        //
                        for(size_t paramIdx = 0; paramIdx < paramRes.evalResults.size(); paramIdx++){

                            //
                            EvalResult& inputN = *inputs[paramIdx];
                            EvalResult& paramVarN = paramRes.evalResults[paramIdx];

                            //
                            if(paramVarN.getTypeIndex() == types::ARGS::typeIndex && paramIdx == paramRes.evalResults.size() - 1){

                                static_cast<types::ARGS*>(paramVarN.getVariableRef().getData())->moveFrom(
                                    FunctionParams(std::next(inputs.begin(), paramIdx), inputs.end()));
                                break;
                            }
                            else if(paramVarN.getVariableRef().isReference() && (inputN.isLValue() || inputN.getVariableRef().isReference())){

                                // Reference
                                paramVarN.getVariableRef().reference(inputN.getVariableRef());
                            }
                            else{

                                // Copy
                                paramVarN.getVariableRef().constructByUniquePtr(inputN.getVariableRef().getData()->clone());
                            }
                        }

                        returns = evaluateExpression(section, functionScope, returnToScope, Context::NONE).evalResults;

                        g_currentlyEvaluatedScript = prevScript;
                },
                {IObject::ARGS_TYPE});

            }
            else if(context == Context::DECL_STRUCT_STATIC){
                
                registerStaticFunction(DeclaringStructByIndex, functionLabel, argIndices,
                    [__functionLabel__ = functionLabel, __numArgs__ = argIndices.size(),
                    __argIndices__ = argIndices, params, section, &scope, __script__ = g_currentlyEvaluatedScript
                    ](FREG_ARGS){

                        // 
                        bool argsInvolved = std::find(__argIndices__.begin(), __argIndices__.end(), types::ARGS::typeIndex)
                                            != __argIndices__.end();

                        // Asserts
                        ASSERT_IS_NO_MEMBER_FUNCTION;
                        if(!argsInvolved){ ASSERT_HAS_N_INPUT_ARGS(__numArgs__); }

                        //
                        Script* prevScript = g_currentlyEvaluatedScript;
                        g_currentlyEvaluatedScript = __script__;

                        //
                        Scope functionScope;
                        functionScope.parent = &scope;

                        // mit params befüllen
                        ProcessingResult paramRes = evaluateExpression(params, functionScope, returnToScope, Context::NONE);
                        if(!argsInvolved){ RETURNING_ASSERT(paramRes.evalResults.size() == inputs.size(), "",); }

                        //
                        for(size_t paramIdx = 0; paramIdx < paramRes.evalResults.size(); paramIdx++){

                            //
                            EvalResult& inputN = *inputs[paramIdx];
                            EvalResult& paramVarN = paramRes.evalResults[paramIdx];

                            //
                            if(paramVarN.getTypeIndex() == types::ARGS::typeIndex && paramIdx == paramRes.evalResults.size() - 1){

                                static_cast<types::ARGS*>(paramVarN.getVariableRef().getData())->moveFrom(
                                    FunctionParams(std::next(inputs.begin(), paramIdx), inputs.end()));
                                break;
                            }
                            else if(paramVarN.getVariableRef().isReference() && (inputN.isLValue() || inputN.getVariableRef().isReference())){

                                // Reference
                                paramVarN.getVariableRef().reference(inputN.getVariableRef());
                            }
                            else{

                                // Copy
                                paramVarN.getVariableRef().constructByUniquePtr(inputN.getVariableRef().getData()->clone());
                            }
                        }

                        returns = evaluateExpression(section, functionScope, returnToScope, Context::NONE).evalResults;

                        g_currentlyEvaluatedScript = prevScript;
                },
                {IObject::ARGS_TYPE});
            }
            else{

                // Konstruktoren
                registerFunction(functionLabel, argIndices,
                    [__functionLabel__ = functionLabel, __numArgs__ = argIndices.size(),
                    __argIndices__ = argIndices, params, section, &scope, __script__ = g_currentlyEvaluatedScript
                    ](FREG_ARGS){

                        // 
                        bool argsInvolved = std::find(__argIndices__.begin(), __argIndices__.end(), types::ARGS::typeIndex)
                                            != __argIndices__.end();

                        // Asserts
                        ASSERT_IS_NO_MEMBER_FUNCTION;
                        if(!argsInvolved){ ASSERT_HAS_N_INPUT_ARGS(__numArgs__); }

                        //
                        Script* prevScript = g_currentlyEvaluatedScript;
                        g_currentlyEvaluatedScript = __script__;

                        //
                        Scope functionScope;
                        functionScope.parent = &scope;

                        // mit params befüllen
                        ProcessingResult paramRes = evaluateExpression(params, functionScope, returnToScope, Context::NONE);

                        if(!argsInvolved){ RETURNING_ASSERT(paramRes.evalResults.size() == inputs.size(), "",); }

                        //
                        for(size_t paramIdx = 0; paramIdx < paramRes.evalResults.size(); paramIdx++){

                            //
                            EvalResult& inputN = *inputs[paramIdx];
                            EvalResult& paramVarN = paramRes.evalResults[paramIdx];

                            //
                            if(paramVarN.getTypeIndex() == types::ARGS::typeIndex && paramIdx == paramRes.evalResults.size() - 1){

                                static_cast<types::ARGS*>(paramVarN.getVariableRef().getData())->moveFrom(
                                    FunctionParams(std::next(inputs.begin(), paramIdx), inputs.end()));
                                break;
                            }
                            else if(paramVarN.getVariableRef().isReference() && (inputN.isLValue() || inputN.getVariableRef().isReference())){

                                // Reference
                                paramVarN.getVariableRef().reference(inputN.getVariableRef());
                            }
                            else{

                                // Copy
                                paramVarN.getVariableRef().constructByUniquePtr(inputN.getVariableRef().getData()->clone());
                            }
                        }

                        returns = evaluateExpression(section, functionScope, returnToScope, Context::NONE).evalResults;

                        g_currentlyEvaluatedScript = prevScript;
                },
                {IObject::ARGS_TYPE});
            }
        }
        else if(node.children.size() == 3 && node.children[0].argument == "struct" &&
                node.children[1].Relation == TkType::Argument && node.children[2].Relation == TkType::Section){

            //
            const std::string& structName = node.children[1].argument;

            Scope& attribScope = STRUCT::emplaceScopes(DeclaringStructByIndex, scope);

            //
            evaluateExpression(node.children[2], attribScope, scope, Context::DECL_STRUCT);

            //
            STRUCT::registerStruct(structName, DeclaringStructByIndex);
        }
        else if(node.children.size() > 2 && node.children[0].argument == "requires"){
            
            if(node.children[1].argument == "uniform"){

                if(!returnToScope.containsVariable(node.children[2].argument) ||
                    returnToScope.getVariable(node.children[2].argument)->getData()->isUniform() == false){

                    _ERROR << "Assertion failed : Uniform " << node.children[2].argument << " konnte nicht gefunden werden\n" << endl;
                }
            }
            else if(typeForKeywordExists(node.children[1].argument) && node.children[2].argument == "uniform" && node.children.size() > 3){

                if(!returnToScope.containsVariable(node.children[3].argument) ||
                    returnToScope.getVariable(node.children[3].argument)->getData()->isUniform() == false ||
                    returnToScope.getVariable(node.children[3].argument)->getData()->getTypeIndex() != getTypeIndexByKeyword(node.children[1].argument)){

                    _ERROR << "Assertion failed : Uniform " << node.children[3].argument << " konnte nicht gefunden werden\n" << endl;
                }
            }
            else if( node.children[2].argument == "uniform"){

            }
            else{

                callFunction("assert", prcResult.evalResults,
                    {&evaluateExpression(node.children[1], scope, scope, context).evalResults[0],
                        &evaluateExpression(node.children[2], scope, scope, context).evalResults[0]}, scope);
            }
        }
        else if(node.children.size() == 3 && node.children[0].argument == "fetch" &&
                node.children[1].argument == "script"){

            for(auto& child : evaluateExpression(node.children[2], scope, scope, context).evalResults){

                RETURNING_ASSERT(child.getTypeIndex() == types::STRING::typeIndex, "Fetch Syntax erwartet String Inputs", {});

                std::string scriptPath = (fs::path(static_cast<types::STRING*>(scope.getVariable("__CWD__")->getData())->getMember()) /
                                            fs::path(static_cast<types::STRING*>(child.getData())->getMember() + "." + g_languageScriptSuffix)).string();

                //
                Variable tmpCalledAs; tmpCalledAs.clone(*scope.getVariable("__ScriptCalledAs__"));
                Variable tmpScript; tmpScript.clone(*scope.getVariable("__script__"));

                // Hier wird Skript als auszuführendes MainProc aufgerufen
                scope.getVariable("__ScriptCalledAs__")->clone(*scope.getVariable("__Include__"));
                scope.setVariable("__script__", new types::STRING(scriptPath));

                //
                ProcessingResult incRes = executeScript(scriptPath, &scope, ExecuteScriptAs::Include);
                prcResult.append(incRes);

                //
                scope.getVariable("__ScriptCalledAs__")->clone(tmpCalledAs);
                scope.getVariable("__script__")->clone(tmpScript);
            }
        }
        else if(node.children.size() == 3 && node.children[0].argument == "fetch" &&
                node.children[1].argument == "backend"){

            for(auto& child : evaluateExpression(node.children[2], scope, scope, context).evalResults){

                RETURNING_ASSERT(child.getTypeIndex() == types::STRING::typeIndex, "Fetch Syntax erwartet String Inputs", {});
                fetchBackend(static_cast<types::STRING*>(child.getData())->getMember());
            }
        }
        else{
            
            _ERROR << "Invalid Chain Template" << endl;
            LOG << node << endl;
        }

        break;
    }
    case(TkType::Section):
    case(TkType::Listing):
    case(TkType::Params):{

        //
        // prcResult.evalResults.resize(node.children.size());

        //
        for(size_t childIdx = 0; childIdx < node.children.size(); childIdx++){

            //
            const ASTNode& child = node.children[childIdx];
            ProcessingResult paramResults = evaluateExpression(child, scope, returnToScope, context);

            // RETURNING_ASSERT(paramResults.size() == 1, "Param Section Eintrag gibt ungleich ein shared EvalResult zurück", {});
            
            prcResult.append(paramResults);
        }

        break;
    }
    default:{
        break;
    }
    }

    //
    g_currentlyEvaluatedNode = prevEvaluatedNode;

    return prcResult;
}