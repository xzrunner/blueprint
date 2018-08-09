#include "blueprint/FlowCtrlBranch.h"
#include "blueprint/Pins.h"

namespace bp
{
namespace node
{

FlowCtrlBranch::FlowCtrlBranch()
	: Node("Branch")
{
	m_input     = std::make_shared<Pins>(PINS_PORT, "", *this);
	m_condition = std::make_shared<Pins>(PINS_BOOLEAN, "Condition", *this);
	AddPins(m_input, true);
	AddPins(m_condition, true);

	m_true  = std::make_shared<Pins>(PINS_PORT, "True", *this);
	m_false = std::make_shared<Pins>(PINS_PORT, "False", *this);
	AddPins(m_true, false);
	AddPins(m_false, false);

	Layout();
}

}
}