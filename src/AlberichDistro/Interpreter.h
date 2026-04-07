#pragma once

#include "TypeInstances/gatheredInstances.h"
#include "nlohmann/json.hpp"

void defaultSetupLexicalInstances();

std::vector<std::unique_ptr<IObject>> executeDistroProgram(const std::string& scriptPath);