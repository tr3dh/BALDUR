#include "VOID.h"

namespace types{

    std::unique_ptr<IObject> VOID::nullRef = nullptr;
    const std::string VOID::nullRefKeyword;
    
    bool VOID::setUpClass(){

        // register in TypeRegister
        if(!init("void", [](){ return new VOID(); })){ return false; }

        // Keyword
        VOID::nullRef = std::make_unique<VOID>();
        VOID::nullRef.reset(new VOID);

        return true;
    }
}

bool IsReferenceValid(std::unique_ptr<IObject>* ptr){

    return ptr != &g_nullRefs[ptr->get()->getTypeIndex()];
}