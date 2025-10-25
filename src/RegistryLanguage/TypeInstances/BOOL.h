#pragma once

#include "InstanceIncludes.h"

namespace types{

    class BOOL : public INativeObject<BOOL, bool>{

    public:

        static bool setUpClass();

        BOOL() = default;
        BOOL(bool b) : INativeObject(b){}
        BOOL(bool* bPtr) : INativeObject(bPtr){}

        // virtual ist redundant, die print bleibt überscheibbar
        void print() const override{

            LOG << (getMember() == true ? "true" : "false");
        }
    };
};