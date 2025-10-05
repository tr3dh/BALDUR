// #include "INT.h"
// #include "BOOL.h"

// namespace types{

//     bool INT::setUpClass(){

//         // register in TypeRegister
//         if(!init("int", [](){ return new INT(); })){ return false; }

//         // Setze Funktionen auf
//         registerFunction("equals", {typeIndex, typeIndex},

//             [__functionLabel__ = "bigger", __numArgs__ = 2](std::vector<IObject*>& returns, const std::vector<IObject*>& inputs, IObject* member){

//             // Asserts
//             ASSERT_IS_NO_MEMBER_FUNCTION;
//             ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

//             // Casts
//             INT* inst1 = static_cast<INT*>(inputs[0]);
//             INT* inst2 = static_cast<INT*>(inputs[1]);

//             //
//             PERMISSIVLY_RESIZE_RETURNS(1);

//             //
//             if(returns[0]->getTypeIndex() != typeIndex){
//                 returns[0] =
//             }

//             //
//             inst1->getMember() = inst1->getMember() == inst2->getMember();
//         });

//         // Setze Funktionen auf
//         registerFunction("bigger", {typeIndex, typeIndex},

//             [__functionLabel__ = "bigger", __numArgs__ = 2](std::vector<IObject*>& returns, const std::vector<IObject*>& inputs, IObject* member){

//             // Asserts
//             ASSERT_IS_NO_MEMBER_FUNCTION;
//             ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

//             // Casts
//             INT* inst1 = static_cast<INT*>(inputs[0]);
//             INT* inst2 = static_cast<INT*>(inputs[1]);

//             //
//             inst1->getMember() = inst1->getMember() > inst2->getMember();
//         });

//         // Setze Funktionen auf
//         registerFunction("biggerEquals", {typeIndex, typeIndex},

//             [__functionLabel__ = "bigger", __numArgs__ = 2](std::vector<IObject*>& returns, const std::vector<IObject*>& inputs, IObject* member){

//             // Asserts
//             ASSERT_IS_NO_MEMBER_FUNCTION;
//             ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

//             // Casts
//             INT* inst1 = static_cast<INT*>(inputs[0]);
//             INT* inst2 = static_cast<INT*>(inputs[1]);

//             //
//             inst1->getMember() = inst1->getMember() >= inst2->getMember();
//         });

//         // Setze Funktionen auf
//         registerFunction("smaller", {typeIndex, typeIndex},

//             [__functionLabel__ = "bigger", __numArgs__ = 2](std::vector<IObject*>& returns, const std::vector<IObject*>& inputs, IObject* member){

//             // Asserts
//             ASSERT_IS_NO_MEMBER_FUNCTION;
//             ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

//             // Casts
//             INT* inst1 = static_cast<INT*>(inputs[0]);
//             INT* inst2 = static_cast<INT*>(inputs[1]);

//             //
//             inst1->getMember() = inst1->getMember() < inst2->getMember();
//         });

//         // Setze Funktionen auf
//         registerFunction("smallerEquals", {typeIndex, typeIndex},

//             [__functionLabel__ = "bigger", __numArgs__ = 2](std::vector<IObject*>& returns, const std::vector<IObject*>& inputs, IObject* member){

//             // Asserts
//             ASSERT_IS_NO_MEMBER_FUNCTION;
//             ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

//             // Casts
//             INT* inst1 = static_cast<INT*>(inputs[0]);
//             INT* inst2 = static_cast<INT*>(inputs[1]);

//             //
//             inst1->getMember() = inst1->getMember() <= inst2->getMember();
//         });

//         // Setze Funktionen auf
//         registerFunction("or", {typeIndex, typeIndex},

//             [__functionLabel__ = "bigger", __numArgs__ = 2](std::vector<IObject*>& returns, const std::vector<IObject*>& inputs, IObject* member){

//             // Asserts
//             ASSERT_IS_NO_MEMBER_FUNCTION;
//             ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

//             // Casts
//             INT* inst1 = static_cast<INT*>(inputs[0]);
//             INT* inst2 = static_cast<INT*>(inputs[1]);

//             //
//             inst1->getMember() = inst1->getMember() || inst2->getMember();
//         });

//         // Setze Funktionen auf
//         registerFunction("and", {typeIndex, typeIndex},

//             [__functionLabel__ = "bigger", __numArgs__ = 2](std::vector<IObject*>& returns, const std::vector<IObject*>& inputs, IObject* member){

//             // Asserts
//             ASSERT_IS_NO_MEMBER_FUNCTION;
//             ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

//             // Casts
//             INT* inst1 = static_cast<INT*>(inputs[0]);
//             INT* inst2 = static_cast<INT*>(inputs[1]);

//             //
//             inst1->getMember() = inst1->getMember() && inst2->getMember();
//         });

//         //
//         return true;
//     }
// };