#pragma once

#include "InstanceIncludes.h"
#include "VOID.h"
#include "BOOL.h"
#include "INT.h"

// Map der Form Operator | Funktionslabel
extern std::map<std::string, std::string> g_TwoArgOperations;

// Map der Form Operator | Funktionslabel
extern std::map<std::string, std::string> g_ArgChainOperations;

//
void emplaceStdOperations();