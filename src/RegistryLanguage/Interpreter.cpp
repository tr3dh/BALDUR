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

std::vector<std::unique_ptr<IObject>> executeScript(const std::string& scriptPath){

    // Aufsetzen der mitgelieferten Standard Typen
    // weitere eigene Typen können bspl. in der eigenen main aufgerufen werden
    setUpTypes();

    //
    // START_TIMER;

    //
    std::ifstream file(scriptPath);
    if (!file) {
        _ERROR << "kein Script " << scriptPath << " gefunden" << ENDL;
        return {};
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

        if(lineStr.find_first_not_of(" \t;{") == lineStr.npos){
            
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
    SetUpLexer(g_UsedOperators);                              

    //
    LOG << g_FunctionRegister << endl;

    auto tokens = lexExpression(src.scriptContent);
    LOG << tokens << endl;

    ASTNode Expr;
    Expr.end = tokens.size();

    convertTokensToAST(Expr, tokens, src.scriptContent);
    LOG << Expr << endl;

    //
    // LOG_TIMER;

    LOG << "Writing into Scope ...\n" << endl;

    // Scope aufsetzen
    Scope nullScope = {};

    // Scope befüllen
    nullScope.constructVariable("__ScriptCalledAs__", types::INT::typeIndex);

    //
    nullScope.constructVariable("__MainProc__", types::INT::typeIndex);
    nullScope.setVariable("__MainProc__", new types::INT(0));

    //
    nullScope.constructVariable("__CoProc__", types::INT::typeIndex);
    nullScope.setVariable("__CoProc__", new types::INT(1));

    //
    nullScope.constructVariable("__Include__", types::INT::typeIndex);
    nullScope.setVariable("__Include__", new types::INT(2));

    // Hier wird Skript als auszuführendes MainProc aufgerufen
    nullScope.setVariable("__ScriptCalledAs__", new types::INT(0));

    //
    nullScope.constructVariable("__script__", types::STRING::typeIndex);
    nullScope.setVariable("__script__", new types::STRING(std::filesystem::absolute(scriptPath).string()));

    //
    nullScope.constructVariable("__args__", types::STRING::typeIndex);
    nullScope.setVariable("__args__", new types::STRING("--execute"));

    //
    auto scriptReturn = evaluateExpression(Expr, nullScope, nullScope, Context::NONE);

    //
    // LOG_TIMER;

    LOG << endl;

    LOG << "Skript Outcome:" << endl;
    LOG << scriptReturn.evalResults << endl;

    LOG << g_TypeRegister << endl;

    std::vector<std::unique_ptr<IObject>> isolatedObjects;
    isolatedObjects.reserve(scriptReturn.evalResults.size());

    // hier evtl mov und nicht clone
    // >> clean up für Structs, sodass keine invaliden parent ptrs überbleiben
    for(auto& obj : scriptReturn.evalResults){
        isolatedObjects.emplace_back(obj.getVariableRef().getData()->clone());
    }

    //
    LOG << "Cleaning up Scopes" << endl;

    // Löschen der pointer auf die nullScope
    STRUCT::cleanUp();
    // g_staticScopes.clear();

    // nicht einfach löschen da sonst die parent ptrs in den attrib scopes sonst ungültig werden
    for(auto& [idx, scope] : g_staticScopes){ scope.parent = nullptr; }

    // nullScope wird geläscht ...
    // --- ab hier sind alle ptrs auf die nullScope ungültig 

    return isolatedObjects;
}