#include "blueprint/NodeLayout.h"
#include "blueprint/Node.h"
#include "blueprint/RenderSystem.h"
#include "blueprint/Pins.h"

#include <painting2/Callback.h>

#include <algorithm>

namespace bp
{

const uint32_t NodeLayout::DEFAULT_WIDTH  = 100;
const uint32_t NodeLayout::DEFAULT_HEIGHT = 20;
const uint32_t NodeLayout::TITLE_HEIGHT   = 20;
const float    NodeLayout::PINS_RADIUS    = 5;

void NodeLayout::UpdateNodeStyle(node::Node& node)
{
	node::Node::Style s;

	auto& input  = node.GetAllInput();
	auto& output = node.GetAllOutput();
	s.line_num = std::max(input.size(), output.size());
	s.height = static_cast<float>(NodeLayout::TITLE_HEIGHT + NodeLayout::DEFAULT_HEIGHT * s.line_num);

	auto rs = RenderSystem::Instance();
	auto& tb = rs->GetInputTB();
	float max_w = 0;
	for (int i = 0; i < s.line_num; ++i)
	{
		float w = 0;
		if (input.size() > static_cast<size_t>(i)) {
			auto sz = pt2::Callback::CalcLabelSize(input[i]->GetName(), tb);
			w += sz.x;
		}
		if (output.size() > static_cast<size_t>(i)) {
			auto sz = pt2::Callback::CalcLabelSize(output[i]->GetName(), tb);
			w += sz.x;
		}
		if (w > max_w) {
			max_w = w;
		}
	}
	s.width = max_w * rs->GetTextPinsScale() + 60;

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

}