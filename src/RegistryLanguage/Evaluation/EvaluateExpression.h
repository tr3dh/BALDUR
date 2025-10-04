#pragma once

#include "SemanticAnalysis/AST.h"

#include "EvalResult.h"
#include "Scope.h"

#include "../TypeInstances/gatheredInstances.h"

enum class Context : uint8_t{

    NONE,
    ASSIGN_LEFTSIDE,
    ASSIGN_RIGHTSIDE
};

//
EvalResult evaluateExpression(const ASTNode& node, Scope& scope, Context context);