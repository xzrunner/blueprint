#include "blueprint/Node.h"
#include "blueprint/NodeLayout.h"
#include "blueprint/Pins.h"
#include "blueprint/Connecting.h"
#include "blueprint/RenderSystem.h"

namespace bp
{

Node::Node(const std::string& title)
	: m_title(title)
{
	m_pos.MakeInvalid();
}

void Node::Draw(const sm::Matrix2D& mt) const
{
	auto render = RenderSystem::Instance();

	// panel
	float hw = m_style.width  * 0.5f;
	float hh = m_style.height * 0.5f;
	auto pos = mt * sm::vec2(0, 0);
	render->DrawPanel(*this, pos, hw, hh);

	// pins
	if (!IsStyleOnlyTitle())
	{
		// input
		for (auto& in : GetAllInput()) {
			render->DrawPins(*in, NodeLayout::GetPinsPos(*in));
		}

		// output
		for (auto& out : GetAllOutput()) {
			render->DrawPins(*out, NodeLayout::GetPinsPos(*out));
		}
	}

	// connecting
	render->DrawConnecting(*this, mt);
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