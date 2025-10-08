#include "gatheredInstances.h"

void setUpTypes(){

    ASSERT(types::VOID::setUpClass(), "Type VOID konnte nicht aufgesetzt werden");
    ASSERT(types::BOOL::setUpClass(), "Type BOOL konnte nicht aufgesetzt werden");
    ASSERT(types::INT::setUpClass(), "Type INT konnte nicht aufgesetzt werden");
    ASSERT(types::DOUBLE::setUpClass(), "Type DOUBLE konnte nicht aufgesetzt werden");

    emplaceStdOperations();
}