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

            // kein Cast, bestehender double wird übergeben
            case(ConstantType::FLOAT):
            case(ConstantType::DOUBLE):
                // return Variable(node.value);
            
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
            EvalResult& assignTo = leftSide[0];
            EvalResult& assignValue = rightSide[0];

            //
            bool assignToIsRValue = assignValue.isRValue();
            bool assignToIsReferende = assignTo.getVariableRef().isReference();
            bool assignValueIsRValue = assignValue.isRValue();
            bool assignValueIsReference = assignValue.getVariableRef().isReference();

            //
            LOG << "Am zuweisen" << endl;
            LOG << assignTo << "   <<<<   " << assignValue << endl;

            // assignTo->getVariableRef() = assignedValue->isRValue() ? std::move(assignedValue.getRef()) : assignedValue.getRef();
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
                callFunction(functionLabel, results, params);
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
                
                // Für Fall das ein Kontruktor Aufruf hinten angestellt ist
                const size_t offset = node.children[node.children.size() - 1].Relation == TkType::Params ? 1 : 0;

                const ASTNode& keywordNode = (firstNonArgChildPosition == node.children.size() - offset) ?
                    node.children[node.children.size() - 2 - offset] :              // Standard fall
                    node.children[firstNonArgChildPosition - 1];                   // spezifizierende Notation

                const ASTNode& variableNameNode = (firstNonArgChildPosition == node.children.size() - offset) ?
                    node.children[node.children.size() - 1 - offset] : 
                    node.children[node.children.size() - 1 - offset];

                //
                RETURNING_ASSERT(g_TypeRegister.contains(keywordNode.argument),
                    "Kein Type für unbekanntes Keyword " + keywordNode.argument + " gefunden", {});

                //
                RETURNING_ASSERT(scope.getVariable(variableNameNode.argument) == nullptr,
                    "Variable " + variableNameNode.argument + " existiert bereits im Scope", {});

                // initialisierung der leeren Variable unter dem entsprechenden namen
                scope.variableTable.try_emplace(variableNameNode.argument);

                //
                Variable* variablePtr = scope.getVariable(variableNameNode.argument);
                variablePtr->constructByObject(constructRegisteredType(keywordNode.argument));

                //
                results.emplace_back();
                results[0].setLValue(variablePtr);
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

            RETURNING_ASSERT(paramResults.size() == 1, "Param Section Eintrag gibt mehr als ein shared EvalResult zurück", {});

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