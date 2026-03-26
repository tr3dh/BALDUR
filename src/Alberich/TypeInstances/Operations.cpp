#include "Operations.h"

//
std::vector<std::string> g_UsedOperators = {};

//
std::map<std::string, std::string> g_OneArgOperations{};

// Map der Form Operator | Funktionslabel
std::map<std::string, std::string> g_TwoArgOperations = {};

// Map der Form Operator | (Funktionslabel, verknüpfende Operation)
// zb. '+' | (sum),
// dabei startet sum eine forschleife, diese erstellt ein temp result mit einem deepcopy des ersten wertes
// und verknüpft alle weiteren member über addUp
// addUp ist dann für zwei argumente deklariert und bearbeitet das erste direkt
// andere Option :
// direkt verkettende Funktion hinterlegen
// und schleife, die diese Aufruft in default Logik einbetten
std::map<std::string, std::string> g_ArgChainOperations = {};

//
std::vector<EvalResult*> convertEvalResultsVecToPtrVec(std::vector<EvalResult>& resVec){

    //
    std::vector<EvalResult*> memberPtrs;
    memberPtrs.reserve(resVec.size());

    for(auto& m : resVec){

        memberPtrs.emplace_back(&m);
    }

    return memberPtrs;
}

//
bool emplaceStdOperations(){

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
            RETURNING_ASSERT(!recipientIsRValue && (!sourceIsRValue || source.getVariableRef().isReference()), "Bei Referenzierung sind rvalues mit im Spiel ",);

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

    registerFunction("__walrusAssign__", {IObject::ARBITATRY_TYPE, IObject::ARBITATRY_TYPE},
        [__functionLabel__ = "__walrusAssign__", __numArgs__ = 2](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

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

            //
            returns.emplace_back().setLValue(&recipient.getVariableRef());
    },
    {IObject::ARBITATRY_TYPE});

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
    registerFunction("logTypeRegister", {},
        [__functionLabel__ = "typename", __numArgs__ = 0](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            // ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            // PREPARE_RETURNS;

            // Returns
            for(const auto& [label, idx] : g_TypeRegister.typeIndices){
                LOG << "reg[" << label << "] = " << idx << endl;
            }
    },
    {});

    //
    registerFunction("logFunctionRegister", {},
        [__functionLabel__ = "typename", __numArgs__ = 0](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            // ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            // PREPARE_RETURNS;

            // Returns
            LOG << "functionRegister mit " << g_FunctionRegister.functions.size() << " Einträgen" << endl;

            for(const auto& [key, val] : g_FunctionRegister.functions){

                LOG << key.first << " [";
                for(const auto& idx : key.second){
                    LOG << idx << ", ";
                }

                LOG << "]" << endl;
            }
    },
    {});

    //
    registerFunction("logMemberFunctionRegister", {},
        [__functionLabel__ = "typename", __numArgs__ = 0](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            // ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            // PREPARE_RETURNS;

            // Returns

            for(const auto& [idx, reg] : g_MemberFunctionRegisters){
                
                LOG << "Register für tpIdx " << idx << endl;

                for(const auto& [key, val] : reg.functions){

                    LOG << key.first << " [";
                    for(const auto& idx : key.second){
                        LOG << idx << ", ";
                    }

                    LOG << "]" << endl;
                }
            }
    },
    {});

    //
    registerFunction("logStaticFunctionRegister", {},
        [__functionLabel__ = "typename", __numArgs__ = 0](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            // ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            // PREPARE_RETURNS;

            // Returns

            for(const auto& [idx, reg] : g_StaticFunctionRegisters){
                
                LOG << "Register für tpIdx " << idx << endl;

                for(const auto& [key, val] : reg.functions){

                    LOG << key.first << " [";
                    for(const auto& idx : key.second){
                        LOG << idx << ", ";
                    }

                    LOG << "]" << endl;
                }
            }
    },
    {});

    //
    registerFunction("logNullRefs", {},
        [__functionLabel__ = "typename", __numArgs__ = 0](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            // ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            // PREPARE_RETURNS;

            // Returns

            //
            LOG << "NullRefs gefunden : " << g_nullRefs.size() << endl;

            for(const auto& [idx, ref] : g_nullRefs){
                
                LOG << "nullRefs [" << idx << "] : " << ref << endl;
            }

            LOG << endl;
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
                RETURNING_ASSERT(TRIGGER_ASSERT, arg1->getMember(),);
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
        [__functionLabel__ = "__copy__", __numArgs__ = 1](FREG_ARGS){

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

            returns.back().variable.inValidate();
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

    //
    registerFunction("call", {types::STRING::typeIndex, types::ARGS::typeIndex},
        [__functionLabel__ = "call", __numArgs__ = 2](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

            GET_ARG(types::STRING, 0);
            GET_ARG(types::ARGS, 1);

            callFunction(arg0->getMember(), returns, convertEvalResultsVecToPtrVec(arg1->getMember()), returnToScope);
    },
    {});

    //
    registerFunction("callOnExtendedStack", {types::STRING::typeIndex, types::INT::typeIndex, types::ARGS::typeIndex},
        [__functionLabel__ = "callOnExtendedStack", __numArgs__ = 3](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

            GET_ARG(types::STRING, 0);  // Funktionsname
            GET_ARG(types::INT, 1);     // Stack-Größe in MB
            GET_ARG(types::ARGS, 2);    // Argumente

            // Stack Größe in Bytes
            size_t stackSize = static_cast<size_t>(arg1->getMember()) * 1024 * 1024;
            
            // Daten für Thread vorbereiten
            struct ThreadData {
                std::string functionName;
                std::vector<EvalResult*> args;
                std::vector<EvalResult>* returns;
                Scope* returnToScope;
            };
            
            ThreadData data = {
                arg0->getMember(),
                convertEvalResultsVecToPtrVec(arg2->getMember()),
                &returns,
                &returnToScope
            };
            
            // Thread Funktion
            auto threadFunc = [](void* arg) -> DWORD {
                ThreadData* d = static_cast<ThreadData*>(arg);
                callFunction(d->functionName, *(d->returns), d->args, *(d->returnToScope));
                return 0;
            };

            // Windows Thread mit erweitertem Stack
            HANDLE thread = CreateThread(
                NULL,
                stackSize,
                threadFunc,
                &data,
                STACK_SIZE_PARAM_IS_A_RESERVATION,
                NULL
            );

            WaitForSingleObject(thread, INFINITE);
            CloseHandle(thread);
    },
    {});

    //
    registerFunction("__dereference__", {IObject::ARBITATRY_TYPE},
        [__functionLabel__ = "dereference", __numArgs__ = 1](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

            if(inputs[0]->getVariableRef().isReference()){

                inputs[0]->getVariableRef().reference(&g_nullRefs[inputs[0]->getTypeIndex()]);
            }
    },
    {});

    //
    registerFunction("__delete__", {IObject::ARBITATRY_TYPE},
        [__functionLabel__ = "__delete__", __numArgs__ = 1](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

            RETURNING_ASSERT(inputs[0]->isLValue(), "...",);

            RETURNING_ASSERT(returnToScope.eraseVariable(&inputs[0]->getVariableRef()), "...",);
    },
    {});

    //
    registerFunction("__delete__", {types::STRING::typeIndex},
        [__functionLabel__ = "__delete__", __numArgs__ = 1](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

            GET_ARG(types::STRING, 0);

            RETURNING_ASSERT(returnToScope.eraseVariable(arg0->getMember()), "...",);
    },
    {});

    //
    registerFunction("__invalidate__", {IObject::ARBITATRY_TYPE},
        [__functionLabel__ = "__invalidate__", __numArgs__ = 1](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

            if(inputs[0]->getVariableRef().isReference()){

                inputs[0]->getVariableRef().getUniqueData()->reset(nullptr);
            }
            else{
                
                inputs[0]->getVariableRef().inValidate();
            }
    },
    {});

    //
    registerFunction("isInstance", {IObject::ARBITATRY_TYPE},
        [__functionLabel__ = "isInstance", __numArgs__ = 1](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            //
            GET_RETURN(types::BOOL, 0);

            ret0->getMember() = !inputs[0]->getVariableRef().isReference();
    },
    {types::BOOL::typeIndex});

    //
    registerFunction("isReference", {IObject::ARBITATRY_TYPE},
        [__functionLabel__ = "isReference", __numArgs__ = 1](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            //
            GET_RETURN(types::BOOL, 0);

            ret0->getMember() = inputs[0]->getVariableRef().isReference();
    },
    {types::BOOL::typeIndex});

    //
    registerFunction("isUniform", {IObject::ARBITATRY_TYPE},
        [__functionLabel__ = "isUniform", __numArgs__ = 1](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            //
            GET_RETURN(types::BOOL, 0);

            ret0->getMember() = inputs[0]->getVariableRef().getData()->isUniform();
    },
    {types::BOOL::typeIndex});

    //
    registerFunction("isRValue", {IObject::ARBITATRY_TYPE},
        [__functionLabel__ = "isRValue", __numArgs__ = 1](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            //
            GET_RETURN(types::BOOL, 0);

            ret0->getMember() = inputs[0]->isRValue();
    },
    {types::BOOL::typeIndex});

    //
    registerFunction("isLValue", {IObject::ARBITATRY_TYPE},
        [__functionLabel__ = "isLValue", __numArgs__ = 1](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            //
            GET_RETURN(types::BOOL, 0);

            ret0->getMember() = inputs[0]->isLValue();
    },
    {types::BOOL::typeIndex});

    //
    registerFunction("isValid", {IObject::ARBITATRY_TYPE},
        [__functionLabel__ = "isValid", __numArgs__ = 1](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            //
            GET_RETURN(types::BOOL, 0);

            ret0->getMember() = inputs[0]->isValid();
    },
    {types::BOOL::typeIndex});

    //
    registerFunction("isInValid", {IObject::ARBITATRY_TYPE},
        [__functionLabel__ = "isInValid", __numArgs__ = 1](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            //
            GET_RETURN(types::BOOL, 0);

            ret0->getMember() = !inputs[0]->isValid();
    },
    {types::BOOL::typeIndex});

    return true;
}