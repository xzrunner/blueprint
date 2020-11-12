#pragma once

#include "blueprint/VarType.h"

#include <string>

namespace bp
{

struct Variant
{
    Variant() {
        memset(f4, 0, sizeof(f4));
    }

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
        float m2[4];
        float m3[9];
        float m4[16];
    };
};

}