#include "blueprint/BranchNode.h"

namespace bp
{

BranchNode::BranchNode()
{
	m_input     = std::make_shared<Pins>("", *this);
	m_condition = std::make_shared<Pins>("Condition", *this);
	m_all_input.push_back(m_input);
	m_all_input.push_back(m_condition);

	m_true  = std::make_shared<Pins>("True", *this);
	m_false = std::make_shared<Pins>("False", *this);
	m_all_output.push_back(m_true);
	m_all_output.push_back(m_false);
}

}