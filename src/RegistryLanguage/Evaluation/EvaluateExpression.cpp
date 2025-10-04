#include "EvaluateExpression.h"

//
EvalResult evaluateExpression(const ASTNode& node, Scope& scope, Context context){

    //
    switch(node.Relation){

    case(TkType::Constant):{

        // Variable v = determineVariableType(node.value);
        // return EvalResult(v); // EvalResult konstruiert automatisch den Typ

        switch(node.constantType){

            // Cast in INT
            case(ConstantType::INT):
            case(ConstantType::SIZE):
                // return Variable(static_cast<int>(node.value));

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
    }

    return EvalResult();
}