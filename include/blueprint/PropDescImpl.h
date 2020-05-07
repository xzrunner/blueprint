#pragma once

#include "blueprint/PropDesc.h"

#include <vector>

namespace bp
{

struct PropEnum : public PropDesc
{
    virtual Type GetType() const override { return Type::Enum; }

    std::vector<std::string> types;
};

struct PropRegion : public PropDesc
{
    virtual Type GetType() const override { return Type::Region; }

    float min = 0, max = 0;
};

}