#pragma once

#include "Lexer.h"

enum class ConstantType : uint8_t {
    
    NONE,
    INT,
    SIZE,
    FLOAT,
    DOUBLE
};

struct ASTNode{

    //
    size_t begin = 0, end = 0;

    //
    TkType Relation = TkType::None;
    std::vector<ASTNode> children = {};

    //
    size_t Operator = g_numLexerOperators;

    //
    ConstantType constantType = ConstantType::NONE;
    double value = 0.0f;
    std::string argument = "__NULLEXPR__";

    //
    size_t depth = 0;
};

std::ostream& operator<<(std::ostream& os, const ASTNode& obj);

void convertTokensToAST(ASTNode& Expr, const std::vector<Token>& tokens, const std::string& expressionStr);