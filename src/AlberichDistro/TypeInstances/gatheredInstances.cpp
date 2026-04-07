#include "gatheredInstances.h"

//
bool setupTensorBackend(){

    fetchBackend("tensorExpr");
    fetchBackend("tensorIdn");
    fetchBackend("tensorEq");

    return true;
}

void setUpDistroTypes(){

    setUpTypes();

    emplaceBackend("tensorExpr", &types::TENSOR_EXPRESSION::setUpClass);
    emplaceBackend("tensorIdn", &types::INDEX_NOTATED_TENSOR_EXPRESSION::setUpClass);
    emplaceBackend("tensorEq", &types::TENSOR_EXPRESSION_EQUATION::setUpClass);

    emplaceBackend("tensor", &setupTensorBackend);
}