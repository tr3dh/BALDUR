#pragma once

#include "InstanceIncludes.h"

namespace types{

    class INT : public INativeObject<INT, int>{

    public:

        static bool setUpClass(){

            return true;
        };

        INT() = default;
        INT(int i) : INativeObject(i){}
    };
};