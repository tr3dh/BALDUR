#include "Interpreter.h"
#include "Evaluation/EvaluateExpression.h"

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

std::vector<std::unique_ptr<IObject>> executeProgram(const std::string& scriptPath, Scope* parent){

    // Aufsetzen der mitgelieferten Standard Typen
    // weitere eigene Typen können bspl. in der eigenen main aufgerufen werden
    setUpTypes();

    //
    SetUpLexer(g_UsedOperators);            

    //
    Scope nullScope = {};
    nullScope.parent = parent;

    //
    Scope* rootScope = nullScope.getRootScope();

    //
    std::string ExecMode = "DEBUG";
    nullScope.constructAndReturnVariable("ExecMode")->constructByObject(new types::STRING(&ExecMode));

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
    nullScope.constructVariable("__proc__", types::STRING::typeIndex);
    nullScope.setVariable("__proc__", new types::STRING(std::filesystem::absolute(scriptPath).string()));

    //
    nullScope.constructVariable("__args__", types::STRING::typeIndex);
    nullScope.setVariable("__args__", new types::STRING("--execute"));

    //
    nullScope.constructVariable("__CWD__", types::STRING::typeIndex);
    nullScope.setVariable("__CWD__", new types::STRING(fs::path(scriptPath).parent_path().string()));

    //
    ProcessingResult scriptReturn = executeScript(scriptPath, &nullScope, ExecuteScriptAs::MainProc);

    // isolieren der Objekte aus dem EvalResult Vektor
    std::vector<std::unique_ptr<IObject>> isolatedObjects;
    isolatedObjects.reserve(scriptReturn.evalResults.size());

    // hier evtl mov und nicht clone
    // >> clean up für Structs, sodass keine invaliden parent ptrs überbleiben
    for(auto& obj : scriptReturn.evalResults){
        isolatedObjects.emplace_back(obj.getVariableRef().getData()->clone());
    }

    // Löschen der pointer auf die nullScope
    STRUCT::cleanUp();

    // nicht einfach löschen da sonst die parent ptrs in den attrib scopes sonst ungültig werden
    for(auto& [idx, scope] : g_staticScopes){ scope.parent = nullptr; }
    // g_staticScopes.clear();

    // nullScope wird geläscht ...
    // --- ab hier sind alle ptrs auf die nullScope ungültig 

    return isolatedObjects;
}

ProcessingResult executeScript(const std::string& scriptPath, Scope* nullScope, ExecuteScriptAs execAs){

    //
    RETURNING_ASSERT(nullScope != nullptr, "nullScope pointer ist nullptr", {});

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
        static bool fuseLine;
        fuseLine = false;

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
        else if(string::contains(line, "\\") && line.find_last_of("\\") == line.find_last_not_of(" \t")){

            lineStr = line.substr(0, line.find_last_of("\\"));
            fuseLine = true;
        }
        else{

            lineStr = line;
        }

        if(fuseLine){

        }
        else if(lineStr.find_first_not_of(" \t;{") == lineStr.npos){
            
        }
        else if(lineStr[lineStr.find_last_not_of(" \t")] != ';'){
            
            lineStr += ";";
        }
        else{
            
        }

        src.scriptContent += lineStr;
        src.scriptContent += "\n";
    }

    //
    file.close();

    //
    src.cacheLineBreaks();

    auto tokens = lexExpression(src.scriptContent);

    ASTNode Expr;
    Expr.end = tokens.size();

    convertTokensToAST(Expr, tokens, src.scriptContent);

    //
    return evaluateExpression(Expr, *nullScope, *nullScope, Context::NONE);
}