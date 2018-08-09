#include "blueprint/Node.h"
#include "blueprint/NodeLayout.h"

namespace bp
{
namespace node
{

Node::Node(const std::string& title)
	: m_title(title)
{
}

void Node::AddPins(const std::shared_ptr<Pins>& pins, bool input)
{
	if (input) {
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
}