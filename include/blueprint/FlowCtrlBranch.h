#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pins.h"

namespace bp
{
namespace node
{

class FlowCtrlBranch : public Node
{
public:
	FlowCtrlBranch()
		: Node("Branch")
	{
		AddPins(std::make_shared<Pins>(true, 0, PINS_PORT, "", *this));
		AddPins(std::make_shared<Pins>(true, 1, PINS_BOOLEAN, "Condition", *this));

		AddPins(std::make_shared<Pins>(false, 0, PINS_PORT, "True", *this));
		AddPins(std::make_shared<Pins>(false, 1, PINS_PORT, "False", *this));

		Layout();
	}

	RTTR_ENABLE(Node)

}; // FlowCtrlBranch

}
}