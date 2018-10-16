#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pins.h"

namespace bp
{
namespace node
{

class EventBeginPlay : public Node
{
public:
	EventBeginPlay()
		: Node("EventBeginPlay")
	{
		AddPins(std::make_shared<Pins>(false, 0, PINS_PORT, "", *this));

		Layout();
	}

	RTTR_ENABLE(Node)

}; // EventBeginPlay

}
}