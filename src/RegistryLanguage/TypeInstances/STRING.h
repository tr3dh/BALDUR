#pragma once

#include "InstanceIncludes.h"
#include "BOOL.h"
#include "INT.h"

namespace types{

    class STRING : public INativeObject<STRING, std::string>{

    public:

        static bool setUpClass();

        STRING() = default;
        STRING(const std::string& str) : INativeObject(str){}

        // virtual ist redundant, die prdouble bleibt überscheibbar
        void print() const override{

            LOG << getMember();
        }
    };
};