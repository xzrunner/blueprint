#include "blueprint/Node.h"
#include "blueprint/NodeLayout.h"
#include "blueprint/Pins.h"
#include "blueprint/Connecting.h"
#include "blueprint/RenderSystem.h"
#include "blueprint/NodeFactory.h"

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

void Node::StoreToJson(const std::string& dir, rapidjson::Value& val,
	                   rapidjson::MemoryPoolAllocator<>& alloc) const
{
	val.SetObject();

	bool dirty = false;

	rapidjson::Value style_val;
	style_val.SetObject();
	if (IsStyleOnlyTitle()) {
		style_val.AddMember("only_title", true, alloc);
		dirty = true;
	}
	if (IsStyleSmallTitleFont()) {
		style_val.AddMember("small_title_font", true, alloc);
		dirty = true;
	}

	if (dirty) {
		val.AddMember("style", style_val, alloc);
	}
}

void Node::LoadFromJson(mm::LinearAllocator& alloc, const std::string& dir,
	                    const rapidjson::Value& val)
{
	if (val.HasMember("style"))
	{
		bool style_changed = false;
		auto& style_val = val["style"];
		if (style_val.HasMember("only_title")) {
			style_changed = true;
			SetStyleOnlyTitle(style_val["only_title"].GetBool());
		}
		if (style_val.HasMember("small_title_font")) {
			style_changed = true;
			SetStyleSmallTitleFont(style_val["small_title_font"].GetBool());
		}
		if (style_changed) {
			Layout();
		}
	}
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

void Node::Register(cpputil::ClassInfo<Node>* ci)
{
	NodeFactory::Instance()->Register(ci);
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