#include "EvaluateExpression.h"

// Value determineValueType(double val) {

//     //
//     if (std::floor(val) == val) {
//         if (val >= 0 && val <= static_cast<double>(std::numeric_limits<size_t>::max()))
//             return Value(static_cast<size_t>(val));
//         else if (val >= std::numeric_limits<int>::min() && val <= std::numeric_limits<int>::max())
//             return Value(static_cast<int>(val));
//     }

    
//     if (val >= -std::numeric_limits<float>::max() && val <= std::numeric_limits<float>::max()) {
//         return Value(static_cast<float>(val)); // float reicht
//     }

//     //
//     return Value(val);
// }

// Value addValues(const Value& a, const Value& b) {

//     return std::visit([](auto&& lhs, auto&& rhs) -> Value {

//         using L = std::decay_t<decltype(lhs)>;
//         using R = std::decay_t<decltype(rhs)>;

//         if constexpr (std::is_same_v<L, std::monostate> || std::is_same_v<R, std::monostate>) {
//             CRITICAL_ASSERT(TRIGGER_ASSERT, "Cannot add uninitialized value");
//         }
//         else if constexpr (std::is_same_v<L, int> && std::is_same_v<R, int>) {
//             return lhs + rhs;
//         }
//         else if constexpr (std::is_same_v<L, size_t> && std::is_same_v<R, size_t>) {
//             return lhs + rhs;
//         }
//         else if constexpr (std::is_same_v<L, float> && std::is_same_v<R, float>) {
//             return lhs + rhs;
//         }
//         else if constexpr (std::is_same_v<L, double> && std::is_same_v<R, double>) {
//             return lhs + rhs;
//         }
//         else if constexpr ((std::is_arithmetic_v<L> && std::is_arithmetic_v<R>)) {
            
//             // Alles auf double addieren, dann auf minimalen Typ optimieren
//             double sum = static_cast<double>(lhs) + static_cast<double>(rhs);
//             return determineValueType(sum);
//         }
//         else {
//             CRITICAL_ASSERT(TRIGGER_ASSERT, "Unsupported types for addition");
//         }
//     }, a, b);
// }

// EvalResult& operator+=(EvalResult& lhs, const EvalResult& rhs) {

//     lhs.getRef() = addValues(lhs.getRef(), rhs.getRef());
//     return lhs;
// }

//
Type getTypeFromKeyWord(const std::string& initKeyword){

    for (auto tp : magic_enum::enum_values<Type>()) {
    
        if(magic_enum::enum_name(tp) == initKeyword){

            return tp;
        }
    }

    return Type::NONE;
}

//
EvalResult evaluateExpression(const ASTNode& node, Scope& scope, Context context){

    //
    // LOG << node << endl;

    //
    switch(node.Relation){

        case(TkType::Constant):{

            // Value v = determineValueType(node.value);
            // return EvalResult(v); // EvalResult konstruiert automatisch den Typ

            switch(node.constantType){

                // Cast in INT
                case(ConstantType::INT):
                case(ConstantType::SIZE):
                    return Value(static_cast<int>(node.value));

                // kein Cast, bestehender double wird übergeben
                case(ConstantType::FLOAT):
                case(ConstantType::DOUBLE):
                    return Value(node.value);
                
                default:{

                    break;
                }
            }

            return Value(node.value);
            break;
        }
        case(TkType::String):{

            return EvalResult(Value(node.argument));

            break;
        }
        case(TkType::Argument):{

            // 
            const std::string& arg = node.argument;

            // Check für Keywords
            if(arg == "true"){

                return EvalResult(Value(true));
            }
            else if(arg == "false"){

                return EvalResult(Value(false));
            }
            else if(arg == "null" || arg == "NULL"){

                return EvalResult(Value(0));
            }

            Value* valuePtr = scope.getVariable(node.argument);

            if(valuePtr == nullptr && context == Context::ASSIGN_LEFTSIDE){
                
                scope.constructVariable(node.argument, Type::NONE);
            }
            else if(valuePtr == nullptr && context == Context::ASSIGN_RIGHTSIDE){

                RETURNING_ASSERT(TRIGGER_ASSERT, "In Zuweisung auf rechter Seite verwendete Variable " + node.argument + " existiert nicht", EvalResult::nullRef);
            }

            return EvalResult(scope.getVariable(node.argument));
            break;
        }
        // move für gleich wenn rvalue, bei lvalue kopieren
        case(TkType::Operator):{
            
            const std::string& Operator = g_lengthSortedLexerOperators[node.Operator];

            if(Operator == "="){

                RETURNING_ASSERT(node.children.size() == 2,
                    "Verknüpfung über '=' Operator von ungleich 2 child nodes", EvalResult());

                EvalResult variable = evaluateExpression(node.children[0], scope, Context::ASSIGN_LEFTSIDE);
                EvalResult assignedValue = evaluateExpression(node.children[1], scope, Context::ASSIGN_RIGHTSIDE);

                variable.getRef() = assignedValue.isRValue() ? std::move(assignedValue.getRef()) : assignedValue.getRef();

                // evaluateExpression(node.children[0], context).getRef() = evaluateExpression(node.children[1], context).getRef();

                // for(size_t childIdx = 0; childIdx < node.children.size(); childIdx++){

                //     //
                //     const ASTNode child = node.children[childIdx];
                //     evaluateExpression(child, scope);
                // }
            }
            else if(Operator == "+"){

                RETURNING_ASSERT(node.children.size() > 1,
                     "Ungültige Verknüpfung über '+' Operator, weniger als 2 erforderliche Operation Childs", EvalResult());

                EvalResult result(evaluateExpression(node.children[0], scope, context).getRef());

                for(size_t childIdx = 1; childIdx < node.children.size(); childIdx++){

                    result += evaluateExpression(node.children[childIdx], scope, context);
                }

                LOG << "Result " << result << endl;;

                return result;
            }
            else if(Operator == "-"){

                RETURNING_ASSERT(node.children.size() > 1,
                     "Ungültige Verknüpfung über '+' Operator, weniger als 2 erforderliche Operation Childs", EvalResult());

                EvalResult result(evaluateExpression(node.children[0], scope, context).getRef());

                for(size_t childIdx = 1; childIdx < node.children.size(); childIdx++){

                    result -= evaluateExpression(node.children[childIdx], scope, context);
                }

                LOG << "Result " << result << endl;;

                return result;
            }
            else if(Operator == COLON){

                for(size_t childIdx = 0; childIdx < node.children.size(); childIdx++){

                    //
                    const ASTNode child = node.children[childIdx];
                    evaluateExpression(child, scope, context);
                }
            }
            
            break;
        }
        case TkType::Chain:{

            bool allMembersAreArgs = std::all_of(
                node.children.begin(), node.children.end(),
                [](const ASTNode& child) { return child.Relation == TkType::Argument; });

            // Find the first element not equal to 1
            auto firstNonArgChild = std::find_if(node.children.begin(), node.children.end(),
                        [](const ASTNode& child) { return child.Relation != TkType::Argument; });

            size_t firstNonArgChildPosition = std::distance(node.children.begin(), firstNonArgChild);

            if(node.children.size() > 1){

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
                    node.children[node.children.size() - 2 - offset] :        // Standard fall
                    node.children[firstNonArgChildPosition - 1];                   // spezifizierende Notation

                const ASTNode& variableNameNode = (firstNonArgChildPosition == node.children.size() - offset) ?
                    node.children[node.children.size() - 1 - offset] : 
                    node.children[node.children.size() - 1 - offset];

                const Type variableType = getTypeFromKeyWord(keywordNode.argument);

                scope.constructVariable(variableNameNode.argument, variableType);

                LOG << "Constructed var : " << variableType << " " << variableNameNode.argument << endl;
                return EvalResult(scope.getVariable(variableNameNode.argument));
            }

            break;
        }
        default:{
            break;
        }
    }

    // for(size_t childIdx = 0; childIdx < node.children.size(); childIdx++){

    //     //
    //     const ASTNode child = node.children[childIdx];

    //     evaluateExpression(child, context);
    // }

    return EvalResult::nullRef;
}