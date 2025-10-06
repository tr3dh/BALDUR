#include "gatheredInstances.h"

void setUpTypes(){

    ASSERT(types::BOOL::setUpClass(), "Type BOOL konnte nicht aufgesetzt werden");
    ASSERT(types::INT::setUpClass(), "Type INT konnte nicht aufgesetzt werden");
}