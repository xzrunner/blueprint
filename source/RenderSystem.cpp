#include "blueprint/RenderSystem.h"
#include "blueprint/Node.h"

#include <SM_Rect.h>
#include <painting2/PrimitiveDraw.h>

namespace bp
{

void RenderSystem::Draw(const Node& node, const sm::Matrix2D& mat)
{
	auto& style = node.GetStyle();

	float hw = style.size.x * 0.5f;
	float hh = style.size.y * 0.5f;
	sm::rect r(mat * sm::vec2(-hw, -hh), mat * sm::vec2(hw, hh));

	pt2::PrimitiveDraw::SetColor(style.color);
	pt2::PrimitiveDraw::Rect(nullptr, r, true);
}

}