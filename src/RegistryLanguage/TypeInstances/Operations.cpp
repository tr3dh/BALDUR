#include "Operations.h"

//
std::vector<std::string> g_UsedOperators = {
    
    COLON,
    "=", "<<", "<>", "<-", "<+",                    // Memory Management Semantik
    "+=", "-=", "*=", "/=", "^=",                   // Ops für 2 Arg Operationen
    ".=", "..=", ":=", "\\x=", "\\(x)=", "\\(.)=",  // für Matrix Ops
    "\\diff=",
    "&=", "!&=", "|=", "!|=", "x|=", "!x|=",        // Ops für boolsche/logische 2 Arg Operationen 
    "&&", "!&", "||", "!|", "x|", "!x|",            // ...
    "==", "!=", ">=", "<=", ">", "<", "%",          // Ops für 2 Arg Vergleichs Operationen
    "+", "-", "*", "/", "^",                        // Ops für Verkettung mult Arg Operations per 2 Arg Operationen
    ".", "..", ":", "\\x", "\\(x)", "°=",           // für verkettung über Matrix ops
    "\\diff",
    "++", "--", "!",                                // Single Argument Ops
    KOMMA,                                          //
    "~", "'", "°",                                  // Ops für Index Notation
    "^~", "^'", "^°",                               // Ops für Index Notation
    "->", ">>",                                     // Zugriff auf Statics Scope / Attrib Scopes                     
};

//
std::map<std::string, std::string> g_OneArgOperations{

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
};

// Map der Form Operator | Funktionslabel
std::map<std::string, std::string> g_TwoArgOperations = {

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
std::map<std::string, std::string> g_ArgChainOperations = {

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
    
    // Bools
    {"and", "__andAssign__"},
    {"or", "__orAssign__"},
    {"xor", "__xorAssign__"},
    {"nand", "__nandAssign__"},
    {"nor", "__norAssign__"},
    {"nxor", "__nxorAssign__"},

    {".", "__dotProductAssign__"},
    {"..", "__mirroringDoubleContractionAssign__"},
    {":", "__crossingDoubleContractionAssign__"},
    {"\\x", "__crossProductAssign__"},
    {"\\(x)", "__dyadProductAssign__"},
    {"°", "__traceAssign__"},
    {"\\diff", "__diffAssign__"},
};

//
void emplaceStdOperations(){

    registerFunction("__assign__", {IObject::ARBITATRY_TYPE, IObject::ARBITATRY_TYPE},
        [__functionLabel__ = "__assign__", __numArgs__ = 2](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            //
            EvalResult& recipient = *inputs[0];
            EvalResult& source = *inputs[1];

            //
            bool recipientIsRValue = recipient.isRValue();
            bool sourceIsRValue = source.isRValue();

            //
            RETURNING_ASSERT(!recipientIsRValue, "Variable der Wert zuwewiesen wird ist Rvalue",);

            ASSERT(recipient.getTypeIndex() == types::VOID::typeIndex || recipient.getTypeIndex() == source.getTypeIndex(), 
                "narrowing conversion");

            //
            if(recipient.getVariableRef().isReference()){

                RETURNING_ASSERT(IsReferenceValid(recipient.getVariableRef().getUniqueData()), "Nicht initialisierte Recipient Referenz",);
            }

            if(source.getVariableRef().isReference()){

                RETURNING_ASSERT(IsReferenceValid(source.getVariableRef().getUniqueData()), "Nicht initialisierte Source Referenz",);
            }

            //
            if(sourceIsRValue){
                
                // RValue wird gemovt, da er eh nur temporär vorhanden ist
                recipient.getVariableRef().move(source.getVariableRef());
            }
            else{

                // deepcopy vom lvalue
                recipient.getVariableRef().clone(source.getVariableRef());
            }  
    },
    {});

    registerFunction("__reference__", {IObject::ARBITATRY_TYPE, IObject::ARBITATRY_TYPE},
        [__functionLabel__ = "__reference__", __numArgs__ = 2](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            //
            EvalResult& recipient = *inputs[0];
            EvalResult& source = *inputs[1];

            //
            bool recipientIsRValue = recipient.isRValue();
            bool sourceIsRValue = source.isRValue();

            //
            RETURNING_ASSERT(!recipientIsRValue && !sourceIsRValue, "Bei Referenzierung sind rvalues mit im Spiel ",);

            ASSERT(recipient.getTypeIndex() == types::VOID::typeIndex || recipient.getTypeIndex() == source.getTypeIndex(), 
                    "narrowing conversion");

            RETURNING_ASSERT(recipient.getVariableRef().isReference(), "Recipient bei Referenzierung ist keine Referenz",);

            if(source.getVariableRef().isReference()){

                RETURNING_ASSERT(IsReferenceValid(source.getVariableRef().getUniqueData()), "Nicht initialisierte Source Referenz",);
            }

            recipient.getVariableRef().reference(source.getVariableRef());
    },
    {});

    //
    registerFunction("__move__", {IObject::ARBITATRY_TYPE, IObject::ARBITATRY_TYPE},
        [__functionLabel__ = "__move__", __numArgs__ = 2](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            //
            EvalResult& recipient = *inputs[0];
            EvalResult& source = *inputs[1];

            //
            bool recipientIsRValue = recipient.isRValue();
            bool sourceIsRValue = source.isRValue();

            //
            RETURNING_ASSERT(!recipientIsRValue, "Bei Mov sind rvalues mit im Spiel ",);

            ASSERT(recipient.getTypeIndex() == types::VOID::typeIndex || recipient.getTypeIndex() == source.getTypeIndex(), 
                    "narrowing conversion");

            if(source.getVariableRef().isReference()){

                RETURNING_ASSERT(IsReferenceValid(source.getVariableRef().getUniqueData()), "Nicht initialisierte Source Referenz",);
            }

            recipient.getVariableRef().move(source.getVariableRef());
    },
    {});

    //
    registerFunction("__copy__", {IObject::ARBITATRY_TYPE, IObject::ARBITATRY_TYPE},
        [__functionLabel__ = "__copy__", __numArgs__ = 2](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            //
            EvalResult& recipient = *inputs[0];
            EvalResult& source = *inputs[1];

            //
            bool recipientIsRValue = recipient.isRValue();
            bool sourceIsRValue = source.isRValue();

            //
            RETURNING_ASSERT(!recipientIsRValue, "Bei Mov sind rvalues mit im Spiel ",);

            ASSERT(recipient.getTypeIndex() == types::VOID::typeIndex || recipient.getTypeIndex() == source.getTypeIndex(), 
                    "narrowing conversion");

            if(source.getVariableRef().isReference()){

                RETURNING_ASSERT(IsReferenceValid(source.getVariableRef().getUniqueData()), "Nicht initialisierte Source Referenz",);
            }

            recipient.getVariableRef().clone(source.getVariableRef());
    },
    {});

    //
    registerFunction("__move__", {IObject::ARBITATRY_TYPE},
        [__functionLabel__ = "__move__", __numArgs__ = 1](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            // PREPARE_RETURNS;

            //
            returns.emplace_back();
            returns.back().constructRValueByObject(constructRegisteredType(inputs[0]->getTypeIndex()));

            //
            EvalResult& recipient = returns.back();
            EvalResult& source = *inputs[0];

            //
            bool recipientIsRValue = recipient.isRValue();
            bool sourceIsRValue = source.isRValue();

            //
            // RETURNING_ASSERT(!recipientIsRValue, "Bei Mov sind rvalues mit im Spiel ",);

            ASSERT(recipient.getTypeIndex() == types::VOID::typeIndex || recipient.getTypeIndex() == source.getTypeIndex(), 
                    "narrowing conversion");

            if(source.getVariableRef().isReference()){

                RETURNING_ASSERT(IsReferenceValid(source.getVariableRef().getUniqueData()), "Nicht initialisierte Source Referenz",);
            }

            recipient.getVariableRef().move(source.getVariableRef());
    },
    {types::VOID::typeIndex});

    registerFunction("__swap__", {IObject::ARBITATRY_TYPE, IObject::ARBITATRY_TYPE},
        [__functionLabel__ = "__swap__", __numArgs__ = 2](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            //
            EvalResult& recipient = *inputs[0];
            EvalResult& source = *inputs[1];

            //
            bool recipientIsRValue = recipient.isRValue();
            bool sourceIsRValue = source.isRValue();

            //
            RETURNING_ASSERT(!recipientIsRValue && !sourceIsRValue, "Bei " + std::string(__functionLabel__) + " sind rvalues mit im Spiel ",);

            ASSERT(recipient.getTypeIndex() == types::VOID::typeIndex || recipient.getTypeIndex() == source.getTypeIndex(), 
                    "narrowing conversion");

            recipient.getVariableRef().swap(source.getVariableRef());
    },
    {});

    registerFunction("countArgs", {IObject::ARGS_TYPE},
        [__functionLabel__ = "countArgs", __numArgs__ = 0](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            // ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            // Returns
            GET_RETURN(types::INT, 0);

            // schreiben in returns
            ret0->getMember() = inputs.size();
    },
    {types::INT::typeIndex});

    //
    registerFunction("sizeof", {IObject::ARBITATRY_TYPE},
        [__functionLabel__ = "countArgs", __numArgs__ = 1](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            // Returns
            GET_RETURN(types::INT, 0);

            // schreiben in returns
            ret0->getMember() = (!inputs[0]->getVariableRef().isReference()) ? inputs[0]->getVariableRef().getData()->getSize() : 8;
    },
    {types::INT::typeIndex});

    //
    registerFunction("typeid", {IObject::ARBITATRY_TYPE},
        [__functionLabel__ = "countArgs", __numArgs__ = 1](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            // Returns
            GET_RETURN(types::INT, 0);

            // schreiben in returns
            ret0->getMember() = inputs[0]->getTypeIndex();
    },
    {types::INT::typeIndex});

    //
    registerFunction("typename", {IObject::ARBITATRY_TYPE},
        [__functionLabel__ = "typename", __numArgs__ = 1](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            // Returns
            GET_RETURN(types::STRING, 0);

            // schreiben in returns
            ret0->getMember() = getKeywordByTypeIndex(inputs[0]->getTypeIndex());
    },
    {types::STRING::typeIndex});

    //
    registerFunction("timeStamp", {},
        [__functionLabel__ = "typename", __numArgs__ = 0](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            // Returns
            GET_RETURN(types::STRING, 0);

            // schreiben in returns
            ret0->getMember() = getTimestamp();
    },
    {types::STRING::typeIndex});

    //
    registerFunction("log", {IObject::ARGS_TYPE},
        [__functionLabel__ = "typename", __numArgs__ = 0](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            // ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            // Returns
            for(size_t paramIdx = 0; paramIdx < inputs.size(); paramIdx++){
                
                inputs[paramIdx]->getVariableRef().getData()->print();
                LOG << (paramIdx == inputs.size() - 1 ? "\n" : "") << std::flush;
            }
    },
    {});

    //
    registerFunction("slog", {IObject::ARGS_TYPE},
        [__functionLabel__ = "typename", __numArgs__ = 0](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            // ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            // Returns
            for(size_t paramIdx = 0; paramIdx < inputs.size(); paramIdx++){
                
                inputs[paramIdx]->getVariableRef().getData()->print();
                LOG << (paramIdx == inputs.size() - 1 ? "\n" : " ") << std::flush;
            }
    },
    {});

    //
    registerFunction("logRes", {IObject::ARGS_TYPE},
        [__functionLabel__ = "typename", __numArgs__ = 0](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            // ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            // Returns
            for(size_t paramIdx = 0; paramIdx < inputs.size(); paramIdx++){
                
                LOG << *inputs[paramIdx] << endl;
            }
    },
    {});

    //
    registerFunction("logScope", {},
        [__functionLabel__ = "typename", __numArgs__ = 0](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            // ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            // PREPARE_RETURNS;

            // Returns
            LOG << returnToScope << endl;
    },
    {});

    //
    registerFunction("logRootScope", {},
        [__functionLabel__ = "typename", __numArgs__ = 0](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            // ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            // PREPARE_RETURNS;

            // Returns
            LOG << *returnToScope.getRootScope() << endl;
    },
    {});

    //
    registerFunction("assert", {types::BOOL::typeIndex, types::STRING::typeIndex},
        [__functionLabel__ = "assert", __numArgs__ = 2](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            // PREPARE_RETURNS;

            GET_ARG(types::BOOL, 0); GET_ARG(types::STRING, 1);

            // Returns
            if(!arg0->getMember()){
                _ERROR << arg1->getMember() << endl;
            }
    },
    {});

    //
    registerFunction("variableExists", {types::STRING::typeIndex},
        [__functionLabel__ = "variableExists", __numArgs__ = 1](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            GET_RETURN(types::BOOL, 0);
            GET_ARG(types::STRING, 0);

            // Returns
            ret0->getMember() = returnToScope.containsVariable(arg0->getMember());
    },
    {types::BOOL::typeIndex});

    //
    registerFunction("__copy__", {IObject::ARBITATRY_TYPE},
        [__functionLabel__ = "__copy__", __numArgs__ = 0](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            returns.back().variable.clone(inputs[0]->getVariableRef());
    },
    {types::VOID::typeIndex});

    //
    registerFunction("copy", {IObject::ARGS_TYPE},
        [__functionLabel__ = "typename", __numArgs__ = 0](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            // ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            returns.reserve(inputs.size());

            // Returns
            for(size_t paramIdx = 0; paramIdx < inputs.size(); paramIdx++){
                
                ASSERT(inputs[paramIdx]->isLValue() || inputs[paramIdx]->getVariableRef().isReference(),
                        "copy ist redundant für rvalue variable");

                returns.emplace_back();
                returns.back().variable.constructByUniquePtr(inputs[paramIdx]->getVariableRef().getData()->clone());
            }
    },
    {IObject::ARGS_TYPE});

    //
    registerFunction("__reference__", {IObject::ARBITATRY_TYPE},
        [__functionLabel__ = "__reference__", __numArgs__ = 1](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            returns.back().variable.reference(inputs[0]->getVariableRef());
    },
    {types::VOID::typeIndex});

    //
    registerFunction("reference", {IObject::ARGS_TYPE},
        [__functionLabel__ = "typename", __numArgs__ = 0](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            // ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            returns.reserve(inputs.size());

            // Returns
            for(size_t paramIdx = 0; paramIdx < inputs.size(); paramIdx++){
                
                RETURNING_ASSERT(inputs[paramIdx]->isLValue() || inputs[paramIdx]->getVariableRef().isReference(),
                                    "Referenzierung von nicht hinterlegtem rvalue variable",);

                returns.emplace_back();
                returns.back().variable.reference(inputs[paramIdx]->getVariableRef());
            }
    },
    {IObject::ARGS_TYPE});

    //
    registerFunction("logTypeRegister", {},
        [__functionLabel__ = "logTypeRegister", __numArgs__ = 0](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            LOG << g_TypeRegister << endl;
    },
    {});
}