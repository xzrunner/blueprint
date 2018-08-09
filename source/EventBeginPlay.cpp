#include "blueprint/EventBeginPlay.h"
#include "blueprint/Pins.h"

namespace bp
{
namespace node
{

EventBeginPlay::EventBeginPlay()
	: Node("EventBeginPlay")
{
	m_output = std::make_shared<Pins>(PINS_PORT, "", *this);
	AddPins(m_output, false);

	Layout();
}

}
}