#include "gatheredInstances.h"

void setUpTypes(){

    ASSERT(types::VOID::setUpClass(), "Type VOID konnte nicht aufgesetzt werden");
    ASSERT(types::ARGS::setUpClass(), "Type ARGS konnte nicht aufgesetzt werden");
    ASSERT(types::BOOL::setUpClass(), "Type BOOL konnte nicht aufgesetzt werden");
    ASSERT(types::INT::setUpClass(), "Type INT konnte nicht aufgesetzt werden");
    ASSERT(types::DOUBLE::setUpClass(), "Type DOUBLE konnte nicht aufgesetzt werden");
    ASSERT(types::STRING::setUpClass(), "Type STRING konnte nicht aufgesetzt werden");
    ASSERT(types::TENSOR_EXPRESSION::setUpClass(), "Type TENSOR_EXPRESSION konnte nicht aufgesetzt werden");
    ASSERT(types::INDEX_NOTATED_TENSOR_EXPRESSION::setUpClass(), "Type INDEX_NOTATED_TENSOR_EXPRESSION konnte nicht aufgesetzt werden");
    ASSERT(types::TENSOR_EXPRESSION_EQUATION::setUpClass(), "Type TENSOR_EXPRESSION_EQUATION konnte nicht aufgesetzt werden");

    ASSERT(types::ARGS::emplaceOperations(), "Operationen für ARGS Type konnten nicht implementiert werden");
   
    emplaceStdOperations();
}