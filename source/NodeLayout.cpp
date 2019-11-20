#include "blueprint/NodeLayout.h"
#include "blueprint/Node.h"
#include "blueprint/RenderSystem.h"
#include "blueprint/Pin.h"

#include <painting2/Callback.h>
#include <node0/SceneNode.h>
#include <node2/CompTransform.h>

#include <algorithm>

namespace bp
{

const uint32_t NodeLayout::DEFAULT_WIDTH  = 100;
const uint32_t NodeLayout::DEFAULT_HEIGHT = 20;
const uint32_t NodeLayout::TITLE_HEIGHT   = 20;
const float    NodeLayout::PIN_RADIUS     = 5;
const float    NodeLayout::CONNECTING_BEZIER_DIST = 0.3f;

void NodeLayout::UpdateNodeStyle(Node& node)
{
	Node::Style s = node.GetStyle();

	auto& input  = node.GetAllInput();
	auto& output = node.GetAllOutput();
    if (s.hori) {
        s.line_num = std::max(input.size(), output.size());
    } else {
        s.line_num = 1;
    }
	s.height = 0;
	if (node.IsStyleOnlyTitle()) {
		s.height = static_cast<float>(NodeLayout::TITLE_HEIGHT) - 4;
	} else {
		s.height = static_cast<float>(NodeLayout::TITLE_HEIGHT + NodeLayout::DEFAULT_HEIGHT * s.line_num);
	}
    if (!s.hori && s.draw_pin_label) {
        s.height += PIN_RADIUS * 4;
    }

	auto rs = RenderSystem::Instance();
	auto& tb = rs->GetInputTB();
	float max_w = 0;
	for (int i = 0; i < s.line_num; ++i)
	{
		float w = 0;
		if (input.size() > static_cast<size_t>(i)) {
			auto sz = pt2::Callback::CalcLabelSize(input[i]->GetDesc(), tb);
			w += sz.x;
		}
		if (output.size() > static_cast<size_t>(i)) {
			auto sz = pt2::Callback::CalcLabelSize(output[i]->GetDesc(), tb);
			w += sz.x;
		}
		if (w > max_w) {
			max_w = w;
		}
	}
	s.width = max_w * rs->GetTextPinScale() + 60;

	// title
	{
		auto sz = pt2::Callback::CalcLabelSize(node.GetTitle(), rs->GetTitleTB());
		float w = sz.x * rs->GetTextTitleScale() + 20;
		if (w > s.width) {
			s.width = w;
		}
	}

	node.SetStyle(s);
}

sm::vec2 NodeLayout::GetPinPos(const Pin& pin)
{
    return GetPinPos(pin.GetParent(), pin.IsInput(), pin.GetPosIdx());
}

sm::vec2 NodeLayout::GetPinPos(const Node& node, bool is_input, size_t idx)
{
	sm::vec2 pos;

	auto& style = node.GetStyle();
	float hw = style.width  * 0.5f;
	float hh = style.height * 0.5f;
    if (node.GetStyle().hori)
    {
	    if (is_input) {
		    pos.x = -hw + PIN_RADIUS * 2;
	    } else {
		    pos.x = hw - PIN_RADIUS * 2;
	    }

	    if (!node.IsStyleOnlyTitle()) {
		    pos.y = hh - NodeLayout::DEFAULT_HEIGHT - (idx + 0.5f) * NodeLayout::DEFAULT_HEIGHT;
	    } else {
		    pos.y = hh - NodeLayout::DEFAULT_HEIGHT * 0.5f;
	    }
    }
    else
    {
        if (is_input) {
		    pos.y = hh - PIN_RADIUS;
	    } else {
		    pos.y = -hh + PIN_RADIUS;
	    }

        const size_t num = is_input ? node.GetAllInput().size() : node.GetAllOutput().size();
        pos.x = (style.width / num) * (idx + 0.5f) - hw;
    }

	return node.GetPos() + pos;
}

}