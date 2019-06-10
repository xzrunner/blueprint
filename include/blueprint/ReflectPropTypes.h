#pragma once

namespace bp
{

enum class VariantType
{
    Void = 0,
    Bool,
    Int,
    Float1,
    Float2,
    Float3,
    Float4,
};

void prop_types_regist_rttr();

}
