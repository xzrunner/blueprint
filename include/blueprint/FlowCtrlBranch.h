#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pin.h"

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
		AddPin(std::make_shared<Pin>(true, 0, PIN_PORT, "", *this));
		AddPin(std::make_shared<Pin>(true, 1, PIN_BOOLEAN, "Condition", *this));

		AddPin(std::make_shared<Pin>(false, 0, PIN_PORT, "True", *this));
		AddPin(std::make_shared<Pin>(false, 1, PIN_PORT, "False", *this));

		Layout();
	}

	RTTR_ENABLE(Node)

}; // FlowCtrlBranch

}
}