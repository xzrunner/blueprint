#pragma once

namespace bp
{

enum class VarType
{
    Void = 0,
    Bool,
    Int,
    Float,
    Float2,
    Float3,
    Float4,
    Matrix2,
    Matrix3,
    Matrix4,
};

void var_type_regist_rttr();

}
