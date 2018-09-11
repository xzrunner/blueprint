#pragma once

#include "Pins.h"

#include <cu/cu_macro.h>
#include <SM_Matrix2D.h>
#include <painting2/Textbox.h>

namespace bp
{

class Node;
class Pins;

class RenderSystem
{
public:
	auto& GetTitleTB() const { return m_title_tb; }
	auto& GetInputTB() const { return m_input_tb; }

	float GetTextTitleScale() const;
	float GetTextPinsScale() const;

	void DrawPanel(const Node& node, const sm::vec2& pos, float hw, float hh);
	void DrawPins(const Pins& pins, const sm::vec2& pos);
	void DrawConnecting(const Node& node, const sm::Matrix2D& mat);

private:
	pt2::Textbox m_title_tb, m_small_title_tb;
	pt2::Textbox m_input_tb, m_output_tb;

	CU_SINGLETON_DECLARATION(RenderSystem)

}; // RenderSystem

}