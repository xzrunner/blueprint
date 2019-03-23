#pragma once

#include "Pins.h"

#include <cu/cu_macro.h>
#include <SM_Matrix2D.h>
#include <SM_Rect.h>
#include <painting2/Textbox.h>

namespace tess { class Painter; }

namespace bp
{

class Node;
class Pins;
class Connecting;

class RenderSystem
{
public:
	auto& GetTitleTB() const { return m_title_tb; }
	auto& GetInputTB() const { return m_input_tb; }

	float GetTextTitleScale() const;
	float GetTextPinsScale() const;

	void DrawPanel(const Node& node, const sm::vec2& pos, float hw, float hh, bool draw_text);
	void DrawPins(const Pins& pins, const sm::vec2& pos);
	void DrawConnecting(const Node& node, const sm::Matrix2D& mat, const sm::rect& screen_region);

public:
    static const pt0::Color COL_TEXT;
    static const pt0::Color COL_ZERO;

private:
    bool DrawConnecting(tess::Painter& pt, const Connecting& conn) const;

private:
	pt2::Textbox m_title_tb, m_small_title_tb;
	pt2::Textbox m_input_tb, m_output_tb;

	CU_SINGLETON_DECLARATION(RenderSystem)

}; // RenderSystem

}