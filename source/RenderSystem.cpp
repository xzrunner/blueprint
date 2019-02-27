#include "blueprint/RenderSystem.h"
#include "blueprint/Node.h"
#include "blueprint/Pins.h"
#include "blueprint/NodeLayout.h"
#include "blueprint/Connecting.h"

#include <SM_Rect.h>
#include <painting2/RenderSystem.h>
#include <tessellation/Painter.h>

namespace
{

const float PINS_RADIUS = bp::NodeLayout::PINS_RADIUS;

const pt0::Color COL_TEXT     = pt0::Color(224, 224, 224);
const pt0::Color COL_ZERO     = pt0::Color(0, 0, 0, 0);

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
	tess::Painter pt;
    pt.AddRectFilled(sm::vec2(-hw, -hh) + pos, sm::vec2(hw, hh) + pos, node.GetStyle().panel_bg_col.ToABGR());
	pt2::RenderSystem::DrawPainter(pt);

	// title
	sm::Matrix2D mat;
	mat.Scale(TEXT_TITLE_SCALE, TEXT_TITLE_SCALE);
	mat.Translate(pos.x, pos.y + hh - NodeLayout::TITLE_HEIGHT * 0.5f);
	auto& tb = node.IsStyleSmallTitleFont() ? m_small_title_tb : m_title_tb;
	pt2::RenderSystem::DrawText(node.GetTitle(), tb, mat, COL_TEXT, COL_ZERO);
}

void RenderSystem::DrawPins(const Pins& pins, const sm::vec2& pos)
{
	sm::Matrix2D mat;
	mat.Scale(TEXT_PINS_SCALE, TEXT_PINS_SCALE);
	mat.Translate(pos.x, pos.y);

	bool connected = !pins.GetConnecting().empty();

	tess::Painter pt;
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
		uint32_t col = pins.GetColor().ToABGR();
		assert(vertices.size() % 3 == 0);
		if (connected) {
			for (int i = 0, n = vertices.size(); i < n; ) {
				pt.AddTriangleFilled(vertices[i++], vertices[i++], vertices[i++], col);
			}
		} else {
			for (int i = 0, n = vertices.size(); i < n; ) {
				pt.AddTriangle(vertices[i++], vertices[i++], vertices[i++], col);
			}
		}
	}
	else
	{
		if (connected) {
			pt.AddCircleFilled(pos, PINS_RADIUS, pins.GetColor().ToABGR());
		} else {
			pt.AddCircle(pos, PINS_RADIUS, pins.GetColor().ToABGR());
		}
	}
	pt2::RenderSystem::DrawPainter(pt);

	if (pins.IsInput()) {
		mat.Translate(PINS_TEXT_OFFSET, 0);
		pt2::RenderSystem::DrawText(pins.GetDesc(), m_input_tb, mat, COL_TEXT, COL_ZERO);
	} else {
		mat.Translate(-PINS_TEXT_OFFSET, 0);
		pt2::RenderSystem::DrawText(pins.GetDesc(), m_output_tb, mat, COL_TEXT, COL_ZERO);
	}
}

void RenderSystem::DrawConnecting(const Node& node, const sm::Matrix2D& mat)
{
	tess::Painter pt;
	for (auto& src : node.GetAllOutput())
	{
		for (auto& c : src->GetConnecting())
		{
			auto& curve = c->GetCurve();
			if (curve.shape.GetVertices().empty()) {
				continue;
			}
			auto& dst = c->GetTo();
			assert(dst);
			if (src->GetType() == dst->GetType()) {
				auto& vertices = curve.shape.GetVertices();
				pt.AddPolyline(vertices.data(), vertices.size(), src->GetColor().ToABGR());
			} else {
				assert(!curve.color.empty());
				auto& vertices = curve.shape.GetVertices();
				pt.AddPolylineMultiColor(vertices.data(), curve.color.data(), vertices.size());
			}
		}
	}
	pt2::RenderSystem::DrawPainter(pt);
}

}