#pragma once

#include "EvalResult.h"

enum class Context : uint8_t{

    NONE,
    ASSIGN_LEFTSIDE,
    ASSIGN_RIGHTSIDE
};

//
Type getTypeFromKeyWord(const std::string& initKeyword);

//
EvalResult evaluateExpression(const ASTNode& node, Scope& scope, Context context);