#pragma once

#include "../SemanticAnalysis/AST.h"
#include "InstanceIncludes.h"
#include "VOID.h"
#include "ARGS.h"
#include "BOOL.h"
#include "INT.h"
#include "STRING.h"

//
extern std::vector<std::string> g_UsedOperators;

// Map der Form Operator | Funktionslabel
extern std::map<std::string, std::string> g_OneArgOperations;

// Map der Form Operator | Funktionslabel
extern std::map<std::string, std::string> g_TwoArgOperations;

// Map der Form Operator | Funktionslabel
extern std::map<std::string, std::string> g_ArgChainOperations;

//
bool emplaceStdOperations();