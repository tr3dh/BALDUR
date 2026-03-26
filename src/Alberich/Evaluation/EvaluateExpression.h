#pragma once

#include "../SemanticAnalysis/AST.h"
#include "../LanguageRegister/RegisterInstances.h"
#include "Scope.h"
#include "../TypeInstances/gatheredInstances.h"
#include "IdentifyNodeCases.h"
#include "../TypeInstances/STRUCT.h"
#include "../Interpreter.h"
#include "../LanguageRegister/BackendRegister.h"

enum class Context : uint8_t{

    NONE,
    ASSIGN_LEFTSIDE,
    ASSIGN_RIGHTSIDE,
    FIRST_LOOP_FRAME,
    DECL_STRUCT,
    DECL_STRUCT_STATIC
};

// Alles auf vector<EvalResult> ummünzen 
template<typename T>
void moveAppendVector(std::vector<T>& recipient, std::vector<T>& source){

    //
    recipient.reserve(source.size());

    // alle elemente verschieben
    std::move(source.begin(), source.end(), std::back_inserter(recipient));
}

enum class ExitCase : uint8_t{

    None,
    Continue,
    Break,
    Return,
};

//
std::ostream& operator<<(std::ostream& os, ExitCase exit);

//
extern std::map<std::string, ExitCase> g_ExitCasesByKeyword;

struct ProcessingResult{

    EvalResultVec evalResults = {};
    ExitCase exit = ExitCase::None;

    ProcessingResult() = default;

    void clear(){

        evalResults.clear();
        exit = ExitCase::None;
    }

    void append(ProcessingResult& other, bool ignoreExit = false){

        //
        switch(other.exit){

            case(ExitCase::None):{

                break;
            }
            default:{

                if(!ignoreExit){
                    
                    //
                    exit = other.exit;

                    //
                    evalResults.clear();
                }

                break;
            }
        }

        // Im Standardfall werden einfach die evalResults gemovt
        // für den Fall Return oder Break oder Continue spezifische Verhalten
        moveAppendVector(evalResults, other.evalResults);
    }
};

//
EvalResultPtrVec convertEvalResultsToPtrVec(EvalResultVec& resVec);

//
Variable* constructVariable(const std::string& variableName, Scope& scope, TypeIndex typeIndex, bool constructAsReference);

//
ProcessingResult evaluateExpression(const ASTNode& node, Scope& scope, Scope& returnToScope, Context context);