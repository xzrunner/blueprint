#include "blueprint/EventBeginPlay.h"
#include "blueprint/FlowCtrlBranch.h"
#include "blueprint/node/Commentary.h"
#include "blueprint/node/SetLocalVar.h"
#include "blueprint/node/GetLocalVar.h"

#include <ee0/ReflectPropTypes.h>

#include <rttr/registration.h>

#define REGIST_NODE_RTTI(name, prop)                          \
	rttr::registration::class_<bp::node::name>("bp::"#name)   \
		.constructor<>()                                      \
		prop                                                  \
	;

#define PROP
#define REGIST_NODE_RTTI_DEFAULT(name) REGIST_NODE_RTTI(name, PROP)

RTTR_REGISTRATION
{
	rttr::registration::class_<bp::node::EventBeginPlay>("bp_event_begin_play")
		.constructor<>()
	;

	rttr::registration::class_<bp::node::FlowCtrlBranch>("bp_flow_ctrl_branch")
		.constructor<>()
	;

REGIST_NODE_RTTI(Commentary,                                                                       \
.property("comment", &bp::node::Commentary::GetCommentText, &bp::node::Commentary::SetCommentText) \
(                                                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Comment"))                               \
)                                                                                                  \
.property("width", &bp::node::Commentary::GetWidth, &bp::node::Commentary::SetWidth)               \
(                                                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Width"))                                 \
)                                                                                                  \
.property("height", &bp::node::Commentary::GetHeight, &bp::node::Commentary::SetHeight)            \
(                                                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Height"))                                \
)
)
REGIST_NODE_RTTI(SetLocalVar,                                                                      \
.property("name", &bp::node::SetLocalVar::GetVarName, &bp::node::SetLocalVar::SetVarName)          \
(                                                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Var Name"))                              \
)
)
REGIST_NODE_RTTI(GetLocalVar,                                                                      \
.property("name", &bp::node::GetLocalVar::GetVarName, &bp::node::GetLocalVar::SetVarName)          \
(                                                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Var Name"))                              \
)
)

}

namespace bp
{

void regist_rttr()
{
}

}
