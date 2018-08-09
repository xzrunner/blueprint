#pragma once

#include <SM_Vector.h>
#include <painting2/Color.h>

#include <vector>
#include <memory>

namespace bp
{
namespace node
{

class Pins;

class Node
{
public:
	Node(const std::string& title);
	virtual ~Node() {}

	auto& GetTitle() const { return m_title; }

	auto& GetAllInput() const { return m_all_input; }
	auto& GetAllOutput() const { return m_all_output; }

public:
	struct Style
	{
		float width;
		float height;

		int line_num;
	};

	const Style& GetStyle() const { return m_style; }
	void SetStyle(const Style& style) { m_style = style; }

protected:
	void AddPins(const std::shared_ptr<Pins>& pins, bool input);

	void SetWidth(float width);

	void Layout();

private:
	std::string m_title;

	Style m_style;

	std::vector<std::shared_ptr<Pins>> m_all_input;
	std::vector<std::shared_ptr<Pins>> m_all_output;

}; // Node

}
}