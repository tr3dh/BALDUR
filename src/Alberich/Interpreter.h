#pragma once

#include "TypeInstances/gatheredInstances.h"

enum class ExecuteScriptAs : int{

    None,
    MainProc,
    CoProc,
    Include
};

int countOccurrences(const std::string& str, const std::string& sub);

struct Script;

extern std::map<std::string, Script> g_Scripts;

extern Script* g_currentlyEvaluatedScript;
extern ASTNode* g_currentlyEvaluatedNode;

struct Script{

    std::vector<size_t> lineBreaks = {};
    std::string scriptContent;
    size_t numLines = -1;

    std::vector<Token> tokens;
    ASTNode Expr;

    std::string scriptPath;

    void cacheLineBreaks(){
    
        numLines = countOccurrences(scriptContent, "\n") + 1;
        lineBreaks.reserve(numLines + 1);

        lineBreaks.emplace_back(0);

        size_t pos = 0;
        const std::string par = "\n";

        while ((pos = scriptContent.find(par, pos)) != std::string::npos) {

            lineBreaks.emplace_back(pos + 1);

            // Zum nächsten Zeichen nach dem Zeilenumbruch springen
            pos += par.length(); 
        }

        lineBreaks.emplace_back(scriptContent.length());
    }

    std::string getLine(size_t lineNum) {

        if (lineNum >= lineBreaks.size() - 1){ return "";}
        return scriptContent.substr(lineBreaks[lineNum], lineBreaks[lineNum + 1] - lineBreaks[lineNum] - 1);
    }
};

enum class STDUniformType : int{

};

void defaultSetupLexicalInstances();
bool CheckAllOperatorsRegistered();

struct ProcessingResult;
ProcessingResult executeScript(const std::string& scriptPath, Scope* nullScope, ExecuteScriptAs execAs);

std::vector<std::unique_ptr<IObject>> executeProgram(const std::string& scriptPath, Scope* parent = nullptr);