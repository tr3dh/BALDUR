#include "gatheredInstances.h"

//
bool setUpCore(){

    fetchBackend("void");
    fetchBackend("args");
    fetchBackend("bool");
    fetchBackend("int");
    fetchBackend("double");
    fetchBackend("string");

    fetchBackend("argOps");
    fetchBackend("stdOps");

    return true;
}

//
bool setupTensorBackend(){

    fetchBackend("tensorExpr");
    fetchBackend("tensorIdn");
    fetchBackend("tensorEq");

    return true;
}

void setUpTypes(){

    RETURNING_ASSERT(setUpBackendRegister(), "Backendregister konnte nicht aufgesetzt werden",);

    emplaceBackend("void", &types::VOID::setUpClass);
    emplaceBackend("args", &types::ARGS::setUpClass);
    emplaceBackend("bool", &types::BOOL::setUpClass);
    emplaceBackend("int", &types::INT::setUpClass);
    emplaceBackend("double", &types::DOUBLE::setUpClass);
    emplaceBackend("string", &types::STRING::setUpClass);

    emplaceBackend("argOps", &types::ARGS::emplaceOperations);
    emplaceBackend("stdOps", &emplaceStdOperations);

    emplaceBackend("tensorExpr", &types::TENSOR_EXPRESSION::setUpClass);
    emplaceBackend("tensorIdn", &types::INDEX_NOTATED_TENSOR_EXPRESSION::setUpClass);
    emplaceBackend("tensorEq", &types::TENSOR_EXPRESSION_EQUATION::setUpClass);

    emplaceBackend("core", &setUpCore);
    fetchBackend("core");

    emplaceBackend("tensor", &setupTensorBackend);
}