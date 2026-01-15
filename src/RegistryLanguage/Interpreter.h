#pragma once

#include "TypeInstances/gatheredInstances.h"

enum class ExecuteScriptAs : int{

    None,
    MainProc,
    CoProc,
    Include
};

int countOccurrences(const std::string& str, const std::string& sub);

struct Script{

    std::vector<size_t> lineBreaks = {};
    std::string scriptContent;
    size_t numLines = -1;

    void cacheLineBreaks(){
        
        numLines = countOccurrences(scriptContent, "\n");
        lineBreaks.reserve(numLines);

        size_t pos = 0, count = 0;
        const std::string par = "\n";

        while (pos != std::string::npos) {
            
            pos = scriptContent.find(par, pos + par.length());
            
            if(pos != std::string::npos){
                lineBreaks.emplace_back(pos);
            }
        }
    }
};

enum class STDUniformType : int{

};

struct ProcessingResult;
ProcessingResult executeScript(const std::string& scriptPath, Scope* nullScope, ExecuteScriptAs execAs);

std::vector<std::unique_ptr<IObject>> executeProgram(const std::string& scriptPath, Scope* parent = nullptr);