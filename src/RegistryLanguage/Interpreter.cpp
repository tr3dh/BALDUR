#include "Interpreter.h"

int countOccurrences(const std::string& str, const std::string& sub) {

    if (sub.empty()) return 0;

    int count = 0;
    size_t pos = str.find(sub);

    while (pos != std::string::npos) {
        ++count;
        pos = str.find(sub, pos + sub.length());
    }

    return count;
}

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

int executeScript(const std::string& scriptPath){

    // Aufsetzen der mitgelieferten Standard Typen
    // weitere eigene Typen können bspl. in der eigenen main aufgerufen werden
    setUpTypes();

    //
    std::ifstream file(scriptPath);
    if (!file) {
        _ERROR << "kein Script " << scriptPath << " gefunden" << ENDL;
        return 1;
    }

    Script src;
    src.scriptContent = "";

    std::string line;
    while (std::getline(file, line)) {

        //
        std::string lineStr;

        if(string::contains(line, "//")){
            
            if(line.find_first_of("//") == line.find_first_not_of(" \t")){
                
                lineStr = "";
            }
            else{
                
                lineStr = line.substr(0, line.find_first_of("//"));
            }
        }
        else{

            lineStr = line;
        }

        if(lineStr.find_first_not_of(" \t;") == lineStr.npos){
            
        }
        else if(lineStr[lineStr.find_last_not_of(" \t")] != ';'){
            
            lineStr += ";";
        }
        else{

        }

        src.scriptContent += lineStr;
        src.scriptContent += "\n";
    }

    file.close();

    LOG << "Werte Skript " << scriptPath << " aus" << endl;
    LOG << endl;
    LOG << src.scriptContent << endl;

    //
    src.cacheLineBreaks();
    // LOG << src.lineBreaks << endl;

    //
    SetUpLexer({COLON,
                "=", "<<", "<>",
                "+=", "-=", "*=", "/=", "^=",
                "&&", "&&=", "||", "||=",
                "&", "|", "!&", "!|", "&=", "|=", "!&=", "!|=",
                "x|", "!x|", "x|=", "!x|=",
                "==", "!=", ">=", "<=", ">", "<",
                "+", "-", "*", "/", "^",
                "++", "--",
                "!",
                "->",
                KOMMA,});                                  

    auto tokens = lexExpression(src.scriptContent);
    LOG << tokens << endl;

    ASTNode Expr;
    Expr.end = tokens.size();

    convertTokensToAST(Expr, tokens, src.scriptContent);
    LOG << Expr << endl;

    LOG << g_TypeRegister << endl;
    LOG << g_FunctionRegister << endl;

    LOG << "Writing into Scope ...\n" << endl;

    Scope nullScope = {};
    auto r = evaluateExpression(Expr, nullScope, Context::NONE);

    LOG << endl;

    LOG << "Scope:" << endl;
    LOG << nullScope << endl;

    LOG << "Skript Outcome:" << endl;

    if(r.empty()){

        LOG << "empty return" << endl;
    }
    else{

        LOG << r[0] << endl;
    }

    return 0;
}