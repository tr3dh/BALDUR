#include "Interpreter.h"
#include <Windows.h>

std::string getExecutablePath() {
    
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string fullPath(buffer);
    return fullPath;
}

std::string getExecutableDir() {

    std::string fullPath = getExecutablePath();
    size_t pos = fullPath.find_last_of("\\/");
    if (pos != std::string::npos) {
        return fullPath.substr(0, pos);
    }
    return "";
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

std::string g_lspEncoderKey = "*__$§%//BLD\\\\%§$*__";

LSPData getLSPData(const std::string& path){

    //
    ByteSequence bs;
    LSPData res;

    // Laden aus parent/.LSP_CACHE/filename.BYTESEQ
    const std::string LSPCache = (fs::path(path).parent_path() / ".LSP_CACHE" / (fs::path(path).filename().string() + ".BYTESEQ")).string();

    //
    if(fs::exists(LSPCache)){

        bs.fromFile(LSPCache);
        bs.decode(g_lspEncoderKey);
        bs -= res;
    }

    return res;
}

void saveLSPData(LSPData& data, const std::string& path){

    // bisher angestellte Defis kopieren und speichern
    data.definitions = g_definitions;

    //
    ByteSequence bs;
    bs += data;
    bs.encode(g_lspEncoderKey);

    // Speichern in parent/.LSP_CACHE/filename.BYTESEQ
    bs.toFile((fs::path(path).parent_path() / ".LSP_CACHE" / (fs::path(path).filename().string() + ".BYTESEQ")).string());
}

Scope* g_distroScope; 
std::vector<LSPData*> g_LSPDatas = {};

void processScopeBeforeDeletion(Scope* scope){

    if(g_LSPDatas.empty()){ return; }
    g_LSPDatas.back()->addScope(scope);
}

void processStaticScopeBeforeDeletion(TypeIndex tpIdx, Scope* scope){

    if(g_LSPDatas.empty()){ return; }
    g_LSPDatas.back()->addStaticScope(tpIdx, scope);
}

void processMemberScopeBeforeDeletion(TypeIndex tpIdx, Scope* scope){

    if(g_LSPDatas.empty()){ return; }
    g_LSPDatas.back()->addMemberScope(tpIdx, scope);
}

void processScriptBeforeExecution(const std::string& scriptPath){

    // >> aktuellste LSPData ist immer die letzte
    g_LSPDatas.emplace_back(new LSPData());
    // *g_LSPDatas.back() = getLSPData(scriptPath);
}

void processScriptAfterExecution(const std::string& scriptPath){

    //
    g_LSPDatas.back()->addAll();

    //
    for(auto& [idx, scope] : STRUCT::attribScopes){ processMemberScopeBeforeDeletion(idx, &scope); }
    for(auto& [idx, scope] : g_staticScopes){ processStaticScopeBeforeDeletion(idx, &scope); }

    // Distroscope wird erst am Ende von 'executeProgramm' dekonstruiert. Variablen Sollen 
    (*g_processScopeBeforeDeletion)(g_distroScope);

    // Für Goto Defi einfach nach decl word und struct word suchen
    // gleiches Prinzip für hover doku

    //
    saveLSPData(*g_LSPDatas.back(), scriptPath);

    //
    delete g_LSPDatas.back();
    g_LSPDatas.erase(--g_LSPDatas.end());
}

std::unordered_multimap<std::string, Definition> g_definitions = {};

void registerDefinition(const std::string& scriptPath, const std::string& defiLabel, const std::string& defiLine, \
                        const std::pair<size_t, size_t>& defiTokenPos, size_t defiTokenLen){

    //
    // LOG << scriptPath << endln;
    // LOG << defiLabel << endln;
    // LOG << defiLine << endln;
    // LOG << defiTokenPos << endln;
    // LOG << defiTokenLen << endln;

    // LOG << scriptPath << ":" << defiTokenPos.first << ":" << defiTokenPos.second << endln;

    g_definitions.emplace(defiLabel, Definition{
        .script         = scriptPath,
        .label          = defiLabel,
        .definitionLine = defiLine,
        .defiTokenRow   = defiTokenPos.first,
        .defiTokenCol   = defiTokenPos.second,
        .defiTokenLen   = defiTokenLen
    });
}

std::vector<std::unique_ptr<IObject>> executeDistroProgram(const std::string& scriptPath){

    // Handler Setups

    // ByteSequence Assertion Handler
    G_BYTESEQ_ASSERT_HANDLER = triggerAssertHandler;

    // Script- and Scopehandler 
    g_handleScriptBeforeExecution = processScriptBeforeExecution;
    g_handleScriptAfterExecution = processScriptAfterExecution;
    g_processScopeBeforeDeletion = processScopeBeforeDeletion;

    // Defihandler
    g_handleDefinition = registerDefinition;

    //
    if(fs::exists(fs::path(scriptPath).parent_path().string() + "/__LPECONFIG.JSON")){

        //
        // LOG << "[" + getTimestamp() + "] LPEConfig gefunden, Umgebung wird aufgesetzt" << endln;

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
        // LOG << "[" + getTimestamp() + "] keine LPEConfig gefunden, Umgebung wird mit default Config aufgesetzt" << endln;
        
        //
        defaultSetupLexicalInstances();

        //
        // LOG << "[" + getTimestamp() + "] Default LPEConfig wird exportiert ..." << endln;

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
    setUpDistroTypes();

    // Distributionsscope anlegen, wird im weiteren Verlauf an den Alberich-Interpreter übergeben und dient diesem als Rootscope
    g_distroScope = new Scope();

    // Eintragen / Linken uniformisierter Größen zur Programm- / Backendsteuerung 
    g_distroScope->constructAndReturnVariable("g_suppressAssertionWarnings")->constructByObject(new types::BOOL(&g_suppressAssertionWarnings));
    g_distroScope->constructAndReturnVariable("g_terminateAfterAssertionFailed")->constructByObject(new types::BOOL(&g_terminateAfterAssertionFailed));
    g_distroScope->constructAndReturnVariable("g_unwrapOperands")->constructByObject(new types::BOOL(&unwrapOperands));
    g_distroScope->constructAndReturnVariable("g_compareTemplateDependencies")->constructByObject(new types::BOOL(&g_compareTemplateDependencies));

    // Aufruf des Alberich-Interpreters
    auto results = executeProgram(scriptPath, g_distroScope);

    //
    delete g_distroScope;

    // Rückgabe
    return results;
}