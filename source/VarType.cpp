#include "blueprint/VarType.h"

#include <rttr/registration.h>

RTTR_REGISTRATION
{

rttr::registration::enumeration<bp::VarType>("bp_var_type")
(
	rttr::value("void",   bp::VarType::Void),
    rttr::value("bool",   bp::VarType::Bool),
    rttr::value("int",    bp::VarType::Int),
    rttr::value("float ", bp::VarType::Float),
    rttr::value("float2", bp::VarType::Float2),
    rttr::value("float3", bp::VarType::Float3),
    rttr::value("float4", bp::VarType::Float4)
);

}

namespace bp
{
void var_type_regist_rttr()
{
}
}