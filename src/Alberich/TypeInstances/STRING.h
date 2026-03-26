#pragma once

#include "InstanceIncludes.h"
#include "BOOL.h"
#include "INT.h"
#include "DOUBLE.h"

namespace types{

    class STRING : public INativeObject<STRING, std::string>{

    public:

        static bool setUpClass();

        STRING() = default;
        STRING(const std::string& str) : INativeObject(str){}
        STRING(std::string* Ptr) : INativeObject(Ptr){}

        // virtual ist redundant, die prdouble bleibt überscheibbar
        void print() const override{

            LOG << getMember();
        }
    };
};