#include "blueprint/Node.h"
#include "blueprint/NodeLayout.h"
#include "blueprint/Pins.h"
#include "blueprint/Connecting.h"
#include "blueprint/RenderSystem.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<bp::Node::Flags>("bp_node_flags")
		.property("only_title", &bp::Node::Flags::only_title)
		.property("small_title_font", &bp::Node::Flags::small_title_font)
	;
	rttr::registration::class_<bp::Node>("bp_node")
		.property("style", &bp::Node::GetFlags, &bp::Node::SetFlags)
	;
}

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

Node::Flags Node::GetFlags() const
{
	return Flags({ IsStyleOnlyTitle(), IsStyleSmallTitleFont() });
}

void Node::SetFlags(Flags flags)
{
	SetStyleOnlyTitle(flags.only_title);
	SetStyleSmallTitleFont(flags.small_title_font);

	Layout();
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

void Node::Layout()
{
	NodeLayout::UpdateNodeStyle(*this);
}

}