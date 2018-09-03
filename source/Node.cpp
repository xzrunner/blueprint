#include "blueprint/Node.h"
#include "blueprint/NodeLayout.h"
#include "blueprint/Pins.h"
#include "blueprint/Connecting.h"

namespace bp
{

Node::Node(const std::string& title)
	: m_title(title)
{
	m_pos.MakeInvalid();
}

bool Node::SetPos(const sm::vec2& pos)
{
	if (m_pos == pos) {
		return false;
	}

	m_pos = pos;

	for (auto& in : m_all_input) {
		for (auto& c : in->GetConnecting()) {
			c->UpdateCurve();
		}
	}
	for (auto& out : m_all_output) {
		for (auto& c : out->GetConnecting()) {
			c->UpdateCurve();
		}
	}
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