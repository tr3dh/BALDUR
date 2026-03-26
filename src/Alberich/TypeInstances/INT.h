#pragma once

#include "InstanceIncludes.h"
#include "BOOL.h"

namespace types{

    class INT : public INativeObject<INT, int>{

    public:

        static bool setUpClass();

        INT() = default;
        INT(int i) : INativeObject(i){}
        INT(int* Ptr) : INativeObject(Ptr){}

        // virtual ist redundant, die print bleibt überscheibbar
        void print() const override{

            LOG << getMember() << "i";
        }
    };
};