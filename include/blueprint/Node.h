#pragma once

#include <SM_Vector.h>
#include <painting2/Color.h>

#include <vector>
#include <memory>

namespace bp
{

class Node
{
public:
	virtual ~Node() {}

	void SetStyle(const sm::vec2& size, const pt2::Color& color);

public:
	struct Style
	{
		sm::vec2   size;
		pt2::Color color;
	};

	struct Pins
	{
		Pins(const std::string& name, const Node& parent)
			: name(name), parent(parent) {}

		std::string           name;
		std::shared_ptr<Pins> connected = nullptr;

		const Node& parent;
	};

	const Style& GetStyle() const { return m_style; }

protected:
	Style m_style;

	std::vector<std::shared_ptr<Pins>> m_all_input;
	std::vector<std::shared_ptr<Pins>> m_all_output;

}; // Node

}