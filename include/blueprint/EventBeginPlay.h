#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pin.h"

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
		AddPin(std::make_shared<Pin>(false, 0, PIN_PORT, "", *this));

		Layout();
	}

	RTTR_ENABLE(Node)

}; // EventBeginPlay

}
}