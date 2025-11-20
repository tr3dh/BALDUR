#pragma once

#include "TypeInstances/gatheredInstances.h"

enum class ExecuteScriptAs{

    None,
    MainProc,
    CoProc,
    Include
};

struct ProcessingResult;
ProcessingResult executeScript(const std::string& scriptPath, Scope* nullScope, ExecuteScriptAs execAs);

std::vector<std::unique_ptr<IObject>> executeProgram(const std::string& scriptPath, Scope* parent = nullptr);