#include "blueprint/EventBeginPlay.h"
#include "blueprint/FlowCtrlBranch.h"
#include "blueprint/node/Commentary.h"
#include "blueprint/node/SetLocalVar.h"
#include "blueprint/node/GetLocalVar.h"
#include "blueprint/node/Switch.h"
#include "blueprint/node/Function.h"
#include "blueprint/node/Input.h"
#include "blueprint/node/Output.h"

#include <ee0/ReflectPropTypes.h>

#include <js/RTTR.h>

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
REGIST_NODE_RTTI(Switch,                                                                           \
.property("toggle", &bp::node::Switch::GetToggle, &bp::node::Switch::SetToggle)                    \
(                                                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Toggle"))                                \
)
)
REGIST_NODE_RTTI(Function,                                                                         \
.property("name", &bp::node::Function::GetName, &bp::node::Function::SetName)                      \
(                                                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Name"))                                  \
)                                                                                                  \
.property("filepath", &bp::node::Function::GetFilepath, &bp::node::Function::SetFilepath)          \
(                                                                                                  \
    rttr::metadata(js::RTTR::FilePathTag(), true),                                                 \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Filepath")),                             \
    rttr::metadata(ee0::PropOpenFileTag(), ee0::PropOpenFile("*.json"))                            \
)
)
REGIST_NODE_RTTI(Input,                                                                            \
.property("name", &bp::node::Input::GetName, &bp::node::Input::SetName)                            \
(                                                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Name"))                                  \
)                                                                                                  \
.property("type", &bp::node::Input::GetType, &bp::node::Input::SetType)                            \
(                                                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo(bp::node::Input::STR_TYPE))               \
)
)
REGIST_NODE_RTTI(Output,                                                                           \
.property("name", &bp::node::Output::GetName, &bp::node::Output::SetName)                          \
(                                                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Name"))                                  \
)                                                                                                  \
.property("type", &bp::node::Output::GetType, &bp::node::Output::SetType)                          \
(                                                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo(bp::node::Output::STR_TYPE))              \
)
)

}

namespace bp
{

void regist_rttr()
{
}

}
