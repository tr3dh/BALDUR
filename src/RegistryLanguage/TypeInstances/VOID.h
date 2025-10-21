#pragma once

#include "InstanceIncludes.h"

#ifdef VOID
#undef VOID
#endif

bool IsReferenceValid(std::unique_ptr<IObject>* ptr);

namespace types{

    class VOID : public IIndexedObject<VOID>{

    public:

        static bool setUpClass();

        static std::unique_ptr<IObject> nullRef;
        const static std::string nullRefKeyword;

        VOID() = default;

        std::unique_ptr<IObject> clone() override{
            return std::make_unique<VOID>();
        };

        // virtual ist redundant, die print bleibt überscheibbar
        void print() const override{
            LOG << "VOID_MEMBER";
        }
    };
};