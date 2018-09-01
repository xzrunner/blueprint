#include "blueprint/EventBeginPlay.h"
#include "blueprint/Pins.h"

namespace bp
{
namespace node
{

const std::string EventBeginPlay::TYPE_NAME = "bp_event_begin_play";

EventBeginPlay::EventBeginPlay()
	: Node("EventBeginPlay")
{
	m_output = std::make_shared<Pins>(false, 0, PINS_PORT, "", *this);
	AddPins(m_output);

	Layout();
}

}
}