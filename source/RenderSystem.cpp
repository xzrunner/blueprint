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

// pins color
const pt2::Color COL_PINS_PORT      = pt2::Color(255, 255, 255);
const pt2::Color COL_PINS_BOOLEAN   = pt2::Color(140, 0, 0);
const pt2::Color COL_PINS_INTEGER   = pt2::Color(79, 225, 174);
const pt2::Color COL_PINS_FLOAT     = pt2::Color(168, 255, 81);
const pt2::Color COL_PINS_STRING    = pt2::Color(241, 0, 205);
const pt2::Color COL_PINS_TEXT      = pt2::Color(221, 119, 164);
const pt2::Color COL_PINS_VECTOR    = pt2::Color(247, 199, 45);
const pt2::Color COL_PINS_ROTATOR   = pt2::Color(160, 175, 250);
const pt2::Color COL_PINS_TRANSFORM = pt2::Color(243, 111, 0);
const pt2::Color COL_PINS_OBJECT    = pt2::Color(56, 165, 241);

const float TEXT_TITLE_SCALE = 0.7f;
const float TEXT_PINS_SCALE  = 0.5f;

const float PINS_TEXT_OFFSET = 58;

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
	m_input_tb.width = 200;
	m_input_tb.height = NodeLayout::DEFAULT_HEIGHT;

	m_output_tb.font_size = 24;
	m_output_tb.align_hori = pt2::Textbox::HA_RIGHT;
	m_output_tb.width = 200;
	m_output_tb.height = NodeLayout::DEFAULT_HEIGHT;
}

void RenderSystem::DrawNode(const node::Node& node, const sm::Matrix2D& mat)
{
	// pos
	auto pos = mat * sm::vec2(0, 0);
	if (pos != node.GetLastPos())
	{
		// update connecting
		for (auto& in : node.GetAllInput()) {
			for (auto& c : in->GetConnecting()) {
				c->UpdateCurve();
			}
		}
		for (auto& out : node.GetAllOutput()) {
			for (auto& c : out->GetConnecting()) {
				c->UpdateCurve();
			}
		}
		node.SetLastPos(pos);
	}

	// layout
	auto& style = node.GetStyle();
	float hw = style.width  * 0.5f;
	float hh = style.height * 0.5f;

	// panel
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

void RenderSystem::RegisterPinsColor(const std::map<int, pt2::Color>& pins_type2color)
{
	m_pins_type2color.insert(pins_type2color.begin(), pins_type2color.end());
}

pt2::Color RenderSystem::GetPinsColor(int type)
{
	auto itr = m_pins_type2color.find(type);
	return itr == m_pins_type2color.end() ? COL_WHITE : itr->second;
}

void RenderSystem::InitPinsType2Color()
{
	m_pins_type2color.insert({ node::PINS_PORT,      COL_PINS_PORT });
	m_pins_type2color.insert({ node::PINS_BOOLEAN,   COL_PINS_BOOLEAN });
	m_pins_type2color.insert({ node::PINS_INTEGER,   COL_PINS_INTEGER });
	m_pins_type2color.insert({ node::PINS_FLOAT,     COL_PINS_FLOAT });
	m_pins_type2color.insert({ node::PINS_STRING,    COL_PINS_STRING });
	m_pins_type2color.insert({ node::PINS_TEXT,      COL_PINS_TEXT });
	m_pins_type2color.insert({ node::PINS_VECTOR,    COL_PINS_VECTOR });
	m_pins_type2color.insert({ node::PINS_ROTATOR,   COL_PINS_ROTATOR });
	m_pins_type2color.insert({ node::PINS_TRANSFORM, COL_PINS_TRANSFORM });
	m_pins_type2color.insert({ node::PINS_OBJECT,    COL_PINS_OBJECT });
}

void RenderSystem::DrawPanel(const node::Node& node, const sm::vec2& pos, float hw, float hh)
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

void RenderSystem::DrawPins(const node::Pins& pins, const sm::vec2& pos)
{
	sm::Matrix2D mat;
	mat.Scale(TEXT_PINS_SCALE, TEXT_PINS_SCALE);
	mat.Translate(pos.x, pos.y);

	bool connected = !pins.GetConnecting().empty();

	auto type = pins.GetType();
	if (type == node::PINS_PORT)
	{
		std::vector<sm::vec2> vertices = {
			sm::vec2(-PINS_RADIUS, PINS_RADIUS),
			sm::vec2(-PINS_RADIUS, -PINS_RADIUS),
			sm::vec2(PINS_RADIUS, 0),
		};
		for (auto& v : vertices) {
			v += pos;
		}
		pt2::PrimitiveDraw::SetColor(COL_PINS_PORT);
		if (connected) {
			pt2::PrimitiveDraw::Triangles(nullptr, vertices);
		} else {
			pt2::PrimitiveDraw::LineWidth(2);
			pt2::PrimitiveDraw::Polyline(nullptr, vertices, true);
		}
	}
	else
	{
		pt2::PrimitiveDraw::SetColor(GetPinsColor(type));
		if (connected) {
			pt2::PrimitiveDraw::Circle(nullptr, pos, PINS_RADIUS, true);
		} else {
			pt2::PrimitiveDraw::LineWidth(2);
			pt2::PrimitiveDraw::Circle(nullptr, pos, PINS_RADIUS, false);
		}
	}

	if (pins.IsInput()) {
		mat.Translate(PINS_TEXT_OFFSET, 0);
		pt2::RenderSystem::DrawText(pins.GetName(), m_input_tb, mat, COL_TEXT, COL_WHITE);
	} else {
		mat.Translate(-PINS_TEXT_OFFSET, 0);
		pt2::RenderSystem::DrawText(pins.GetName(), m_output_tb, mat, COL_TEXT, COL_WHITE);
	}
}

void RenderSystem::DrawConnecting(const node::Node& node, const sm::Matrix2D& mat)
{
	for (auto& out : node.GetAllOutput()) {
		for (auto& c : out->GetConnecting()) {
			auto& curve = c->GetCurve();
			pt2::PrimitiveDraw::SetColor(RenderSystem::GetPinsColor(out->GetType()));
			pt2::PrimitiveDraw::Polyline(nullptr, curve.GetVertices(), false);
		}
	}
}

}