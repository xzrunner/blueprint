#include "blueprint/Node.h"

namespace bp
{

void Node::SetStyle(const sm::vec2& size, const pt2::Color& color)
{
	m_style.size  = size;
	m_style.color = color;
}

}