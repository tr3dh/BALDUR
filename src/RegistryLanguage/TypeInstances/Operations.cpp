#include "Operations.h"

// Map der Form Operator | Funktionslabel
std::map<std::string, std::string> g_TwoArgOperations = {

    // Inhalte später mit Operatoren liste aus einer json Datei laden, die das Project Env darstellt
    
    {"=", "__assign__"},
    {"<<", "__reference__"},
    {"+=", "__addAssign__"},
    {"-=", "__subAssign__"},
    {"*=", "__mulAssign__"},
    {"/=", "__divAssign__"}
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
    {"/", "__divAssign__"}
};

//
void emplaceStdOperations(){

    // später weitere assign die args entgegen nimmt und diese zwei param dann für jedes zuweisungspaar aufruft

    registerFunction("__assign__", {IObject::ARGS_TYPE},
        [__functionLabel__ = "__assign__", __numArgs__ = 0](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            // ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            RETURNING_ASSERT(inputs.size() % 2 == 0, "Ungerade Arg Anzahl in Mehrfach zuweisung",);

            //
            size_t halfSize = inputs.size() / 2;

            //
            EvalResultVec res{};

            //
            for(size_t childIdx = 0; childIdx < halfSize; childIdx++){

                callFunction("__assign__", res, {inputs[childIdx], inputs[childIdx + halfSize]});
            }
    },
    {});

    registerFunction("__assign__", {IObject::ARBITATRY_TYPE, IObject::ARBITATRY_TYPE},
        [__functionLabel__ = "__assign__", __numArgs__ = 2](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

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
        [__functionLabel__ = "__reference__", __numArgs__ = 2](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

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

    registerFunction("countArgs", {IObject::ARGS_TYPE},
        [__functionLabel__ = "countArgs", __numArgs__ = 0](FunctionReturns returns, FunctionParams inputs, const std::vector<TypeIndex>& functionReturnTypes, TypeMember member){

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
}