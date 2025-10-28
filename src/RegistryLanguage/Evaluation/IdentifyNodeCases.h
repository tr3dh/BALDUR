#pragma once

#include "../SemanticAnalysis/AST.h"
#include "../LanguageRegister/RegisterInstances.h"
#include "Scope.h"
#include "../TypeInstances/gatheredInstances.h"

//
extern const std::vector<std::string> g_ifKonjunctions;

//
bool IsIfStatement(const ASTNode& node);
bool IsElseSection(const ASTNode& node);

//
bool IsWhileLoop(const ASTNode& node);
bool IsForLoop(const ASTNode& node);
bool IsStaticSection(const ASTNode& node);

//
bool IsConstructionCall(const ASTNode& node);
bool IsFunctionCall(const ASTNode& node);