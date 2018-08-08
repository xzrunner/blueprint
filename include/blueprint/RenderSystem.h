#pragma once

#include <SM_Matrix2D.h>

namespace bp
{

class Node;

class RenderSystem
{
public:
	static void Draw(const Node& node, const sm::Matrix2D& mat);

}; // RenderSystem

}