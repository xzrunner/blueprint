#include "blueprint/EventBeginPlay.h"
#include "blueprint/FlowCtrlBranch.h"

#include <rttr/registration.h>

RTTR_REGISTRATION
{
	rttr::registration::class_<bp::node::EventBeginPlay>("bp_event_begin_play")
		.constructor<>()
	;

	rttr::registration::class_<bp::node::FlowCtrlBranch>("bp_flow_ctrl_branch")
		.constructor<>()
	;
}

namespace bp
{

void regist_rttr()
{
}

}
