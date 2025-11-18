#include "Lexer.h"

// Komma muss enthalten sein und ist innerhalb einer Parameter angabe für eine Funktion IMMER der Primäre Operator
std::vector<std::string> g_LexerOperators, g_lengthSortedLexerOperators;
size_t g_numLexerOperators = 0, g_kommaOperatorIndex, g_colonOperatorIndex;

//
std::vector<size_t> g_hierarchieOfLengthSortedLexerOperators, g_LexerOperatorIndices;
size_t exprIdx, exprSize, opIdx, opCharIdx, argLen;

std::unordered_map<char, char> matchingBrackets = {
    {'(', ')'},
    {'[', ']'},
    {'{', '}'}
};
std::unordered_set<char> openingBrackets = {'(', '[', '{'};
std::unordered_set<char> closingBrackets = {')', ']', '}'};

// 
void SetUpLexer(const std::vector<std::string>& operators) {

    //
    g_LexerOperators = operators;

    //
    g_numLexerOperators = g_LexerOperators.size();
    g_lengthSortedLexerOperators = g_LexerOperators;

    //
    g_hierarchieOfLengthSortedLexerOperators = std::vector<size_t>(g_numLexerOperators);
    g_LexerOperatorIndices = std::vector<size_t>(g_numLexerOperators);

    // Sortiere nach Operator Länge
    std::sort(g_lengthSortedLexerOperators.begin(), g_lengthSortedLexerOperators.end(), [](const std::string& a, const std::string& b) {
        return a.size() > b.size();
    });

    // Bestimme hierarchien für sortierte Operatoren
    for(size_t idx = 0; idx < g_lengthSortedLexerOperators.size(); idx++){

        const auto& op = g_lengthSortedLexerOperators[idx];

        g_hierarchieOfLengthSortedLexerOperators[idx]
            = std::distance(g_LexerOperators.begin(), std::find(g_LexerOperators.begin(), g_LexerOperators.end(), op));
    }

    // Bestimme Indices für unsortierte Operatoren
    for(size_t idx = 0; idx < g_hierarchieOfLengthSortedLexerOperators.size(); idx++){

        //
        g_LexerOperatorIndices[g_hierarchieOfLengthSortedLexerOperators[idx]] = idx;
    }

    // Finde Index für Kommaindex
    g_kommaOperatorIndex = std::distance(g_lengthSortedLexerOperators.begin(),
        std::find(g_lengthSortedLexerOperators.begin(), g_lengthSortedLexerOperators.end(), ","));

    g_colonOperatorIndex = std::distance(g_lengthSortedLexerOperators.begin(),
        std::find(g_lengthSortedLexerOperators.begin(), g_lengthSortedLexerOperators.end(), ";"));
}

// Log für Token struct
std::ostream& operator<<(std::ostream& os, const Token& obj) {

    os << "[Token] POS= " << obj.position  << " + " << obj.length << ", hierarch= " << obj.hierarchie << ", type= " << magic_enum::enum_name(obj.type) << " Ops= "
        << ((obj.Operator < g_numLexerOperators) ? g_lengthSortedLexerOperators[obj.Operator] : "None");
    return os;
}

void getOperatorIdx(const std::string& expression){

    //
    const std::string& exprChar = expression.substr(exprIdx,1);

    // Loop durch Operators
    for(opIdx = 0; opIdx < g_lengthSortedLexerOperators.size(); opIdx++){
    
        // const Ref
        const std::string& op = g_lengthSortedLexerOperators[opIdx];

        // Check ob Operator mit aktuellem char startet
        if(string::startsWith(op, exprChar)){

            // Loop durch Operator
            bool opFound = true;
            for(opCharIdx = 0; opCharIdx < op.size(); opCharIdx++){

                if(exprIdx + opCharIdx > expression.size() - 1 ||
                    op[opCharIdx] != expression[exprIdx + opCharIdx]){

                    opFound = false;
                    break;
                }
            }

            if(opFound && (std::isalpha(op.back()) || std::isdigit(op.back())) &&
               exprIdx + op.size() < expression.size() && !std::isspace(expression[exprIdx + op.size()])){
                
                opFound = false;
            }

            if(opFound){

                return;
            }
        }
    }
}

std::vector<Token> lexExpression(const std::string& expression){

    //
    RETURNING_ASSERT(g_numLexerOperators > 0, "Lexer nicht valide initialisiert, keine Operatoren gefunden! Rufe SetUpLexer({ops}) auf", {});

    //
    std::vector<Token> tokens = {};
    tokens.reserve(expression.size());

    //
    exprSize = expression.size();

    //
    size_t hierarchie = 0;

    // Loop durch expression
    for(exprIdx = 0; exprIdx < expression.size();){
        
        // const Ref
        const char& exprChar = expression[exprIdx];

        // schreibt Idx des Operators in opIdx
        // wenn kein Operator gefunden wird dann ist opIdx == g_numLexerOperators also der Größe des Operator Vektors
        getOperatorIdx(expression);

        //
        if(opIdx != g_numLexerOperators){

            //
            tokens.emplace_back(exprIdx, g_lengthSortedLexerOperators[opIdx].size(), hierarchie, TkType::Operator, opIdx);

            // - 1 weil ja eh einmal hochgezählt wird
            exprIdx += g_lengthSortedLexerOperators[opIdx].size() - 1;
        }
        else if(exprChar == '"'){

            argLen = 1;
            while(exprIdx + argLen < expression.size() && expression[exprIdx + argLen] != '"'){

                argLen++;
            }

            if(argLen > 1){

                //
                tokens.emplace_back(exprIdx + 1, argLen - 1, hierarchie, TkType::String, g_numLexerOperators);
                exprIdx += argLen;
            }
        }
        else if(std::isalpha(exprChar) || exprChar == '_'){

            argLen = 0;
            while(exprIdx + argLen < expression.size() &&
                (std::isalpha(expression[exprIdx + argLen]) || expression[exprIdx + argLen] == '_' || std::isdigit(expression[exprIdx + argLen]))){
                argLen++;
            }

            //
            tokens.emplace_back(exprIdx, argLen, hierarchie, TkType::Argument, g_numLexerOperators);

            exprIdx += argLen - 1;
        }
        else if(std::isdigit(exprChar)){

            argLen = 0;
            while(exprIdx + argLen < expression.size() &&
                (std::isdigit(expression[exprIdx + argLen]) || expression[exprIdx + argLen] == '.')){
                argLen++;
            }

            //
            tokens.emplace_back(exprIdx, argLen, hierarchie, TkType::Constant, g_numLexerOperators);

            exprIdx += argLen - 1;
        }
        else if(exprChar == '('){
            tokens.emplace_back(exprIdx, 1, hierarchie, TkType::Paren, g_numLexerOperators);
            hierarchie++;
        }
        else if(exprChar == '['){
            tokens.emplace_back(exprIdx, 1, hierarchie, TkType::Bracket, g_numLexerOperators);
            hierarchie++;
        }
        else if(exprChar == '{'){
            tokens.emplace_back(exprIdx, 1, hierarchie, TkType::Brace, g_numLexerOperators);
            hierarchie++;
        }
        else if (exprChar == ')') {
            hierarchie--;
            tokens.emplace_back(exprIdx, 1, hierarchie, TkType::CLParen, g_numLexerOperators);
        }
        else if (exprChar == ']') {
            hierarchie--;
            tokens.emplace_back(exprIdx, 1, hierarchie, TkType::CLBracket, g_numLexerOperators);
        }
        else if (exprChar == '}') {
            hierarchie--;
            tokens.emplace_back(exprIdx, 1, hierarchie, TkType::CLBrace, g_numLexerOperators);
        }
        else{

        }

        exprIdx++;
    }

    return tokens;
}