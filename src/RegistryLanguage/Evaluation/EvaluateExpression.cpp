#include "EvaluateExpression.h"

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
    RETURNING_ASSERT(!scope.containsVariable(variableName),
        "Variable " + variableName + " existiert bereits im Scope", nullptr);

    // initialisierung der leeren Variable unter dem entsprechenden namen
    Variable* variablePtr = scope.constructAndReturnVariable(variableName);

    !constructAsReference ? 
        variablePtr->constructByObject(constructRegisteredType(typeIndex)) :
        variablePtr->reference(&g_nullRefs[typeIndex]);

    return variablePtr;
}

//
ProcessingResult evaluateExpression(const ASTNode& node, Scope& scope, Context context){

    //
    ProcessingResult prcResult; 

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
            RETURNING_ASSERT(scope.containsVariable(node.argument), "Variable, die zugewiesen werden soll ist nicht im scope vorhanden", {});

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
                res = evaluateExpression(node.children[childIdx], scope, context);
                
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
                paramResults = evaluateExpression(child, scope, context);

                RETURNING_ASSERT(paramResults.evalResults.size() == 1, "Param Section Eintrag gibt ungleich ein shared EvalResult zurück", {});

                prcResult.append(paramResults);
            }
        }
        else if(Operator == "->"){

            RETURNING_ASSERT(node.children.size() == 2, "",{});
            RETURNING_ASSERT(node.children[0].Relation == TkType::Argument, "",{});
            RETURNING_ASSERT(node.children[1].Relation == TkType::Chain, "", {});
            
            const ASTNode& member = node.children[0];
            const ASTNode& memberFunc = node.children[1].children[0];
            const ASTNode& paramsNd = node.children[1].children[1];

            ProcessingResult res = evaluateExpression(member, scope, context);
            ProcessingResult params = evaluateExpression(paramsNd, scope, context);

            callMemberFunction(memberFunc.argument, prcResult.evalResults, convertEvalResultsToPtrVec(params.evalResults), &res.evalResults[0]);
        }
        else if(g_OneArgOperations.contains(Operator) && node.children.size() == 1){

            ProcessingResult res;
            prcResult = evaluateExpression(node.children[0], scope, context);

            //
            for(size_t resIdx = 0; resIdx < prcResult.evalResults.size(); resIdx++){

                //
                callFunction(g_OneArgOperations[Operator], res.evalResults, {&prcResult.evalResults[resIdx]});
            }
        }
        else if(g_TwoArgOperations.contains(Operator)){
            
            RETURNING_ASSERT(node.children.size() == 2,
                "Verknüpfung über zweiseitige Operatoren von ungleich 2 child nodes nicht möglich", {});

            ProcessingResult leftSide = evaluateExpression(node.children[0], scope, Context::ASSIGN_LEFTSIDE);
            RETURNING_ASSERT(!leftSide.evalResults.empty(), "Linke Seite der Zuweisung ist leer", {});

            ProcessingResult rightSide = evaluateExpression(node.children[1], scope, Context::ASSIGN_RIGHTSIDE);
            RETURNING_ASSERT(!rightSide.evalResults.empty(), "Rechte Seite der Zuweisung ist leer", {});

            if(leftSide.evalResults.size() == rightSide.evalResults.size()){

                //
                for(size_t childIdx = 0; childIdx < leftSide.evalResults.size(); childIdx++){

                    callFunction(g_TwoArgOperations[Operator], prcResult.evalResults,
                        { &leftSide.evalResults[childIdx], &rightSide.evalResults[childIdx] });
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

                RETURNING_ASSERT(TRIGGER_ASSERT, "ungleiche Seitengrößen bei two side operator Funktion", {});
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
            prcResult = evaluateExpression(node.children[0], scope, context);

            //
            for(auto& evr : prcResult.evalResults){

                evr.constructRValueByContainedLValue();
            }

            //
            for(size_t childIdx = 1; childIdx < node.children.size(); childIdx++){

                //
                tmpRes = evaluateExpression(node.children[childIdx], scope, context);
                RETURNING_ASSERT(prcResult.evalResults.size() == tmpRes.evalResults.size(), "Ungleiche Größen in Seiten beim 2 s ops",{});
                
                //
                for(size_t resIdx = 0; resIdx < prcResult.evalResults.size(); resIdx++){

                    //
                    callFunction(g_ArgChainOperations[Operator], res.evalResults, {&prcResult.evalResults[resIdx], &tmpRes.evalResults[resIdx]});
                }
            }
        }

        break;
    }
    case TkType::Chain:{

        if(IsForLoop(node)){

            const ASTNode& paramSection = node.children[1];
            const ASTNode& initialValues = paramSection.children[0],
                breakCondition = paramSection.children[1], increment = paramSection.children[2];

            const ASTNode& executableSection = node.children[2];

            Scope loopScope;
            loopScope.parent = &scope;

            evaluateExpression(initialValues, loopScope, context);

            ProcessingResult breakConditionRes, SectionRes;
            bool stayInLoop = true;
            bool firstFrame = true;

            while(true){

                // Abbruchbedingung checken
                breakConditionRes = evaluateExpression(breakCondition, loopScope, context);

                RETURNING_ASSERT(breakConditionRes.evalResults.size() == 1, "", {});
                RETURNING_ASSERT(breakConditionRes.evalResults[0].getTypeIndex() == types::BOOL::typeIndex, "", {});

                //
                stayInLoop = static_cast<types::BOOL*>(breakConditionRes.evalResults[0].getData())->getMember();

                // abbrechen wenn erforderlich
                if(!stayInLoop){ break; }

                // Schleifen Sektion ausfürhen
                SectionRes = evaluateExpression(executableSection, loopScope, !firstFrame ? context : Context::FIRST_LOOP_FRAME);
                if(firstFrame){ firstFrame = false; }

                //
                switch(SectionRes.exit){
                    
                    case(ExitCase::Break):{
                        
                        stayInLoop = false;
                        break;
                    }
                    default:{

                        break;
                    }
                }

                // abbrechen wenn erforderlich
                if(!stayInLoop){ break; }

                // inkrement aufrufen
                evaluateExpression(increment, loopScope, context);
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
            evaluateExpression(paramSection, loopScope, context);

            //
            while(true){

                // Abbruchbedingung checken
                breakConditionRes = evaluateExpression(breakCondition, loopScope, context);

                RETURNING_ASSERT(breakConditionRes.evalResults.size() == 1, "", {});
                RETURNING_ASSERT(breakConditionRes.evalResults[0].getTypeIndex() == types::BOOL::typeIndex, "", {});

                stayInLoop = static_cast<types::BOOL*>(breakConditionRes.evalResults[0].getData())->getMember();

                // abbrechen wenn erforderlich
                if(!stayInLoop){
                    break;
                }

                // Schleifen Sektion ausfürhen
                SectionRes = evaluateExpression(executableSection, loopScope, !firstFrame ? context : Context::FIRST_LOOP_FRAME);
                if(firstFrame){ firstFrame = false; }

                //
                switch(SectionRes.exit){
                    
                    case(ExitCase::Break):{
                        
                        stayInLoop = false;
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
            ProcessingResult paramResults = evaluateExpression(paramSection, scope, context);

            //
            callFunction(functionLabel, prcResult.evalResults, convertEvalResultsToPtrVec(paramResults.evalResults));

            RETURNING_ASSERT(prcResult.evalResults.size() == 1, "If Statement gibt mehr als ein return zurück",{});

            //
            bool executeSection = static_cast<types::BOOL*>(prcResult.evalResults[0].getVariableRef().getData())->getMember();

            ProcessingResult SectionRes;

            //
            if(executeSection){

                Scope childScope;
                childScope.parent = &scope;

                SectionRes = evaluateExpression(section, childScope, context);
            }

            if(SectionRes.exit != ExitCase::None){

                prcResult.append(SectionRes);
            }
        }
        else if(IsElseSection(node)){
            
            ProcessingResult SectionRes;

            Scope childScope;
            childScope.parent = &scope;

            SectionRes = evaluateExpression(node.children[1], childScope, context);

            if(SectionRes.exit != ExitCase::None){

                prcResult.append(SectionRes);
            }
        }
        else if(IsStaticSection(node)){
            
            if(context != Context::FIRST_LOOP_FRAME){

                return {};
            }

            ProcessingResult SectionRes = evaluateExpression(node.children[1], scope, context);

            if(SectionRes.exit != ExitCase::None){

                prcResult.append(SectionRes);
            }
        }
        else if(IsFunctionCall(node)){

            //
            const std::string& functionLabel = node.children[0].argument;

            //
            ProcessingResult params = evaluateExpression(node.children[1], scope, context);

            //
            // RETURNING_ASSERT(node.children[1].children.size() == params.size(),
            // "In Funktion Call enthaltene Argumentanzahl stimmt nicht mit Rückgabeargumentanzahl der Paramsection überein", {});

            //
            callFunction(functionLabel, prcResult.evalResults, convertEvalResultsToPtrVec(params.evalResults));
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
        else if(node.children.size() == 2 && node.children[0].argument == "return"){

            // return des zweiten childs
            prcResult = evaluateExpression(node.children[1], scope, context);
            prcResult.exit = ExitCase::Return;
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
            ProcessingResult paramResults = evaluateExpression(child, scope, context);

            // RETURNING_ASSERT(paramResults.size() == 1, "Param Section Eintrag gibt ungleich ein shared EvalResult zurück", {});
            
            prcResult.append(paramResults);
        }

        break;
    }
    default:{
        break;
    }
    }

    return prcResult;
}