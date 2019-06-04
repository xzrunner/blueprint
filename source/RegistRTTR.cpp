#include "blueprint/EventBeginPlay.h"
#include "blueprint/FlowCtrlBranch.h"
#include "blueprint/node/Commentary.h"
#include "blueprint/node/SetReference.h"
#include "blueprint/node/GetReference.h"
#include "blueprint/node/SetValue.h"
#include "blueprint/node/GetValue.h"
#include "blueprint/node/Proxy.h"
#include "blueprint/node/Hub.h"
#include "blueprint/node/Switch.h"
#include "blueprint/node/CompareLess.h"
#include "blueprint/node/CompareGreater.h"
#include "blueprint/node/CompareEqual.h"
#include "blueprint/node/CompareNotEqual.h"
#include "blueprint/node/Boolean.h"
#include "blueprint/node/Vector1.h"
#include "blueprint/node/Vector2.h"
#include "blueprint/node/Vector3.h"
#include "blueprint/node/Vector4.h"
#include "blueprint/node/Negate.h"
#include "blueprint/node/Add.h"
#include "blueprint/node/Subtract.h"
#include "blueprint/node/Multiply.h"
#include "blueprint/node/Divide.h"
#include "blueprint/node/Combine.h"
#include "blueprint/node/Split.h"
#include "blueprint/node/Function.h"
#include "blueprint/node/Input.h"
#include "blueprint/node/Output.h"
#include "blueprint/node/Abs.h"

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
.property("title", &bp::node::Commentary::GetCommentTitle, &bp::node::Commentary::SetCommentTitle) \
(                                                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Title"))                                 \
)                                                                                                  \
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
REGIST_NODE_RTTI(SetReference,                                                                     \
.property("name", &bp::node::SetReference::GetName, &bp::node::SetReference::SetName)              \
(                                                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Name"))                                  \
)
)
REGIST_NODE_RTTI(GetReference,                                                                     \
.property("name", &bp::node::GetReference::GetName, &bp::node::GetReference::SetName)              \
(                                                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Name"))                                  \
)
)
REGIST_NODE_RTTI(SetValue,                                                                         \
.property("name", &bp::node::SetValue::GetName, &bp::node::SetValue::SetName)                      \
(                                                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Name"))                                  \
)
)
REGIST_NODE_RTTI(GetValue,                                                                         \
.property("name", &bp::node::GetValue::GetName, &bp::node::GetValue::SetName)                      \
(                                                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Name"))                                  \
)
)
REGIST_NODE_RTTI_DEFAULT(Proxy)
REGIST_NODE_RTTI_DEFAULT(Hub)
REGIST_NODE_RTTI_DEFAULT(Switch)
REGIST_NODE_RTTI_DEFAULT(CompareLess)
REGIST_NODE_RTTI_DEFAULT(CompareGreater)
REGIST_NODE_RTTI_DEFAULT(CompareEqual)
REGIST_NODE_RTTI_DEFAULT(CompareNotEqual)
REGIST_NODE_RTTI(Boolean,                                                                          \
.property("value", &bp::node::Boolean::GetValue, &bp::node::Boolean::SetValue)                     \
(                                                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Bool"))                                  \
)
)
REGIST_NODE_RTTI(Vector1,                                                                          \
.property("value", &bp::node::Vector1::GetValue, &bp::node::Vector1::SetValue)                     \
(                                                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("float"))                                 \
)
)
REGIST_NODE_RTTI(Vector2,                                                                          \
.property("value", &bp::node::Vector2::GetValue, &bp::node::Vector2::SetValue)                     \
(                                                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("vec2"))                                  \
)
)
REGIST_NODE_RTTI(Vector3,                                                                          \
.property("value", &bp::node::Vector3::GetValue, &bp::node::Vector3::SetValue)                     \
(                                                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("vec3"))                                  \
)
)
REGIST_NODE_RTTI(Vector4,                                                                          \
.property("value", &bp::node::Vector4::GetValue, &bp::node::Vector4::SetValue)                     \
(                                                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("vec4"))                                  \
)
)
REGIST_NODE_RTTI_DEFAULT(Negate)
REGIST_NODE_RTTI_DEFAULT(Add)
REGIST_NODE_RTTI_DEFAULT(Subtract)
REGIST_NODE_RTTI_DEFAULT(Multiply)
REGIST_NODE_RTTI_DEFAULT(Divide)
REGIST_NODE_RTTI_DEFAULT(Combine)
REGIST_NODE_RTTI_DEFAULT(Split)
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
REGIST_NODE_RTTI_DEFAULT(Abs)

}

namespace bp
{

void regist_rttr()
{
}

}
