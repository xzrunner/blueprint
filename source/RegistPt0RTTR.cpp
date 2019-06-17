#include <painting0/Color.h>

#include <rttr/registration.h>

RTTR_REGISTRATION
{

rttr::registration::class_<pt0::Color>("pt0_color")
	.constructor()(rttr::policy::ctor::as_object)
	.property("r", &pt0::Color::r)
    .property("g", &pt0::Color::g)
    .property("b", &pt0::Color::b)
    .property("a", &pt0::Color::a)
;

}

namespace bp
{

void regist_pt0_rttr()
{
}

}
