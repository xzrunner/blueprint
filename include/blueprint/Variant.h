#pragma once

#include "blueprint/VarType.h"

#include <string>

namespace bp
{

struct Variant
{
    VarType type;

    std::string name;

    union
    {
        bool  b;
        int   i;
        float f;
        float f2[2];
        float f3[3];
        float f4[4];
    };
};

}