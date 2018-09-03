#include "blueprint/Node.h"
#include "blueprint/NodeLayout.h"
#include "blueprint/Pins.h"

namespace bp
{

Node::Node(const std::string& title)
	: m_title(title)
{
	m_last_pos.MakeInvalid();
}

void Node::AddPins(const std::shared_ptr<Pins>& pins)
{
	if (pins->IsInput()) {
		m_all_input.push_back(pins);
	} else {
		m_all_output.push_back(pins);
	}
}

void Node::SetWidth(float width)
{
	m_style.width = width;
}

void Node::Layout()
{
	NodeLayout::UpdateNodeStyle(*this);
}

}