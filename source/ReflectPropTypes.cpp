#include "blueprint/ReflectPropTypes.h"

#include <rttr/registration.h>

RTTR_REGISTRATION
{

rttr::registration::enumeration<bp::VariantType>("rlab_cull_type")
(
	rttr::value("void",   bp::VariantType::Void),
    rttr::value("bool",   bp::VariantType::Bool),
    rttr::value("int",    bp::VariantType::Int),
    rttr::value("float1", bp::VariantType::Float1),
    rttr::value("float2", bp::VariantType::Float2),
    rttr::value("float3", bp::VariantType::Float3),
    rttr::value("float4", bp::VariantType::Float4)
);

}

namespace bp
{
void prop_types_regist_rttr()
{
}
}