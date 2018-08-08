#pragma once

#include "blueprint/Node.h"

namespace bp
{

class BranchNode : public Node
{
public:
	BranchNode();

private:
	// input
	std::shared_ptr<Pins> m_input;
	std::shared_ptr<Pins> m_condition;

	// output
	std::shared_ptr<Pins> m_true;
	std::shared_ptr<Pins> m_false;

}; // BranchNode

}