#include "blueprint/EventBeginPlay.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<bp::node::EventBeginPlay>("bp_event_begin_play")
		.constructor<>()
	;
}