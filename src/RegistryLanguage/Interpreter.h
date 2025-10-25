#pragma once

#include "Evaluation/EvaluateExpression.h"
#include "TypeInstances/gatheredInstances.h"

std::vector<std::unique_ptr<IObject>> executeScript(const std::string& scriptPath, Scope* parent = nullptr);