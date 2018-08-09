#pragma once

#include "blueprint/Node.h"

namespace bp
{
namespace node
{

class FlowCtrlBranch : public Node
{
public:
	FlowCtrlBranch();

private:
	// input
	std::shared_ptr<Pins> m_input;
	std::shared_ptr<Pins> m_condition;

	// output
	std::shared_ptr<Pins> m_true;
	std::shared_ptr<Pins> m_false;

}; // FlowCtrlBranch

}
}