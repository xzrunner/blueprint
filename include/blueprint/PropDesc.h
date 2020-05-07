#pragma once

namespace bp
{

struct PropDesc
{
    enum class Type
    {
        Enum,
        Region,
    };

    virtual Type GetType() const = 0;
};

}