#pragma once

#include "Evaluation/EvaluateExpression.h"

int executeScript(const std::string& scriptPath){

    std::ifstream file(scriptPath);
    if (!file) {
        _ERROR << "kein Script " << scriptPath << " gefunden" << ENDL;
        return 1;
    }

    std::string scriptContent = "";

    std::string line;
    while (std::getline(file, line)) {

        if(string::contains(line, "//")){
            
            if(line.find_first_of("//") == line.find_first_not_of(" \t")){
                
                continue;
            }
            else{
                
                scriptContent += line.substr(0, line.find_first_of("//"));
                scriptContent += "\n";
                continue;
            }
        }

        scriptContent += line;
        scriptContent += "\n";
    }

    file.close();

    LOG << "Werte Skript " << scriptPath << " aus" << endl;
    LOG << endl;
    LOG << scriptContent << endl;

    //
    SetUpLexer({COLON, KOMMA, "=", ".=", ":=",
                "+", "-", "*", "/", "^",
                "+=", "-=", "*=", "/=", "^="                                    // arithmetische Operatoren
                "++", "--", ">>", "<<",                                         // Verschiebungsoperatoren
                ">", "<", "&&", "&", "||", "|", "!", "?",                       // logische Operatoren
                ">=", "<=", "&=", "|=", "!=",                                   // logische Operatoren mit =
                ".", "...", "..", ":",                                          // dots
                "cdots", "vdots", "cdot"});                                     // ausformulierungen

    auto tokens = lexExpression(scriptContent);
    LOG << tokens << endl;

    ASTNode Expr;
    Expr.end = tokens.size();

    convertTokensToAST(Expr, tokens, scriptContent);
    LOG << Expr << endl;

    LOG << "Writing into Scope\n" << endl;

    Scope nullScope = {};
    auto r = evaluateExpression(Expr, nullScope, Context::NONE);

    LOG << "Scope:" << endl;
    LOG << nullScope << endl;

    LOG << "Skript Outcome:" << endl;
    LOG << *r[0].get() << endl;

    return 0;
}