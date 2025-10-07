#include "EvaluateExpression.h"

// Alles auf vector<EvalResult> ummünzen 

//
EvalResultVec evaluateExpression(const ASTNode& node, Scope& scope, Context context){

    //
    EvalResultVec results;

    //
    switch(node.Relation){

    case(TkType::Constant):{

        switch(node.constantType){

            // Cast in INT
            case(ConstantType::INT):
            case(ConstantType::SIZE):
                results.emplace_back(new types::INT(static_cast<int>(node.value)));
                break;

            // kein Cast, bestehender double wird übergeben
            case(ConstantType::FLOAT):
            case(ConstantType::DOUBLE):
                results.emplace_back(new types::DOUBLE(node.value));
                break;
            
            default:{

                break;
            }
        }

        // return Variable(node.value);
        break;
    }
    case(TkType::Argument):{

        if(valueForKeywordExists(node.argument)){

            results.emplace_back(constructFromKeyword(node.argument));
        }
        else if(context == Context::ASSIGN_RIGHTSIDE){

            // ist Object das einem anderen als Wert zugewiesen wird
            RETURNING_ASSERT(scope.containsVariable(node.argument), "Variable, die zugewiesen werden soll ist nicht im scope vorhanden", {});

            //
            results.emplace_back();
            results[0].setLValue(scope.getVariable(node.argument));
        }
        // else if(context == Context::ASSIGN_LEFTSIDE){
        else{

            // default verhalten für Zuweisung als Wert und alle anderen Fälle
            if(!scope.containsVariable(node.argument)){

                scope.constructVariable(node.argument, types::VOID::typeIndex);
            }
            
            //
            results.emplace_back();
            results[0].setLValue(scope.getVariable(node.argument));
        }

        break;
    }
    case(TkType::Operator):{

        const std::string& Operator = g_lengthSortedLexerOperators[node.Operator];

        if(Operator == COLON){

            for(size_t childIdx = 0; childIdx < node.children.size(); childIdx++){

                //
                evaluateExpression(node.children[childIdx], scope, context);
            }
        }
        else if(Operator == "="){

            RETURNING_ASSERT(node.children.size() == 2,
                "Verknüpfung über '=' Operator von ungleich 2 child nodes", {});

            auto leftSide = evaluateExpression(node.children[0], scope, Context::ASSIGN_LEFTSIDE);
            RETURNING_ASSERT(!leftSide.empty(), "Linke Seite der Zuweisung ist leer", {});

            auto rightSide = evaluateExpression(node.children[1], scope, Context::ASSIGN_RIGHTSIDE);
            RETURNING_ASSERT(!rightSide.empty(), "Rechte Seite der Zuweisung ist leer", {});

            //
            EvalResult& recipient = leftSide[0];
            EvalResult& source = rightSide[0];

            //
            bool recipientIsRValue = recipient.isRValue();
            bool sourceIsRValue = source.isRValue();

            //
            RETURNING_ASSERT(!recipientIsRValue, "Variable der Wert zuwewiesen wird ist Rvalue",{});

            ASSERT(recipient.getTypeIndex() == types::VOID::typeIndex || recipient.getTypeIndex() == source.getTypeIndex(), 
                "narrowing conversion");

            //
            if(recipient.getVariableRef().isReference()){

                RETURNING_ASSERT(IsReferenceValid(recipient.getVariableRef().getUniqueData()), "Nicht initialisierte Recipient Referenz", {});
            }

            if(source.getVariableRef().isReference()){

                RETURNING_ASSERT(IsReferenceValid(source.getVariableRef().getUniqueData()), "Nicht initialisierte Source Referenz", {});
            }

            //
            if(sourceIsRValue){
                
                // RValue wird gemovt, da er eh nur temporär vorhanden ist
                recipient.getVariableRef().move(source.getVariableRef());
            }
            else{

                //
                recipient.getVariableRef().clone(source.getVariableRef());
            }
        }
        else if(Operator == "<<"){

            RETURNING_ASSERT(node.children.size() == 2,
                "Verknüpfung über '<<' Operator von ungleich 2 child nodes", {});

            auto leftSide = evaluateExpression(node.children[0], scope, Context::ASSIGN_LEFTSIDE);
            RETURNING_ASSERT(!leftSide.empty(), "Linke Seite der Zuweisung ist leer", {});

            auto rightSide = evaluateExpression(node.children[1], scope, Context::ASSIGN_RIGHTSIDE);
            RETURNING_ASSERT(!rightSide.empty(), "Rechte Seite der Zuweisung ist leer", {});

            //
            EvalResult& recipient = leftSide[0];
            EvalResult& source = rightSide[0];

            ASSERT(recipient.getTypeIndex() == types::VOID::typeIndex || recipient.getTypeIndex() == source.getTypeIndex(), 
                    "narrowing conversion");

            //
            bool recipientIsRValue = recipient.isRValue();
            bool sourceIsRValue = source.isRValue();

            //
            RETURNING_ASSERT(!recipientIsRValue && !sourceIsRValue,
                "Bei Referenzierung sind rvalues mit im Spiel ",{});

            RETURNING_ASSERT(recipient.getVariableRef().isReference(), "Recipient bei Referenzierung ist keine Referenz", {});

            if(source.getVariableRef().isReference()){

                RETURNING_ASSERT(IsReferenceValid(source.getVariableRef().getUniqueData()), "Nicht initialisierte Source Referenz", {});
            }

            recipient.getVariableRef().reference(source.getVariableRef());
        }

        break;
    }
    case TkType::Chain:{

        //
        bool allMembersAreArgs = std::all_of(
            node.children.begin(), node.children.end(),
            [](const ASTNode& child) { return child.Relation == TkType::Argument; });

        // Find the first element not equal to 1
        auto firstNonArgChild = std::find_if(node.children.begin(), node.children.end(),
                    [](const ASTNode& child) { return child.Relation != TkType::Argument; });

        size_t firstNonArgChildPosition = std::distance(node.children.begin(), firstNonArgChild);

        if(node.children.size() > 1){

            bool IsFunctionCall = node.children[0].Relation == TkType::Argument && node.children[1].Relation == TkType::Params;

            if(IsFunctionCall){

                //
                const std::string& functionLabel = node.children[0].argument;

                //
                EvalResultVec params = evaluateExpression(node.children[1], scope, context);

                //
                RETURNING_ASSERT(node.children[1].children.size() == params.size(),
                    "In Funktion Call enthaltene Argumentanzahl stimmt nicht mit Rückgabeargumentanzahl der Paramsection überein", {});

                //
                EvalResultPtrVec paramPtrs;
                paramPtrs.reserve(params.size());

                for(auto& p : params){

                    paramPtrs.emplace_back(&p);
                }

                //
                callFunction(functionLabel, results, paramPtrs);
            }
            else{

                // Keyword Argument
                // Dabei gibts es zwei verschiedene Notationen
                // 1. Für klassen die spezifiziert werden können (templates Tensoren ...)
                // 2. Standard
                // 
                // Bspl:
                // 1. const Tensor[4]{...} F;
                // 2. const BOOL F;
                //
                // Deshalb wird für danach unterschieden wo die letzte Argument Node liegt
                
                bool constructReference;
                const std::string& variableName = node.children[node.children.size() - 1].argument;
                const std::string& typeKeyword = node.children[0].argument;

                if(node.children.size() == 2 && typeKeyword != "ref"){
                    
                    // normaler init
                    RETURNING_ASSERT(g_TypeRegister.contains(typeKeyword),
                        "Kein Type für unbekanntes Keyword " + typeKeyword + " gefunden", {});

                    //
                    RETURNING_ASSERT(!scope.containsVariable(variableName),
                        "Variable " + variableName + " existiert bereits im Scope", {});

                    // initialisierung der leeren Variable unter dem entsprechenden namen
                    Variable* variablePtr = scope.constructAndReturnVariable(variableName);
                    variablePtr->constructByObject(constructRegisteredType(typeKeyword));

                    //
                    results.emplace_back();
                    results[0].setLValue(variablePtr);

                }
                else if(node.children.size() == 2){

                    // init void ref
                    RETURNING_ASSERT(!scope.containsVariable(variableName),
                        "Variable " + variableName + " existiert bereits im Scope", {});
                    
                    //
                    Variable* variablePtr = scope.constructAndReturnVariable(variableName);
                    variablePtr->reference(&g_nullRefs[types::VOID::typeIndex]);

                    //
                    results.emplace_back();
                    results[0].setLValue(variablePtr);

                }
                else{

                    // init type ref
                    RETURNING_ASSERT(!scope.containsVariable(variableName),
                        "Variable " + variableName + " existiert bereits im Scope", {});
                    
                    //
                    Variable* variablePtr = scope.constructAndReturnVariable(variableName);
                    variablePtr->reference(&g_nullRefs[getTypeIndexByKeyword(typeKeyword)]);

                    //
                    results.emplace_back();
                    results[0].setLValue(variablePtr);

                }
            }
        }

        break;
    }
    case(TkType::Params):{

        //
        results.resize(node.children.size());

        //
        for(size_t childIdx = 0; childIdx < node.children.size(); childIdx++){

            //
            const ASTNode& child = node.children[childIdx];
            EvalResultVec paramResults = evaluateExpression(child, scope, context);

            RETURNING_ASSERT(paramResults.size() == 1, "Param Section Eintrag gibt ungleich ein shared EvalResult zurück", {});
            results[childIdx] = std::move(paramResults[0]);
        }

        break;
    }
    default:{
        break;
    }
    }

    return results;
}