#pragma once

#include "InstanceIncludes.h"
#include "BOOL.h"

namespace types{

    class DOUBLE : public INativeObject<DOUBLE, double>{

    public:

        static double setUpClass();

        DOUBLE() = default;
        DOUBLE(double i) : INativeObject(i){}

        // virtual ist redundant, die prdouble bleibt überscheibbar
        void print() const override{

            LOG << getMember() << "d";
        }
    };
};