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

	return true;
}

void Node::AddPins(const std::shared_ptr<Pins>& pins)
{
	auto& array = pins->IsInput() ? m_all_input : m_all_output;
	if (!CheckPinsName(*pins, array)) {
		assert(0);
		return;
	}
	array.push_back(pins);
}

void Node::SetWidth(float width)
{
	m_style.width = width;
}

void Node::Layout()
{
	NodeLayout::UpdateNodeStyle(*this);
}

bool Node::CheckPinsName(const Pins& src, const std::vector<std::shared_ptr<Pins>>& dst)
{
	if (src.GetType() == PINS_PORT) {
		return true;
	}

	auto& name = src.GetName();
	if (name.empty()) {
		return false;
	}

	for (auto& p : dst) {
		if (p->GetName() == name) {
			return false;
		}
	}

	return true;
}

}