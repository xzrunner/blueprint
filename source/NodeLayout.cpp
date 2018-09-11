#include "blueprint/NodeLayout.h"
#include "blueprint/Node.h"
#include "blueprint/RenderSystem.h"
#include "blueprint/Pins.h"

#include <painting2/Callback.h>
#include <node0/SceneNode.h>
#include <node2/CompTransform.h>

#include <algorithm>

namespace bp
{

const uint32_t NodeLayout::DEFAULT_WIDTH  = 100;
const uint32_t NodeLayout::DEFAULT_HEIGHT = 20;
const uint32_t NodeLayout::TITLE_HEIGHT   = 20;
const float    NodeLayout::PINS_RADIUS    = 5;
const float    NodeLayout::CONNECTING_BEZIER_DIST = 0.3f;

void NodeLayout::UpdateNodeStyle(Node& node)
{
	Node::Style s;

	auto& input  = node.GetAllInput();
	auto& output = node.GetAllOutput();
	s.line_num = std::max(input.size(), output.size());
	s.height = 0;
	if (node.IsStyleOnlyTitle()) {
		s.height = static_cast<float>(NodeLayout::TITLE_HEIGHT) - 4;
	} else {
		s.height = static_cast<float>(NodeLayout::TITLE_HEIGHT + NodeLayout::DEFAULT_HEIGHT * s.line_num);
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

sm::vec2 NodeLayout::GetPinsPos(const Pins& pins)
{
	auto& node = pins.GetParent();

	auto& style = node.GetStyle();
	float hw = style.width  * 0.5f;
	float hh = style.height * 0.5f;

	sm::vec2 pos;

	bool left = pins.IsInput();
	if (left) {
		pos.x = -hw + PINS_RADIUS * 2;
	} else {
		pos.x = hw - PINS_RADIUS * 2;
	}

	if (!node.IsStyleOnlyTitle()) {
		int pos_idx = pins.GetPosIdx();
		pos.y = hh - NodeLayout::DEFAULT_HEIGHT - (pos_idx + 0.5f) * NodeLayout::DEFAULT_HEIGHT;
	} else {
		pos.y = hh - NodeLayout::DEFAULT_HEIGHT * 0.5f;
	}

	return node.GetPos() + pos;
}

sm::vec2 NodeLayout::GetPinsPos(const Node& node, bool left, size_t idx)
{
	sm::vec2 pos;

	auto& style = node.GetStyle();
	float hw = style.width  * 0.5f;
	float hh = style.height * 0.5f;

	if (left) {
		pos.x = -hw + PINS_RADIUS * 2;
	} else {
		pos.x = hw - PINS_RADIUS * 2;
	}

	if (!node.IsStyleOnlyTitle()) {
		pos.y = hh - NodeLayout::DEFAULT_HEIGHT - (idx + 0.5f) * NodeLayout::DEFAULT_HEIGHT;
	} else {
		pos.y = hh - NodeLayout::DEFAULT_HEIGHT * 0.5f;
	}

	return node.GetPos() + pos;
}

}