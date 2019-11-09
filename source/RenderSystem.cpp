#include "blueprint/RenderSystem.h"
#include "blueprint/Node.h"
#include "blueprint/Pin.h"
#include "blueprint/NodeLayout.h"
#include "blueprint/Connecting.h"
#include "blueprint/NodeStyle.h"

#include <SM_Calc.h>
#include <painting2/RenderSystem.h>
#include <tessellation/Painter.h>
#include <cpputil/StringHelper.h>

namespace
{

const float PIN_RADIUS = bp::NodeLayout::PIN_RADIUS;

const float TEXT_TITLE_SCALE = 0.7f;
const float TEXT_PIN_SCALE  = 0.5f;

const float PIN_TEXT_OFFSET = 130;

}

namespace bp
{

CU_SINGLETON_DEFINITION(RenderSystem);

const pt0::Color RenderSystem::COL_TEXT = pt0::Color(224, 224, 224);
const pt0::Color RenderSystem::COL_ZERO = pt0::Color(0, 0, 0, 0);

RenderSystem::RenderSystem()
{
	m_title_tb.font_size = 24;
	m_title_tb.align_hori = pt2::Textbox::HA_CENTER;
	m_title_tb.width  = 500;
	m_title_tb.height = NodeLayout::DEFAULT_HEIGHT;
	m_small_title_tb = m_title_tb;
	m_small_title_tb.font_size = 18;

	m_left_tb.font_size = 24;
	m_left_tb.align_hori = pt2::Textbox::HA_LEFT;
	m_left_tb.width = 500;
	m_left_tb.height = NodeLayout::DEFAULT_HEIGHT;

    m_right_tb = m_left_tb;
    m_right_tb.align_hori = pt2::Textbox::HA_RIGHT;

    m_center_tb = m_left_tb;
    m_center_tb.align_hori = pt2::Textbox::HA_CENTER;
}

float RenderSystem::GetTextTitleScale() const
{
	return TEXT_TITLE_SCALE;
}

float RenderSystem::GetTextPinScale() const
{
	return TEXT_PIN_SCALE;
}

void RenderSystem::DrawPanel(const Node& node, const sm::vec2& pos, float hw, float hh, bool draw_text)
{
	// background
	tess::Painter pt;
    sm::vec2 r_min(-hw, -hh);
    sm::vec2 r_max(hw, hh);
    if (!node.GetStyle().hori) {
        r_min.y += PIN_RADIUS * 2;
        r_max.y -= PIN_RADIUS * 2;
    }
    pt.AddRectFilled(r_min + pos, r_max + pos, node.GetStyle().panel_bg_col.ToABGR());
	pt2::RenderSystem::DrawPainter(pt);

	// title
    if (draw_text)
    {
        sm::Matrix2D mat;
        mat.Scale(TEXT_TITLE_SCALE, TEXT_TITLE_SCALE);
        if (node.GetStyle().hori) {
            mat.Translate(pos.x, pos.y + hh - NodeLayout::TITLE_HEIGHT * 0.5f);
        } else {
            mat.Translate(pos.x, pos.y);
        }
        auto& tb = node.IsStyleSmallTitleFont() ? m_small_title_tb : m_title_tb;
        pt2::RenderSystem::DrawText(node.GetTitle(), tb, mat, COL_TEXT, COL_ZERO);
    }
}

void RenderSystem::DrawPin(const Pin& pin, const sm::vec2& pos)
{
	sm::Matrix2D mat;
	mat.Scale(TEXT_PIN_SCALE, TEXT_PIN_SCALE);
	mat.Translate(pos.x, pos.y);

    bool filling = true;
    if (pin.GetParent().GetStyle().hori) {
        filling = !pin.GetConnecting().empty();
    }

	tess::Painter pt;
	auto type = pin.GetType();
	if (type == PIN_PORT)
	{
		std::vector<sm::vec2> vertices = {
			sm::vec2(-PIN_RADIUS, PIN_RADIUS),
			sm::vec2(-PIN_RADIUS, -PIN_RADIUS),
			sm::vec2(PIN_RADIUS, 0),
		};
		for (auto& v : vertices) {
			v += pos;
		}
		uint32_t col = pin.GetColor().ToABGR();
		assert(vertices.size() % 3 == 0);
		if (filling) {
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
		if (filling) {
			pt.AddCircleFilled(pos, PIN_RADIUS, pin.GetColor().ToABGR());
		} else {
			pt.AddCircle(pos, PIN_RADIUS, pin.GetColor().ToABGR());
		}
	}
	pt2::RenderSystem::DrawPainter(pt);

    pt2::Textbox* tb = nullptr;
    float dx = 0;
    float dy = 0;
    if (pin.GetParent().GetStyle().hori)
    {
	    if (pin.IsInput()) {
            tb = &m_left_tb;
            dx = PIN_TEXT_OFFSET;
	    } else {
            tb = &m_right_tb;
            dx = -PIN_TEXT_OFFSET;
	    }
    }
    else
    {
        tb = &m_center_tb;
	    if (pin.IsInput()) {
            dy = -static_cast<float>(NodeLayout::DEFAULT_HEIGHT) + PIN_RADIUS * 2;
	    } else {
            dy =  static_cast<float>(NodeLayout::DEFAULT_HEIGHT) - PIN_RADIUS * 2;
	    }
    }

    mat.Translate(dx, dy);
    assert(tb);
    pt2::RenderSystem::DrawText(pin.GetDesc(), *tb, mat, COL_TEXT, COL_ZERO);
}

void RenderSystem::DrawConnecting(const Node& node, const sm::Matrix2D& mat, const sm::rect& screen_region)
{
	tess::Painter pt;

    // output
	for (auto& o : node.GetAllOutput()) {
		for (auto& c : o->GetConnecting()) {
            DrawConnecting(pt, *c);
		}
	}

    // input
    for (auto& i : node.GetAllInput()) {
        for (auto& c : i->GetConnecting()) {
            auto& from_pos = c->GetFrom()->GetParent().GetPos();
            if (!sm::is_point_in_rect(from_pos, screen_region)) {
                DrawConnecting(pt, *c);
            }
        }
    }

	pt2::RenderSystem::DrawPainter(pt);
}

bool RenderSystem::DrawConnecting(tess::Painter& pt, const Connecting& conn) const
{
	auto& curve = conn.GetCurve();
	if (curve.shape.GetVertices().empty()) {
        return false;
	}

    if (!conn.IsActive())
    {
        auto& vertices = curve.shape.GetVertices();
        pt.AddPolylineDash(vertices.data(), vertices.size(), BG_COLOR_DEFAULT.ToABGR(), tess::DEFAULT_LINE_WIDTH, 5);
    }
    else
    {
        auto& src = conn.GetFrom();
        auto& dst = conn.GetTo();
        assert(src && dst);
		if (src->GetType() == dst->GetType()) {
			auto& vertices = curve.shape.GetVertices();
			pt.AddPolyline(vertices.data(), vertices.size(), src->GetColor().ToABGR());
		} else {
			assert(!curve.color.empty());
			auto& vertices = curve.shape.GetVertices();
			pt.AddPolylineMultiColor(vertices.data(), curve.color.data(), vertices.size());
		}
    }
    return true;
}

}