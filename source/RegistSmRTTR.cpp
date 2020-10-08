#include <SM_Matrix.h>

#include <rttr/registration.h>

RTTR_REGISTRATION
{

rttr::registration::class_<sm::mat2>("sm_mat2")
	.property("val", &sm::mat2::x)
;
rttr::registration::class_<sm::mat3>("sm_mat3")
	.property("val", &sm::mat3::x)
;
rttr::registration::class_<sm::mat4>("sm_mat4")
	.property("val", &sm::mat4::x)
;
rttr::registration::class_<sm::bvec2>("sm_bvec2")
	.constructor()(rttr::policy::ctor::as_object)
	.property("x", &sm::bvec2::x)
	.property("y", &sm::bvec2::y)
;
rttr::registration::class_<sm::bvec3>("sm_bvec3")
	.constructor()(rttr::policy::ctor::as_object)
	.property("x", &sm::bvec3::x)
	.property("y", &sm::bvec3::y)
	.property("z", &sm::bvec3::z)
;
rttr::registration::class_<sm::bvec4>("sm_bvec4")
	.constructor()(rttr::policy::ctor::as_object)
	.property("x", &sm::bvec4::x)
	.property("y", &sm::bvec4::y)
	.property("z", &sm::bvec4::z)
	.property("w", &sm::bvec4::w)
;
rttr::registration::class_<sm::vec2>("sm_vec2")
	.constructor()(rttr::policy::ctor::as_object)
	.property("x", &sm::vec2::x)
	.property("y", &sm::vec2::y)
;
rttr::registration::class_<sm::vec3>("sm_vec3")
	.constructor()(rttr::policy::ctor::as_object)
	.property("x", &sm::vec3::x)
	.property("y", &sm::vec3::y)
	.property("z", &sm::vec3::z)
;
rttr::registration::class_<sm::vec4>("sm_vec4")
	.constructor()(rttr::policy::ctor::as_object)
	.property("x", &sm::vec4::x)
	.property("y", &sm::vec4::y)
	.property("z", &sm::vec4::z)
	.property("w", &sm::vec4::w)
;

}

namespace bp
{

void regist_sm_rttr()
{
}

}
