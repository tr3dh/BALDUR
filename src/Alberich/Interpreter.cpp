#include "Interpreter.h"
#include "Evaluation/EvaluateExpression.h"

std::map<std::string, Script> g_Scripts = {};

Script* g_currentlyEvaluatedScript = nullptr;
ASTNode* g_currentlyEvaluatedNode = nullptr;

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

std::string getErrorContext(){

    std::string res;

    if(g_currentlyEvaluatedScript != nullptr && g_currentlyEvaluatedNode != nullptr){

        std::ostringstream oss;

        // Bereich in tokenfolge : g_currentlyEvaluatedNode->begin - g_currentlyEvaluatedNode->end
        const Token& firstToken = g_currentlyEvaluatedScript->tokens[g_currentlyEvaluatedNode->begin];
        const Token& lastToken = g_currentlyEvaluatedScript->tokens[g_currentlyEvaluatedNode->end - 1];

        size_t position = 0, len = 0, linePos = 0, lineSpan = 0;

        // Zeile i geht von lineBreaks[i] bis lineBreaks[i+1]-1
        for(size_t i = 0; i < g_currentlyEvaluatedScript->lineBreaks.size() - 1; i++){
            
            size_t lineStart = g_currentlyEvaluatedScript->lineBreaks[i];
            size_t lineEnd = g_currentlyEvaluatedScript->lineBreaks[i + 1] - 1; // -1 weil lineBreaks[i+1] bereits nächste Zeile ist
            
            // Zeile in der sich firstToken befindet
            if(lineStart <= firstToken.position && firstToken.position < g_currentlyEvaluatedScript->lineBreaks[i + 1]){
                position = lineStart;
                linePos = i;
            }
            
            // Zeile in der sich lastToken befindet
            if(lineStart <= lastToken.position && lastToken.position < g_currentlyEvaluatedScript->lineBreaks[i + 1]){
                len = lineEnd - position;
                lineSpan = i - linePos;
            }
        }
        
        //
        res += "\n>> Context : ";

        // Header mit Dateiname und Zeilennummer
        std::string normalizedPath = fs::absolute(g_currentlyEvaluatedScript->scriptPath).string();
        std::replace(normalizedPath.begin(), normalizedPath.end(), '\\', '/');
        res += normalizedPath + ":" + std::to_string(linePos + 1) + "\n";

        // Debug output
        // res += std::to_string(position) + " " + std::to_string(len) + "\n";
        // res += std::to_string(linePos) + " " + std::to_string(lineSpan) + "\n";

        // alle betroffenen Zeilen mit Markierungen
        for(size_t i = linePos; i <= linePos + lineSpan; i++){

            //
            RETURNING_ASSERT(i < g_currentlyEvaluatedScript->lineBreaks.size(),
                "Assertion Context kann aufgrund fehlerhafter Skriptformatierung nicht erzeugt werden", "");
            
            //
            std::string lineNum = std::to_string(i + 1);
            std::string line = g_currentlyEvaluatedScript->getLine(i);
            res += lineNum + "   | " + line + "\n";

            // Berechne Markierung für diese Zeile
            size_t prefix = lineNum.length() + 5; // Zeilennummer + 3 Spaces + " | "
            
            size_t lineStart = g_currentlyEvaluatedScript->lineBreaks[i];
            size_t lineEnd = g_currentlyEvaluatedScript->lineBreaks[i + 1] - 1;
            
            // Start und Ende der Markierung
            size_t markStart, markEnd;
            
            if(i == linePos){

                // Erste Zeile
                markStart = firstToken.position - lineStart;

            } else {

                // Folgezeilen
                markStart = line.find_first_not_of(" \t");
            }
            
            if(i == linePos + lineSpan){

                // Letzte Zeile
                markEnd = lastToken.position + lastToken.length - lineStart;
            } else {

                // Zwischenzeilen
                markEnd = line.length();
            }
            
            // Erzeuge Markierung
            res += std::string(prefix + markStart, ' ');
            
            if(markEnd > markStart){

                res += (i == linePos) ? "^" : "~";
                if(markEnd - markStart > 1){
                    res += std::string(markEnd - markStart - 1, '~');
                }
            }
            
            res += "\n";
        }
    }

    return res;
}

std::vector<std::unique_ptr<IObject>> executeProgram(const std::string& scriptPath, Scope* parent){

    //
    if(fs::exists(fs::path(scriptPath).parent_path().string() + "/__LPECONFIG.JSON")){

        //
        // LOG << "[" + getTimestamp() + "] LPEConfig gefunden, Umgebung wird aufgesetzt" << endl;

        //
        nlohmann::json lpeConfig = nlohmann::json::parse(std::ifstream(fs::path(scriptPath).parent_path() / "__LPECONFIG.JSON"), nullptr, true, true);

        //
        g_UsedOperators = lpeConfig.at("LEXICON").get<std::vector<std::string>>();
        g_OneArgOperations = lpeConfig.at("PREFIX").get<std::map<std::string, std::string>>();
        g_TwoArgOperations = lpeConfig.at("INFIX").get<std::map<std::string, std::string>>();
        g_ArgChainOperations = lpeConfig.at("FOLD").get<std::map<std::string, std::string>>();
    }
    else{

        //
        // LOG << "[" + getTimestamp() + "] keine LPEConfig gefunden, Umgebung wird mit default Config aufgesetzt" << endl;
        
        //
        defaultSetupLexicalInstances();

        //
        // LOG << "[" + getTimestamp() + "] Default LPEConfig wird exportiert ..." << endl;

        //
        nlohmann::ordered_json lpeConfig;
        lpeConfig["LEXICON"]  = g_UsedOperators;
        lpeConfig["PREFIX"]   = g_OneArgOperations;
        lpeConfig["INFIX"]    = g_TwoArgOperations;
        lpeConfig["FOLD"]     = g_ArgChainOperations;

        //
        std::ofstream(fs::path(scriptPath).parent_path() / "__LPECONFIG.JSON") << lpeConfig.dump(4);
    }

    //
    CheckAllOperatorsRegistered();

    //
    // g_terminateAfterAssertionFailed = true;

    // Abhier wird als Context in den Asserts der entsprechende Code Ausschnitt aufgeführt
    g_getErrorContext = &getErrorContext;

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

    //
    nullScope.constructAndReturnVariable("g_unwrapOperands")->constructByObject(new types::BOOL(&unwrapOperands));
    // noch mehr davon freischalten (abbruch nach erster Assertion, suppress assertions, etc)

    //
    // nullScope.constructAndReturnVariable("g_compareTemplateDependencies")->constructByObject(new types::BOOL(&g_compareTemplateDependencies));

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

    //
    g_getErrorContext = nullptr;

    return isolatedObjects;

    // nullScope wird geläscht ...
    // --- ab hier sind alle ptrs auf die nullScope ungültig 
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

    bool cachePrevScript = false;
    Script* g_previouslyEvaluatedScript = nullptr;
    ASTNode* g_previouslyEvaluatedNode = nullptr;

    if(g_Scripts.contains(scriptPath)){
        
        RETURNING_ASSERT(TRIGGER_ASSERT, "Mehrfacher Include von " + scriptPath, {});
    }

    Script& src = g_Scripts.try_emplace(scriptPath).first->second;
    src.scriptPath = scriptPath;

    if(g_currentlyEvaluatedScript != nullptr){

        cachePrevScript = true;

        g_previouslyEvaluatedScript = g_currentlyEvaluatedScript;
    }

    g_currentlyEvaluatedScript = &src;
    
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

    src.tokens = lexExpression(src.scriptContent);
    src.Expr.end = src.tokens.size();

    convertTokensToAST(src.Expr, src.tokens, src.scriptContent);

    //
    ProcessingResult prc = evaluateExpression(src.Expr, *nullScope, *nullScope, Context::NONE);

    if(cachePrevScript){

        g_currentlyEvaluatedScript = g_previouslyEvaluatedScript;
    }

    //
    return prc;
}

bool CheckAllOperatorsRegistered()
{
    bool allFound = true;

    auto check = [&](const std::map<std::string, std::string>& ops, const std::string& mapName)
    {
        for (const auto& [op, _] : ops)
        {
            if (std::find(g_UsedOperators.begin(), g_UsedOperators.end(), op) == g_UsedOperators.end())
            {
                RETURNING_ASSERT(TRIGGER_ASSERT, "Operator '" + op + "' kommt in PREFIX, INFIX oder FOLD vor wird im LEXICON aber nicht angegeben",);
                allFound = false;
            }
        }
    };

    check(g_OneArgOperations,   "g_OneArgOperations");
    check(g_TwoArgOperations,   "g_TwoArgOperations");
    check(g_ArgChainOperations, "g_ArgChainOperations");

    return allFound;
}

void defaultSetupLexicalInstances(){
    
    g_UsedOperators = {
    
        COLON,

        // Zuwisungen und Memory Management
        "=", "<<", "<>", "<-", "<+",                            // Memory Management Semantik
        "+=", "-=", "*=", "/=", "^=",                           // Ops für 2 Arg Operationen
        ".=", ".n=", "..=", ":=", "\\x=", "\\(x)=", "\\(.)=",  // für Matrix Ops
        "\\diff=",
        "&=", "!&=", "|=", "!|=", "x|=", "!x|=",        // Ops für boolsche/logische 2 Arg Operationen

        // Walrus Operator
        "=>",

        // Inline Operatoren
        "&&", "!&", "||", "!|", "x|", "!x|",            // ...
        "==", "!=", ">=", "<=", ">", "<", "%",          // Ops für 2 Arg Vergleichs Operationen
        "+", "-", "*", "/", "^",                        // Ops für Verkettung mult Arg Operations per 2 Arg Operationen
        ".", ".n", "..", ":", "\\x", "\\(x)", "°=",     // für verkettung über Matrix ops
        "\\diff",
        "++", "--", "!",                                // Single Argument Ops
        KOMMA,                                          //
        "~", "'", "°", "$",                             // Ops für Index Notation
        "^~", "^'", "^°",                               // Ops für Index Notation
        "->", ">>",                                     // Zugriff auf Statics Scope / Attrib Scopes
        "dref", "invl", "delete",
    };

    //
    g_OneArgOperations = {

        {"!", "__negate__"},
        {"-", "__negate__"},
        {"++", "__increment__"},
        {"--", "__decrement__"},
        {"<-", "__move__"},
        {"<<", "__reference__"},
        {"<+", "__copy__"},
        {"~", "__inverseAssign__"},
        {"'", "__transposeAssign__"},
        {"°", "__traceAssign__"},
        {"^~", "__inverseInplaceAssign__"},
        {"^'", "__transposeInplaceAssign__"},
        {"^°", "__traceInplaceAssign__"},
        {"$", "__sectionAssign__"},
        {"dref", "__dereference__"},
        {"invl", "__invalidate__"},
        {"delete", "__delete__"},
    };

    // Map der Form Operator | Funktionslabel
    g_TwoArgOperations = {

        // Inhalte später mit Operatoren liste aus einer json Datei laden, die das Project Env darstellt
        
        {"=", "__assign__"},
        {"<<", "__reference__"},
        {"<>", "__swap__"},
        {"<-", "__move__"},
        {"<+", "__copy__"},

        {"+=", "__addAssign__"},
        {"-=", "__subAssign__"},
        {"*=", "__mulAssign__"},
        {"/=", "__divAssign__"},
        {"^=", "__expAssign__"},

        {"=>", "__walrusAssign__"},

        {"==", "__equal__"},
        {"!=", "__notEqual__"},
        {">",  "__bigger__"},
        {"<",  "__smaller__"},
        {">=", "__biggerEqual__"},
        {"<=",  "__smallerEqual__"},

        {"&=", "__andAssign__"},
        {"|=", "__orAssign__"},
        {"x|=", "__xorAssign__"},
        {"!&=", "__nandAssign__"},
        {"!|=", "__norAssign__"},
        {"!x|=", "__nxorAssign__"},

        {"%", "__modulo__"},
        {".=", "__dotProductAssign__"},
        {".n=", "__contractingDotProductAssign__"},
        {"..=", "__mirroringDoubleContractionAssign__"},
        {":=", "__crossingDoubleContractionAssign__"},
        {"\\x=", "__crossProductAssign__"},
        {"\\(x)=", "__dyadProductAssign__"},
        {"°=", "__traceAssign__"},
        {"\\diff=", "__diffAssign__"},
    };

    // Map der Form Operator | (Funktionslabel, verknüpfende Operation)
    // zb. '+' | (sum),
    // dabei startet sum eine forschleife, diese erstellt ein temp result mit einem deepcopy des ersten wertes
    // und verknüpft alle weiteren member über addUp
    // addUp ist dann für zwei argumente deklariert und bearbeitet das erste direkt
    // andere Option :
    // direkt verkettende Funktion hinterlegen
    // und schleife, die diese Aufruft in default Logik einbetten
    g_ArgChainOperations = {

        {"+", "__addAssign__"},
        {"-", "__subAssign__"},
        {"*", "__mulAssign__"},
        {"/", "__divAssign__"},
        {"^", "__expAssign__"},
        
        // Bools
        {"&&", "__andAssign__"},
        {"||", "__orAssign__"},
        {"x|", "__xorAssign__"},
        {"!&", "__nandAssign__"},
        {"!|", "__norAssign__"},
        {"!x|", "__nxorAssign__"},
        
        // // Bools
        // {"and", "__andAssign__"},
        // {"or", "__orAssign__"},
        // {"xor", "__xorAssign__"},
        // {"nand", "__nandAssign__"},
        // {"nor", "__norAssign__"},
        // {"nxor", "__nxorAssign__"},

        {".", "__dotProductAssign__"},
        {".n", "__contractingDotProductAssign__"},
        {"..", "__mirroringDoubleContractionAssign__"},
        {":", "__crossingDoubleContractionAssign__"},
        {"\\x", "__crossProductAssign__"},
        {"\\(x)", "__dyadProductAssign__"},
        {"°", "__traceAssign__"},
        {"\\diff", "__diffAssign__"},
    };
}