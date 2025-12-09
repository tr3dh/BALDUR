#pragma once

#include "TypeInstances/gatheredInstances.h"

enum class ExecuteScriptAs : int{

    None,
    MainProc,
    CoProc,
    Include
};

enum class STDUniformType : int{

};

struct ProcessingResult;
ProcessingResult executeScript(const std::string& scriptPath, Scope* nullScope, ExecuteScriptAs execAs);

std::vector<std::unique_ptr<IObject>> executeProgram(const std::string& scriptPath, Scope* parent = nullptr);