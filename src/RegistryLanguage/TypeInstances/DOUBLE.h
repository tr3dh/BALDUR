#pragma once

#include "InstanceIncludes.h"
#include "BOOL.h"
#include "INT.h"

namespace types{

    class DOUBLE : public INativeObject<DOUBLE, double>{

    public:

        static bool setUpClass();

        DOUBLE() = default;
        DOUBLE(double i) : INativeObject(i){}
        DOUBLE(double* Ptr) : INativeObject(Ptr){}

        // virtual ist redundant, die prdouble bleibt überscheibbar
        void print() const override{

            LOG << getMember() << "d";
        }
    };
};