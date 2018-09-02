#pragma once

#include "Pins.h"

#include <cu/cu_macro.h>
#include <SM_Matrix2D.h>
#include <painting2/Textbox.h>

#include <map>

namespace bp
{

namespace node { class Node; class Pins; }

class RenderSystem
{
public:
	void DrawNode(const node::Node& node, const sm::Matrix2D& mat);

	auto& GetTitleTB() const { return m_title_tb; }
	auto& GetInputTB() const { return m_input_tb; }

	float GetTextTitleScale() const;
	float GetTextPinsScale() const;

	void RegisterPinsColor(const std::map<int, pt2::Color>& pins_type2color);

	pt2::Color GetPinsColor(int type);

private:
	void InitPinsType2Color();

	void DrawPanel(const node::Node& node, const sm::vec2& pos, float hw, float hh);
	void DrawPins(const node::Pins& pins, const sm::vec2& pos);
	void DrawConnecting(const node::Node& node, const sm::Matrix2D& mat);

private:
	pt2::Textbox m_title_tb, m_small_title_tb;
	pt2::Textbox m_input_tb, m_output_tb;

	std::map<int, pt2::Color> m_pins_type2color;

	CU_SINGLETON_DECLARATION(RenderSystem)

}; // RenderSystem

}