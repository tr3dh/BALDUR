#include "Interpreter.h"

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

LSPData g_LSPData;
std::string g_lspEncoderKey = "*__$§%//BLD\\\\%§$*__";

void processScopeBeforeDeletion(Scope* scope){

    g_LSPData.addScope(scope);
}

std::vector<std::unique_ptr<IObject>> executeDistroProgram(const std::string& scriptPath){

    // ByteSequence Setup
    G_BYTESEQ_ASSERT_HANDLER = triggerAssertHandler;
    g_processScopeBeforeDeletion = processScopeBeforeDeletion;

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
    Scope distroScope = {};

    // Eintragen / Linken uniformisierter Größen zur Programm- / Backendsteuerung 
    distroScope.constructAndReturnVariable("g_suppressAssertionWarnings")->constructByObject(new types::BOOL(&g_suppressAssertionWarnings));
    distroScope.constructAndReturnVariable("g_terminateAfterAssertionFailed")->constructByObject(new types::BOOL(&g_terminateAfterAssertionFailed));
    distroScope.constructAndReturnVariable("g_unwrapOperands")->constructByObject(new types::BOOL(&unwrapOperands));
    distroScope.constructAndReturnVariable("g_compareTemplateDependencies")->constructByObject(new types::BOOL(&g_compareTemplateDependencies));
    
    //
    ByteSequence LSPDataBs;
    g_LSPData = LSPData();

    //
    if(fs::exists(fs::path(scriptPath).parent_path().string() + "/__LSPCONFIG.BYTESEQ")){

        LSPDataBs.fromFile(fs::path(scriptPath).parent_path().string() + "/__LSPCONFIG.BYTESEQ");
        LSPDataBs.decode(g_lspEncoderKey);
        LSPDataBs -= g_LSPData;
    }

    // Aufruf des Alberich-Interpreters
    auto results = executeProgram(scriptPath, &distroScope);

    // distroScope abhandlen, wird erst am Ende der Funktion sauber dekonstruiert
    (*g_processScopeBeforeDeletion)(&distroScope);

    //
    g_LSPData.addAll();

    // Distroscope löschen und Filehandhabung für LSP regeln
    // Ab besten ergebnisse zwischen speichern und später zurückgeben
    // Clean up oder beforeDeletionHandler für distro Scope so aufrufen
    // File für LSP schreiben

    // Infos abspeichern mit Info >> TypeKeywords, Funktion etc
    // Keywords abspeichern
    // Typekeywords
    // Funktions
    // Operators
    // static / memberfunktions
    // vars
    
    // Bei Goto Defi einfach nach decl word und struct word suchen
    // gleiches Prinzip für hover doku

    //
    LSPDataBs += g_LSPData;
    LSPDataBs.encode(g_lspEncoderKey);

    // ByteSequence wird in File geschrieben
    LSPDataBs.toFile(fs::path(scriptPath).parent_path().string() + "/__LSPCONFIG.BYTESEQ");

    // Rückgabe
    return results;
}