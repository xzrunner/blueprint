#include "blueprint/FlowCtrlBranch.h"
#include "blueprint/Pins.h"

namespace bp
{
namespace node
{

FlowCtrlBranch::FlowCtrlBranch()
	: Node("Branch")
{
	m_input     = std::make_shared<Pins>(true, 0, PINS_PORT, "", *this);
	m_condition = std::make_shared<Pins>(true, 1, PINS_BOOLEAN, "Condition", *this);
	AddPins(m_input);
	AddPins(m_condition);

	m_true  = std::make_shared<Pins>(false, 0, PINS_PORT, "True", *this);
	m_false = std::make_shared<Pins>(false, 1, PINS_PORT, "False", *this);
	AddPins(m_true);
	AddPins(m_false);

	Layout();
}

}
}