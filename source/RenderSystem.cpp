#include "blueprint/RenderSystem.h"
#include "blueprint/Node.h"
#include "blueprint/Pins.h"
#include "blueprint/NodeLayout.h"
#include "blueprint/Connecting.h"

#include <SM_Rect.h>
#include <painting2/PrimitiveDraw.h>
#include <painting2/RenderSystem.h>

namespace
{

const float PINS_RADIUS = bp::NodeLayout::PINS_RADIUS;

const pt2::Color COL_PANEL_BG = pt2::Color(25, 25, 25, 196);

const pt2::Color COL_TEXT     = pt2::Color(224, 224, 224);
const pt2::Color COL_WHITE    = pt2::Color(255, 255, 255);

const float TEXT_TITLE_SCALE = 0.7f;
const float TEXT_PINS_SCALE  = 0.5f;

const float PINS_TEXT_OFFSET = 80;

}

namespace bp
{

CU_SINGLETON_DEFINITION(RenderSystem);

RenderSystem::RenderSystem()
{
	m_title_tb.font_size = 24;
	m_title_tb.align_hori = pt2::Textbox::HA_CENTER;
	m_title_tb.width  = 200;
	m_title_tb.height = NodeLayout::DEFAULT_HEIGHT;
	m_small_title_tb = m_title_tb;
	m_small_title_tb.font_size = 18;

	m_input_tb.font_size = 24;
	m_input_tb.align_hori = pt2::Textbox::HA_LEFT;
	m_input_tb.width = 300;
	m_input_tb.height = NodeLayout::DEFAULT_HEIGHT;

	m_output_tb.font_size = 24;
	m_output_tb.align_hori = pt2::Textbox::HA_RIGHT;
	m_output_tb.width = 300;
	m_output_tb.height = NodeLayout::DEFAULT_HEIGHT;
}

void RenderSystem::DrawNode(const Node& node, const sm::Matrix2D& mat)
{
	// layout
	auto& style = node.GetStyle();
	float hw = style.width  * 0.5f;
	float hh = style.height * 0.5f;

	// panel
	auto pos = mat * sm::vec2(0, 0);
	DrawPanel(node, pos, hw, hh);

	// input
	for (auto& in : node.GetAllInput()) {
		DrawPins(*in, NodeLayout::GetPinsPos(*in));
	}

	// output
	for (auto& out : node.GetAllOutput()) {
		DrawPins(*out, NodeLayout::GetPinsPos(*out));
	}

	// ext
	node.Draw(mat);

	// connecting
	DrawConnecting(node, mat);
}

float RenderSystem::GetTextTitleScale() const
{
	return TEXT_TITLE_SCALE;
}

float RenderSystem::GetTextPinsScale() const
{
	return TEXT_PINS_SCALE;
}

void RenderSystem::DrawPanel(const Node& node, const sm::vec2& pos, float hw, float hh)
{
	// background
	sm::rect r(sm::vec2(-hw, -hh) + pos, sm::vec2(hw, hh) + pos);
	pt2::PrimitiveDraw::SetColor(COL_PANEL_BG);
	pt2::PrimitiveDraw::Rect(nullptr, r, true);

	// title
	sm::Matrix2D mat;
	mat.Scale(TEXT_TITLE_SCALE, TEXT_TITLE_SCALE);
	mat.Translate(pos.x, pos.y + hh - NodeLayout::TITLE_HEIGHT * 0.5f);
	auto& tb = node.GetStyle().small_title ? m_small_title_tb : m_title_tb;
	pt2::RenderSystem::DrawText(node.GetTitle(), tb, mat, COL_TEXT, COL_WHITE);
}

void RenderSystem::DrawPins(const Pins& pins, const sm::vec2& pos)
{
	sm::Matrix2D mat;
	mat.Scale(TEXT_PINS_SCALE, TEXT_PINS_SCALE);
	mat.Translate(pos.x, pos.y);

	bool connected = !pins.GetConnecting().empty();

	auto type = pins.GetType();
	if (type == PINS_PORT)
	{
		std::vector<sm::vec2> vertices = {
			sm::vec2(-PINS_RADIUS, PINS_RADIUS),
			sm::vec2(-PINS_RADIUS, -PINS_RADIUS),
			sm::vec2(PINS_RADIUS, 0),
		};
		for (auto& v : vertices) {
			v += pos;
		}
		pt2::PrimitiveDraw::SetColor(pins.GetColor());
		if (connected) {
			pt2::PrimitiveDraw::Triangles(nullptr, vertices);
		} else {
			pt2::PrimitiveDraw::LineWidth(2);
			pt2::PrimitiveDraw::Polyline(nullptr, vertices, true);
		}
	}
	else
	{
		pt2::PrimitiveDraw::SetColor(pins.GetColor());
		if (connected) {
			pt2::PrimitiveDraw::Circle(nullptr, pos, PINS_RADIUS, true);
		} else {
			pt2::PrimitiveDraw::LineWidth(2);
			pt2::PrimitiveDraw::Circle(nullptr, pos, PINS_RADIUS, false);
		}
	}

	if (pins.IsInput()) {
		mat.Translate(PINS_TEXT_OFFSET, 0);
		pt2::RenderSystem::DrawText(pins.GetDesc(), m_input_tb, mat, COL_TEXT, COL_WHITE);
	} else {
		mat.Translate(-PINS_TEXT_OFFSET, 0);
		pt2::RenderSystem::DrawText(pins.GetDesc(), m_output_tb, mat, COL_TEXT, COL_WHITE);
	}
}

void RenderSystem::DrawConnecting(const Node& node, const sm::Matrix2D& mat)
{
	for (auto& src : node.GetAllOutput())
	{
		for (auto& c : src->GetConnecting())
		{
			auto& curve = c->GetCurve();
			if (curve.shape.GetVertices().empty()) {
				continue;
			}
			auto& dst = c->GetTo();
			if (src->GetType() == dst->GetType()) {
				pt2::PrimitiveDraw::SetColor(src->GetColor());
				pt2::PrimitiveDraw::Polyline(nullptr, curve.shape.GetVertices(), false);
			} else {
				assert(!curve.color.empty());
				pt2::PrimitiveDraw::Polyline(nullptr, curve.shape.GetVertices(), curve.color, false);
			}
		}
	}
}

}