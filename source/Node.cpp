#include "blueprint/Node.h"
#include "blueprint/NodeLayout.h"
#include "blueprint/Pin.h"
#include "blueprint/Connecting.h"
#include "blueprint/RenderSystem.h"

#include <node2/RenderSystem.h>

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

void Node::Draw(const ur::Device& dev, ur::Context& ctx, const n2::RenderParams& rp) const
{
	auto render = RenderSystem::Instance();

    // LOD
    int lod = 0;
    float scale = rp.GetCamScale();
    if (scale < 2.0f) {
        lod = 2;
    } else if (scale < 5.0f) {
        lod = 1;
    } else {
        lod = 0;
    }

	// panel
	float hw = m_style.width  * 0.5f;
	float hh = m_style.height * 0.5f;
	auto pos = rp.GetMatrix() * sm::vec2(0, 0);
	render->DrawPanel(dev, ctx, *this, pos, hw, hh, lod >= 2);

	// pin
	if (lod >= 2 && !IsStyleOnlyTitle())
	{
		// input
		for (auto& in : GetAllInput()) {
			render->DrawPin(dev, ctx, *in, NodeLayout::GetPinPos(*in));
		}

		// output
		for (auto& out : GetAllOutput()) {
			render->DrawPin(dev, ctx, *out, NodeLayout::GetPinPos(*out));
		}
	}

	// connecting
    if (lod >= 1) {
        render->DrawConnecting(dev, ctx, *this, rp.GetMatrix(), rp.GetScreenRegion());
    }
}

bool Node::UpdateExtInputPorts(bool is_connecting)
{
    if (!IsExtensibleInputPorts()) {
        return false;
    }

    bool dirty = false;

    if (is_connecting)
    {
        const int n = m_all_input.size();
        auto name = GenInputPinName(n);
        AddPin(std::make_shared<Pin>(true, n, PIN_ANY_VAR, name, *this));
        dirty = true;
    }
    else
    {
        if (m_all_input.size() > 2) {
            if (m_all_input.back()->GetConnecting().empty()) {
                m_all_input.pop_back();
                dirty = true;
            }
        }
    }

    return dirty;
}

void Node::PrepareExtInputPorts(int count)
{
    if (count <= static_cast<int>(m_all_input.size())) {
        return;
    }

    for (int i = m_all_input.size(); i < count; ++i) {
        auto name = GenInputPinName(i);
        AddPin(std::make_shared<Pin>(true, i, PIN_ANY_VAR, name, *this));
    }

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

bool Node::CheckPinName(const Pin& src, const std::vector<std::shared_ptr<Pin>>& dst)
{
	if (src.GetType() == PIN_PORT) {
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

void Node::AddPin(const std::shared_ptr<Pin>& pin)
{
    auto& array = pin->IsInput() ? m_all_input : m_all_output;
    if (!CheckPinName(*pin, array)) {
        assert(0);
        return;
    }
    array.push_back(pin);
}

void Node::Layout()
{
	NodeLayout::UpdateNodeStyle(*this);
}

std::string Node::GenInputPinName(size_t idx) const
{
    std::string name;
    name += static_cast<char>('A' + idx);
    return name;
}

}