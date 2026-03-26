// Lexer for Single Thread Lexer

#pragma once

#include "defines.h"

//
#define KOMMA ","
#define COLON ";"

// Komma muss enthalten sein und ist innerhalb einer Parameter angabe für eine Funktion IMMER der Primäre Operator
extern std::vector<std::string> g_LexerOperators, g_lengthSortedLexerOperators;
extern size_t g_numLexerOperators, g_kommaOperatorIndex, g_colonOperatorIndex;

extern std::vector<size_t> g_hierarchieOfLengthSortedLexerOperators, g_LexerOperatorIndices;
extern size_t exprIdx, exprSize, opIdx, opCharIdx, argLen;

extern std::unordered_map<char, char> matchingBrackets;
extern std::unordered_set<char> openingBrackets, closingBrackets;

//
void SetUpLexer(const std::vector<std::string>& operators);

enum class TkType : uint8_t{

    None,               // default
    Argument,           // Substitutionsargument
    Constant,           // ausgeschriebene Zahl
    Operator,           // Operator
    Function,           // Funktionsname
    Container,
    Paren,
    Bracket,
    Brace,
    CLParen,
    CLBracket,
    CLBrace,
    Listing,
    Section,
    Params,
    Chain,
    String,
};

//
struct Token{

    // Konstruktor
    Token(size_t pos, size_t len, size_t hier, TkType t, size_t op)
        : position(pos), length(len), hierarchie(hier), type(t), Operator(op) {}

    // Standardkonstruktor
    Token() = default;

    size_t position, length, hierarchie;
    TkType type;
    size_t Operator;
};

std::ostream& operator<<(std::ostream& os, const Token& obj);

void getOperatorIdx(const std::string& expression);
std::vector<Token> lexExpression(const std::string& expression);